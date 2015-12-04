#pragma once

#include <exception>
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

namespace si
{
	/// An exception class for XML parsing exceptions.
	class XMLParseException : public std::exception
	{
	public:
		/// Creates an XML parsing exception from the given
		/// data.
		XMLParseException(
			const std::string& errorName,
			const std::string& errorStr1, const std::string& errorStr2);

		/// Creates an XML parsing exception from the error in the
		/// given XML document.
		XMLParseException(const tinyxml2::XMLDocument& doc);
		
		/// Gets this XML parsing exception's
		/// error name.
		std::string getErrorName() const;

		/// Gets this XML parsing exception's
		/// first error string.
		std::string getErrorStr1() const;

		/// Gets this XML parsing exception's
		/// second error string.
		std::string getErrorStr2() const;

		/// Creates a formatted error message
		/// from this XML parsing exception's
		/// data.
		std::string getErrorMessage() const;

	private:
		std::string errorName;
		std::string errorStr1;
		std::string errorStr2;
	};

	/// Defines a scene description class.
	class SceneDescription final
	{
	public:
		/// Creates a new scene description from the
		/// XML document at the given path.
		SceneDescription(const std::string& path);

		/// Gets the path of the XML document
		/// that describes this scene.
		std::string getPath() const;

		/// Reads all texture assets defined in this
		/// scene description document.
		std::map<std::string, std::shared_ptr<sf::Texture>> readTextures() const;
		
	private:
		/// Gets this scene description's texture definition node.
		const tinyxml2::XMLElement* getTexturesNode() const;

		/// Converts the given path, which is relative
		/// to the scene description's path, to a
		/// path that is relative to the program's
		/// path.
		std::string convertPath(const std::string& path) const;

		/// Throws an error if the XML document
		/// associated with this scene is in an error
		/// state.
		void throwError();

		tinyxml2::XMLDocument doc;
		std::string path;
	};
}
