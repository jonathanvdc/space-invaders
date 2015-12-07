#include "SceneDescription.h"

#include <experimental/filesystem>
#include <exception>
#include <fstream>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <SFML/Graphics.hpp>
#include "tinyxml2/tinyxml2.h"
#include "Common.h"
#include "Entity.h"
#include "ProjectileEntity.h"
#include "ShipEntity.h"
#include "IController.h"
#include "PlayerController.h"
#include "ShipCollisionController.h"
#include "OutOfBoundsController.h"
#include "IRenderable.h"
#include "Scene.h"
#include "SpriteRenderable.h"
#include "RelativeBoxRenderable.h"

using namespace si;

/// Creates an XML parsing exception from the given
/// data.
XMLParseException::XMLParseException(
	const std::string& errorName,
	const std::string& errorStr1, const std::string& errorStr2)
	: errorName(errorName),
	errorStr1(errorStr1), errorStr2(errorStr2)
{
	std::string msg = this->errorName;
	if (this->errorStr1.size() > 0)
	{
		msg += " - " + this->errorStr1;
	}
	if (this->errorStr2.size() > 0)
	{
		msg += " - " + this->errorStr2;
	}
	this->std::exception::exception(msg.c_str());
}

/// Creates an XML parsing exception from the error in the
/// given XML document.
XMLParseException::XMLParseException(const tinyxml2::XMLDocument& doc)
	: XMLParseException(
		doc.ErrorName(),
		doc.GetErrorStr1(), doc.GetErrorStr2())
{ }

/// Gets this XML parsing exception's
/// error name.
std::string XMLParseException::getErrorName() const
{
	return this->errorName;
}

/// Gets this XML parsing exception's
/// first error string.
std::string XMLParseException::getErrorStr1() const
{
	return this->errorStr1;
}

/// Gets this XML parsing exception's
/// second error string.
std::string XMLParseException::getErrorStr2() const
{
	return this->errorStr2;
}

/// Creates a formatted error message
/// from this XML parsing exception's
/// data.
std::string XMLParseException::getErrorMessage() const
{
	return std::string(this->what());
}

SceneDescriptionException::SceneDescriptionException(const char* message)
	: std::exception(message)
{ }

SceneDescriptionException::SceneDescriptionException(const std::string& message)
	: std::exception(message.c_str())
{ }

/// Creates a new scene description from the
/// XML document at the given path.
SceneDescription::SceneDescription(const std::string& path)
	: path(path)
{
	this->doc.LoadFile(this->path.c_str());
	this->throwError();
}

/// Gets the path of the XML document
/// that describes this scene.
std::string SceneDescription::getPath() const
{
	return this->path;
}

/// Reads all texture assets defined in this
/// scene description document.
std::map<std::string, std::shared_ptr<sf::Texture>> SceneDescription::readTextures() const
{
	std::map<std::string, std::shared_ptr<sf::Texture>> results;
	auto textureNode = this->getTexturesNode();
	if (textureNode == nullptr)
		return results;
	for (auto child = textureNode->FirstChildElement();
		 child != nullptr;
		 child = child->NextSiblingElement())
	{
		std::string name = getAttribute(child, "id");
		std::string path = getAttribute(child, "path");
		auto newPath = this->convertPath(path);
		auto tex = std::make_shared<sf::Texture>();
		if (!tex->loadFromFile(newPath))
		{
			throw SceneDescriptionException("Couldn't load texture file '" + newPath + "'.");
		}
		results[name] = tex;
	}
	return results;
}

/// Reads all renderable elements definitions in this
/// scene description document.
std::map<std::string, si::view::IRenderable_ptr> SceneDescription::readRenderables(
	const std::map<std::string, std::shared_ptr<sf::Texture>>& textures) const
{
	std::map<std::string, si::view::IRenderable_ptr> results;
	auto node = this->getRenderablesNode();
	if (node == nullptr)
	{
		return results;
	}

	for (auto child = node->FirstChildElement();
		 child != nullptr;
		 child = child->NextSiblingElement())
	{
		auto name = getAttribute(child, "id");
		results[name] = readRenderable(child, textures);
	}
	
	return results;
}

/// Reads the scene described by this document.
std::unique_ptr<Scene> SceneDescription::readScene() const
{
	auto textures = this->readTextures();
	auto assets = this->readRenderables(textures);

	auto result = std::make_unique<Scene>();

	auto playerNode = getSingleChild(this->doc.RootElement(), "Player");

	std::shared_ptr<si::model::ShipEntity> player = readShipEntity(playerNode);
	result->addTrackedEntity(player, readAssociatedView(playerNode, assets));

	double playerAccel = getDoubleAttribute(playerNode, "accel");
	result->addController(std::make_shared<si::controller::PlayerController>(player, playerAccel));
	result->addController(std::make_shared<si::controller::OutOfBoundsController>(player, DoubleRect(-0.1, -0.1, 1.2, 1.2)));
	result->registerPlayer(player);

	return result;
}

/// Reads an entity node's associated view.
si::view::IRenderable_ptr SceneDescription::readAssociatedView(
	const tinyxml2::XMLElement* node,
	const std::map<std::string, si::view::IRenderable_ptr>& assets)
{
	std::string assetName = getAttribute(node, "asset");
	if (assets.find(assetName) == assets.end())
	{
		throw SceneDescriptionException("'Sprite' node has an 'asset' attribute value of '" + assetName +
			"', but no asset named '" + assetName + "' was found.");
	}

	return assets.at(assetName);
}

/// Reads a renderable element specified by the given node.
si::view::IRenderable_ptr SceneDescription::readRenderable(
	const tinyxml2::XMLElement* node,
	const std::map<std::string, std::shared_ptr<sf::Texture>>& textures)
{
	std::string nodeName = node->Name();
	if (nodeName == "Sprite")
	{
		std::string texName = getAttribute(node, "texture");
		if (textures.find(texName) == textures.end())
		{
			throw SceneDescriptionException("'Sprite' node has a 'texture' attribute value of '" + texName +
				"', but no texture named '" + texName + "' was found.");
		}

		return std::make_shared<si::view::SpriteRenderable>(textures.at(texName));
	}
	else if (nodeName == "Box")
	{
		double x = getDoubleAttribute(node, "posX");
		double y = getDoubleAttribute(node, "posY");
		double width = getDoubleAttribute(node, "width");
		double height = getDoubleAttribute(node, "height");

		auto contents = readRenderable(getSingleChild(node), textures);

		return std::make_shared<si::view::RelativeBoxRenderable>(contents, DoubleRect(x, y, width, height));
	}
	else
	{
		throw SceneDescriptionException("Unexpected node type: '" + nodeName + "'.");
	}
}

/// Reads a ship entity as specified by the given node.
std::unique_ptr<si::model::ShipEntity> SceneDescription::readShipEntity(
	const tinyxml2::XMLElement* node)
{
	auto physProps = getPhysicsProperties(node);
	Vector2d pos(getDoubleAttribute(node, "posX"), getDoubleAttribute(node, "posY"));
	double maxHealth = getDoubleAttribute(node, "health");

	return std::make_unique<si::model::ShipEntity>(physProps, pos, maxHealth);
}

/// Reads a projectile entity as specified by the given node.
/// The return type of this function is a parameterless function,
/// which can be used to create an arbitrary number of projectiles
/// on-demand.
std::unique_ptr<si::model::ProjectileEntity> SceneDescription::readProjectileEntity(
	const tinyxml2::XMLElement* node)
{
	auto physProps = getPhysicsProperties(node);
	Vector2d pos(getDoubleAttribute(node, "posX"), getDoubleAttribute(node, "posY"));
	Vector2d veloc(getDoubleAttribute(node, "velX"), getDoubleAttribute(node, "velY"));

	return std::make_unique<si::model::ProjectileEntity>(physProps, pos, veloc);
}

/// Reads a model entity specified by the given node.
std::unique_ptr<si::model::Entity> SceneDescription::readEntity(
	const tinyxml2::XMLElement* node)
{
	std::string nodeName = node->Name();
	if (nodeName == "Ship")
	{
		return readShipEntity(node);
	}
	else if (nodeName == "Projectile")
	{
		return readProjectileEntity(node);
	}
	else
	{
		throw SceneDescriptionException("Unexpected node type: '" + nodeName + "'.");
	}
}

/// Converts the given path, which is relative
/// to the scene description's path, to a
/// path that is relative to the program's
/// path.
std::string SceneDescription::convertPath(const std::string& relativePath) const
{
	namespace fs = std::tr2::sys;

	return fs::absolute(relativePath, fs::path(this->path)).string();
}

/// Throws an error if the XML document
/// associated with this scene is in an error
/// state.
void SceneDescription::throwError()
{
	if (doc.Error())
	{
		throw XMLParseException(doc);
	}
}

/// Ensures that the attribute with the given name is present
/// in the given XML node. Otherwise, an exception is thrown.
void SceneDescription::ensureAttributePresent(const tinyxml2::XMLElement* node, const char* name)
{
	if (node->Attribute(name) == nullptr)
	{
		throw SceneDescriptionException("'" + std::string(node->Name()) + "' node did not have a '" + name + "' attribute.");
	}
}

/// Gets the value of the attribute with the given name 
/// in the given XML node. If no such attribute can be 
/// found, an exception is thrown.
std::string SceneDescription::getAttribute(const tinyxml2::XMLElement* node, const char* name)
{
	ensureAttributePresent(node, name);
	return node->Attribute(name);
}

/// Gets the value of the integer attribute with the
/// given name in the given XML node. 
/// If no such attribute can be found, an
/// exception is thrown.
double SceneDescription::getDoubleAttribute(const tinyxml2::XMLElement* node, const char* name)
{
	double result;
	switch (node->QueryDoubleAttribute(name, &result))
	{
	case tinyxml2::XML_NO_ATTRIBUTE:
		throw SceneDescriptionException("'" + std::string(node->Name()) + "' node did not have a '" + name + "' attribute.");;
	case tinyxml2::XML_WRONG_ATTRIBUTE_TYPE:
		throw SceneDescriptionException("'" + std::string(node->Name()) + "' node did have a '" + name + "' attribute, but its value was not formatted as an integer.");
	default:
		return result;
	}
}

/// Reads the given node's physics properties.
si::model::PhysicsProperties SceneDescription::getPhysicsProperties(
	const tinyxml2::XMLElement* node)
{
	double mass = getDoubleAttribute(node, "mass");
	double radius = getDoubleAttribute(node, "radius");

	return{ mass, radius };
}


/// Gets the only child of the given XML node.
/// If this cannot be done, an exception is thrown.
const tinyxml2::XMLElement* SceneDescription::getSingleChild(const tinyxml2::XMLElement* node, const char* name)
{
	auto child = node->FirstChildElement(name);

	if (child == nullptr)
	{
		throw SceneDescriptionException(
			"'" + std::string(node->Name()) + "' should have had exactly one " + 
			(name == nullptr ? "child" : "'" + std::string(name) + "' child") + " node, but had none.");
	}

	if (child->NextSiblingElement(name) != nullptr)
	{
		throw SceneDescriptionException(
			"'" + std::string(node->Name()) + "' should have had exactly one " + 
			(name == nullptr ? "child" : "'" + std::string(name) + "' child") + " node, but has more than one.");
	}

	return child;
}

const tinyxml2::XMLElement* SceneDescription::getTexturesNode() const
{
	return this->doc.RootElement()->FirstChildElement("Textures");
}

const tinyxml2::XMLElement* SceneDescription::getRenderablesNode() const
{
	return this->doc.RootElement()->FirstChildElement("Assets");
}