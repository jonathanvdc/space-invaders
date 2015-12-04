#include "SceneDescription.h"

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
	for (auto child = textureNode->FirstChildElement(); 
		 child != nullptr; 
		 child = child->NextSiblingElement())
	{
		std::string name = child->Attribute("id");
		std::string path = child->Attribute("path");
		auto newPath = this->convertPath(path);
		auto tex = std::make_shared<sf::Texture>();
		if (!tex->loadFromFile(newPath))
		{
			throw std::runtime_error("Couldn't load file '" + newPath + "'.");
		}
		results[name] = tex;
	}
	return results;
}

void SceneDescription::throwError()
{
	if (doc.Error())
	{
		throw XMLParseException(doc);
	}
}
