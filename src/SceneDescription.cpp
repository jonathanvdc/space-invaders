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
#include "ProjectileCollisionController.h"
#include "OutOfBoundsController.h"
#include "IRenderable.h"
#include "Scene.h"
#include "SpriteRenderable.h"
#include "RelativeBoxRenderable.h"

using namespace si;
using namespace si::parser;

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

// Constants that define XML node names.
const char* const PlayerNodeName = "Player";
const char* const SpriteNodeName = "Sprite";
const char* const BoxNodeName = "Box";
const char* const ProjectileNodeName = "Projectile";
const char* const ShipNodeName = "Ship";
const char* const AssetsTableNodeName = "Assets";
const char* const TextureTableNodeName = "Textures";

// Constants that define XML attribute names.
const char* const IdAttributeName = "id";
const char* const PathAttributeName = "path";
const char* const PositionXAttributeName = "posX";
const char* const PositionYAttributeName = "posY";
const char* const WidthAttributeName = "width";
const char* const HeightAttributeName = "height";
const char* const VelocityXAttributeName = "velX";
const char* const VelocityYAttributeName = "velY";
const char* const AccelerationAttributeName = "accel";
const char* const MassAttributeName = "mass";
const char* const RadiusAttributeName = "radius";
const char* const HealthAttributeName = "health";
const char* const TextureAttributeName = "texture";
const char* const AssetAttributeName = "asset";

// Default game bounds. Anything that exceeds these bounds
// will be removed from the game.
const DoubleRect GameBounds{ -0.1, -0.1, 1.2, 1.2 };

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
		std::string name = getAttribute(child, IdAttributeName);
		std::string path = getAttribute(child, PathAttributeName);
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
		auto name = getAttribute(child, IdAttributeName);
		results[name] = readRenderable(child, textures);
	}
	
	return results;
}

/// Reads the scene described by this document.
std::unique_ptr<Scene> SceneDescription::readScene() const
{
	// Start by reading all textures and assets (renderable view elements).
	auto textures = this->readTextures();
	auto assets = this->readRenderables(textures);

	auto scene = std::make_unique<Scene>();

	// Find and parse the player node.
	auto playerNode = getSingleChild(this->doc.RootElement(), PlayerNodeName);
	auto player = readShipEntity(playerNode, assets)();
	// Add the player to the scene.
	addToScene(player, *scene);

	// Register the player, and throw in a player controller while we're at it.
	double playerAccel = getDoubleAttribute(playerNode, AccelerationAttributeName);
	scene->addController(std::make_shared<si::controller::PlayerController>(player.model, playerAccel));
	scene->registerPlayer(player.model);

	return scene;
}

/// Reads an entity node's associated view.
si::view::IRenderable_ptr SceneDescription::readAssociatedView(
	const tinyxml2::XMLElement* node,
	const std::map<std::string, si::view::IRenderable_ptr>& assets)
{
	return getReferenceAttribute(node, AssetAttributeName, assets);
}

/// Reads a renderable element specified by the given node.
si::view::IRenderable_ptr SceneDescription::readRenderable(
	const tinyxml2::XMLElement* node,
	const std::map<std::string, std::shared_ptr<sf::Texture>>& textures)
{
	std::string nodeName = node->Name();
	if (nodeName == SpriteNodeName)
	{
		auto tex = getReferenceAttribute(node, TextureAttributeName, textures);
		return std::make_shared<si::view::SpriteRenderable>(tex);
	}
	else if (nodeName == BoxNodeName)
	{
		double x = getDoubleAttribute(node, PositionXAttributeName);
		double y = getDoubleAttribute(node, PositionYAttributeName);
		double width = getDoubleAttribute(node, WidthAttributeName);
		double height = getDoubleAttribute(node, HeightAttributeName);

		auto contents = readRenderable(getSingleChild(node), textures);

		return std::make_shared<si::view::RelativeBoxRenderable>(contents, DoubleRect(x, y, width, height));
	}
	else
	{
		throw SceneDescriptionException("Unexpected node type: '" + nodeName + "'.");
	}
}

/// Reads a ship entity as specified by the given node.
ParsedShipFactory SceneDescription::readShipEntity(
	const tinyxml2::XMLElement* node,
	const std::map<std::string, si::view::IRenderable_ptr>& assets)
{
	auto physProps = getPhysicsProperties(node);
	Vector2d pos(getDoubleAttribute(node, PositionXAttributeName), getDoubleAttribute(node, PositionYAttributeName));
	double maxHealth = getDoubleAttribute(node, HealthAttributeName);

	return [=]()
	{
		auto model = std::make_shared<si::model::ShipEntity>(physProps, pos, maxHealth);
		auto view = readAssociatedView(node, assets);
		std::vector<si::controller::IController_ptr> controllers;
		controllers.push_back(std::make_shared<si::controller::ShipCollisionController>(model));
		controllers.push_back(std::make_shared<si::controller::OutOfBoundsController>(model, GameBounds));
		return ParsedEntity<si::model::ShipEntity>(model, view, controllers);
	};
}

/// Reads a projectile entity as specified by the given node.
/// The return type of this function is a parameterless function,
/// which can be used to create an arbitrary number of projectiles
/// on-demand.
ParsedProjectileFactory SceneDescription::readProjectileEntity(
	const tinyxml2::XMLElement* node,
	const std::map<std::string, si::view::IRenderable_ptr>& assets)
{
	auto physProps = getPhysicsProperties(node);
	Vector2d pos(getDoubleAttribute(node, PositionXAttributeName), getDoubleAttribute(node, PositionYAttributeName));
	Vector2d veloc(getDoubleAttribute(node, VelocityXAttributeName), getDoubleAttribute(node, VelocityYAttributeName));

	return [=]()
	{
		auto model = std::make_shared<si::model::ProjectileEntity>(physProps, pos, veloc);
		auto view = readAssociatedView(node, assets);
		std::vector<si::controller::IController_ptr> controllers;
		controllers.push_back(std::make_shared<si::controller::ProjectileCollisionController>(model));
		controllers.push_back(std::make_shared<si::controller::OutOfBoundsController>(model, GameBounds));
		return ParsedEntity<si::model::ProjectileEntity>(model, view, controllers);
	};
}

/// Reads a model entity specified by the given node.
ParsedEntityFactory<si::model::Entity> SceneDescription::readEntity(
	const tinyxml2::XMLElement* node,
	const std::map<std::string, si::view::IRenderable_ptr>& assets)
{
	std::string nodeName = node->Name();
	if (nodeName == ShipNodeName)
	{
		return readShipEntity(node, assets);
	}
	else if (nodeName == ProjectileNodeName)
	{
		return readProjectileEntity(node, assets);
	}
	else
	{
		throw SceneDescriptionException("Unexpected node type: '" + nodeName + "'.");
	}
}

/// Adds the given entity's model, view and
/// controllers to the given scene.
void SceneDescription::addToScene(
	const ParsedEntity<si::model::Entity>& entity,
	Scene& target)
{
	target.addTrackedEntity(entity.model, entity.view);
	for (const auto& item : entity.controllers)
	{
		target.addController(item);
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
		throw SceneDescriptionException(
			"'" + std::string(node->Name()) + 
			"' node did not have a '" + name + "' attribute.");;
	case tinyxml2::XML_WRONG_ATTRIBUTE_TYPE:
		throw SceneDescriptionException(
			"'" + std::string(node->Name()) + 
			"' node did have a '" + name + "' attribute, but its value was not formatted as an integer.");
	default:
		return result;
	}
}

/// Reads the given node's physics properties.
si::model::PhysicsProperties SceneDescription::getPhysicsProperties(
	const tinyxml2::XMLElement* node)
{
	double mass = getDoubleAttribute(node, MassAttributeName);
	double radius = getDoubleAttribute(node, RadiusAttributeName);

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
	return this->doc.RootElement()->FirstChildElement(TextureTableNodeName);
}

const tinyxml2::XMLElement* SceneDescription::getRenderablesNode() const
{
	return this->doc.RootElement()->FirstChildElement(AssetsTableNodeName);
}