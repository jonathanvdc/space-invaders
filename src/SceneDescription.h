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
#include "ITimelineEvent.h"
#include "Timeline.h"

namespace si
{
	namespace parser
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

		/// Defines a struct that holds data for
		/// entities that have been parsed.
		template<typename T>
		struct ParsedEntity
		{
			/// Creates a parsed entity from the given components.
			ParsedEntity(
				std::shared_ptr<T> model,
				si::view::IRenderable_ptr view,
				const std::vector<si::controller::IController_ptr>& controllers)
				: model(model), view(view), controllers(controllers)
			{ }

			/// Creates a parsed entity from the given components,
			/// with move semantics.
			ParsedEntity(
					std::shared_ptr<T> model,
					si::view::IRenderable_ptr view,
					std::vector<si::controller::IController_ptr>&& controllers)
				: model(model), view(view), controllers(controllers)
			{}

			/// Creates a parsed entity from the given components.
			template<typename V>
			ParsedEntity(
				const ParsedEntity<V>& other)
				: ParsedEntity(other.model, other.view, other.controllers)
			{ }

			/// Creates a parsed entity from the given components,
			/// with move semantics.
			template<typename V>
			ParsedEntity(
				ParsedEntity<V>&& other)
				: ParsedEntity(other.model, other.view, other.controllers)
			{ }

			/// The parsed entity's model.
			std::shared_ptr<T> model;
			/// The parsed entity's view.
			si::view::IRenderable_ptr view;
			/// The parsed entity's controllers.
			std::vector<si::controller::IController_ptr> controllers;
		};

		template<typename T>
		using ParsedEntityFactory = std::function<ParsedEntity<T>()>;

		using ParsedShipFactory = ParsedEntityFactory<si::model::ShipEntity>;
		using ParsedProjectileFactory = ParsedEntityFactory<si::model::ProjectileEntity>;

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

			/// Reads all renderable elements definitions in this
			/// scene description document.
			std::map<std::string, si::view::IRenderable_ptr> readRenderables(
				const std::map<std::string, std::shared_ptr<sf::Texture>>& textures) const;

			/// Reads the scene described by this document.
			std::unique_ptr<Scene> readScene() const;

			/// Reads a renderable element specified by the given node.
			static si::view::IRenderable_ptr readRenderable(
				const tinyxml2::XMLElement* node,
				const std::map<std::string, std::shared_ptr<sf::Texture>>& textures);

			/// Reads an entity node's associated view.
			static si::view::IRenderable_ptr readAssociatedView(
				const tinyxml2::XMLElement* node,
				const std::map<std::string, si::view::IRenderable_ptr>& assets);

			/// Reads a generic ship entity as specified by the given node.
			static ParsedShipFactory readShipEntity(
				const tinyxml2::XMLElement* node,
				const std::map<std::string, si::view::IRenderable_ptr>& assets);

			/// Reads a projectile entity as specified by the given node.
			static ParsedProjectileFactory readProjectileEntity(
				const tinyxml2::XMLElement* node,
				const std::map<std::string, si::view::IRenderable_ptr>& assets);

			/// Reads an unknown entity as specified by the given node.
			static ParsedEntityFactory<si::model::Entity> readEntity(
				const tinyxml2::XMLElement* node,
				const std::map<std::string, si::view::IRenderable_ptr>& assets);

			/// Reads a timeline as specified by the given node.
			static si::timeline::Timeline parseTimeline(
				const tinyxml2::XMLElement* node,
				const std::map<std::string, si::view::IRenderable_ptr>& assets);
			
			/// Reads a timeline event as specified by the given node.
			static si::timeline::ITimelineEvent_ptr parseTimelineEvent(
				const tinyxml2::XMLElement* node,
				const std::map<std::string, si::view::IRenderable_ptr>& assets);

			/// Adds the given entity's model, view and
			/// controllers to the given scene.
			static void addToScene(
				const ParsedEntity<si::model::Entity>& entity,
				Scene& target);

			/// Adds a player entity, as specified by the given node,
			/// to the scene.
			static void addPlayerToScene(
				const tinyxml2::XMLElement* node,
				const std::map<std::string, si::view::IRenderable_ptr>& assets,
				Scene& scene);

			/// Creates a bullet that is fired from the given source. 
			/// Momentum is transferred from the source entity to
			/// the projectile, but the bullet is not added to the
			/// scene.
			static ParsedEntity<si::model::ProjectileEntity> fireProjectile(
				si::model::DriftingEntity& source,
				const ParsedProjectileFactory& projectileFactory);

		private:
			/// Gets this scene description's texture definitions node.
			const tinyxml2::XMLElement* getTexturesNode() const;

			/// Gets this scene description's renderable definitions node.
			const tinyxml2::XMLElement* getRenderablesNode() const;

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

			/// Gets the value of the floating-point attribute with the
			/// given name in the given XML node. 
			/// If no such attribute can be found, an
			/// exception is thrown.
			static double getDoubleAttribute(const tinyxml2::XMLElement* node, const char* name);

			/// Gets the value of the floating-point attribute with the
			/// given name in the given XML node. 
			/// If no such attribute can be found, the given default
			/// value is returned as a result.
			static double getDoubleAttribute(const tinyxml2::XMLElement* node, const char* name, double defaultValue);

			/// Reads the given node's physics properties.
			static si::model::PhysicsProperties getPhysicsProperties(
				const tinyxml2::XMLElement* node);

			/// Gets the only child of the given XML node, optionally
			/// with the given name.
			/// If this cannot be done, an exception is thrown.
			static const tinyxml2::XMLElement* getSingleChild(const tinyxml2::XMLElement* parent,
				const char* name = nullptr, bool isOptional = false);

			/// Gets a value from the given key-value map
			/// identified by the attribute with the given name
			/// in the given node.
			/// If something goes wrong, an exception is thrown.
			template<typename T>
			static T getReferenceAttribute(
				const tinyxml2::XMLElement* node,
				const char* attributeName,
				const std::map<std::string, T>& map)
			{
				std::string attr = getAttribute(node, attributeName);
				if (map.find(attr) == map.end())
				{
					throw SceneDescriptionException(
						"This '" + std::string(node->Name()) + "' node's '" + std::string(attributeName) +
						"' attribute has a value of '" + attr +
						"', but no appropriate element named '" + attr + "' was found.");
				}

				return map.at(attr);
			}

			tinyxml2::XMLDocument doc;
			std::string path;
		};

		/// Parses the scene description XML document at the
		/// given path, and returns a unique pointer
		/// to the scene it describes. An exception is
		/// thrown is something goes wrong.
		std::unique_ptr<Scene> parseScene(const std::string& path);
	}
}
