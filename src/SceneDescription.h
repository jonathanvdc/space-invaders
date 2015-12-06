#pragma once

#include <exception>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <SFML/Graphics.hpp>
#include "tinyxml2/tinyxml2.h"
#include "Common.h"
#include "Entity.h"
#include "PhysicsEntity.h"
#include "ShipEntity.h"
#include "ProjectileEntity.h"
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

	/// An exception class for semantic errors in scene
	/// description documents.
	class SceneDescriptionException : public std::exception
	{
	public:
		SceneDescriptionException(const std::string& message);
		SceneDescriptionException(const char* message);
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

		/// Reads a renderable element specified by the given node.
		static si::view::IRenderable_ptr readRenderable(
			const tinyxml2::XMLElement* node, 
			const std::map<std::string, std::shared_ptr<sf::Texture>>& textures);

		/// Reads a ship entity as specified by the given node.
		static std::shared_ptr<si::model::ShipEntity> readShipEntity(
			const tinyxml2::XMLElement* node);

		/// Reads a projectile entity as specified by the given node.
		/// The return type of this function is a parameterless function,
		/// which can be used to create an arbitrary number of projectiles
		/// on-demand.
		static std::function<std::shared_ptr<si::model::ProjectileEntity>()> readProjectileEntity(
			const tinyxml2::XMLElement* node);

		/// Reads a model entity specified by the given node.
		static si::model::Entity_ptr readEntity(
			const tinyxml2::XMLElement* node);
		
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

		/// Ensures that the attribute with the given name is present
		/// in the given XML node. Otherwise, an exception is thrown.
		static void ensureAttributePresent(const tinyxml2::XMLElement* node, const char* name);

		/// Gets the value of the attribute with the given name 
		/// in the given XML node. If no such attribute can be 
		/// found, an exception is thrown.
		static std::string getAttribute(const tinyxml2::XMLElement* node, const char* name);

		/// Gets the value of the integer attribute with the
		/// given name in the given XML node. 
		/// If no such attribute can be found, an
		/// exception is thrown.
		static double getDoubleAttribute(const tinyxml2::XMLElement* node, const char* name);

		/// Reads the given node's physics properties.
		static si::model::PhysicsProperties getPhysicsProperties(
			const tinyxml2::XMLElement* node);

		/// Gets the only child of the given XML node.
		/// If this cannot be done, an exception is thrown.
		static const tinyxml2::XMLElement* getSingleChild(const tinyxml2::XMLElement* parent);

		tinyxml2::XMLDocument doc;
		std::string path;
	};
}
