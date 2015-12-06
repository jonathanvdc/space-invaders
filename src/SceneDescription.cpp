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
#include "IController.h"
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

const tinyxml2::XMLElement* SceneDescription::getTexturesNode() const
{
	return this->doc.FirstChildElement("Textures");
}

/// Reads a renderable element specified by the given node.
si::view::IRenderable_ptr SceneDescription::readRenderable(
	const tinyxml2::XMLElement* node,
	const std::map<std::string, std::shared_ptr<sf::Texture>>& textures)
{
	std::string nodeName = node->Name();
	if (nodeName == "Sprite")
	{
		std::string textureName = getAttribute(node, "id");

		std::string texName = textureName;
		if (textures.find(texName) == textures.end())
		{
			throw SceneDescriptionException("'Sprite' node had an 'id' attribute value of '" + texName + 
				                            "', but no matching texture was found.");
		}

		return std::make_shared<si::view::SpriteRenderable>(textures.at(texName));
	}
	else if (nodeName == "Box")
	{
		double x = getDoubleAttribute(node, "x");
		double y = getDoubleAttribute(node, "y");
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

/// Gets the only child of the given XML node.
/// If this cannot be done, an exception is thrown.
const tinyxml2::XMLElement* SceneDescription::getSingleChild(const tinyxml2::XMLElement* node)
{
	auto child = node->FirstChildElement();

	if (child == nullptr)
	{
		throw SceneDescriptionException("'" + std::string(node->Name()) + "' should have had exactly one child node, but had none.");
	}

	if (child->NextSiblingElement() != nullptr)
	{
		throw SceneDescriptionException("'" + std::string(node->Name()) + "' should have had exactly one child node, but more than one.");
	}

	return child;
}
