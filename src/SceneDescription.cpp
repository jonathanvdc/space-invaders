#include "SceneDescription.h"

#include <experimental/filesystem>
#include <exception>
#include <fstream>
#include <map>
#include <memory>
#include <string>
#include <utility>
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
#include "IntervalActionController.h"
#include "IRenderable.h"
#include "Scene.h"
#include "SpriteRenderable.h"
#include "RelativeBoxRenderable.h"
#include "GroupRenderable.h"
#include "RibbonParticleRenderable.h"
#include "FramecounterRenderable.h"
#include "ITimelineEvent.h"
#include "Timeline.h"
#include "SpawnEvent.h"
#include "ShowEvent.h"
#include "DeadlineEvent.h"
#include "InvaderWaveEvent.h"

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
		doc.GetErrorStr1() ? doc.GetErrorStr1() : "", 
		doc.GetErrorStr2() ? doc.GetErrorStr2() : "")
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
const char* const GroupNodeName = "Group";
const char* const ProjectileNodeName = "Projectile";
const char* const ShipNodeName = "Ship";
const char* const AssetsTableNodeName = "Assets";
const char* const TextureTableNodeName = "Textures";
const char* const FontsTableNodeName = "Fonts";
const char* const DecorTableNodeName = "Decor";
const char* const BackgroundTableNodeName = "Background";
const char* const TimelineNodeName = "Timeline";
const char* const SpawnNodeName = "Spawn";
const char* const DeadlineNodeName = "Deadline";
const char* const ConcurrentNodeName = "Concurrent";
const char* const ShowNodeName = "Show";
const char* const WaveNodeName = "Wave";
const char* const RibbonParticleNodeName = "RibbonParticle";
const char* const FramecounterNodeName = "Framecounter";
const char* const TextNodeName = "Text";

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
const char* const FireIntervalAttributeName = "fireInterval";
const char* const NameAttributeName = "name";
const char* const RedAttributeName = "r";
const char* const GreenAttributeName = "g";
const char* const BlueAttributeName = "b";
const char* const AlphaAttributeName = "a";
const char* const IntervalAttributeName = "interval";
const char* const LifetimeAttributeName = "lifetime";
const char* const FontAttributeName = "font";
const char* const TextAttributeName = "text";
const char* const DurationAttributeName = "duration";
const char* const RowsAttributeName = "rows";
const char* const ColumnsAttributeName = "columns";

// Default game bounds. Anything that exceeds these bounds
// will be removed from the game.
const DoubleRect GameBounds{ -0.25, -0.25, 1.5, 1.5 };

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

/// Reads all font assets defined in this
/// scene description document.
std::map<std::string, sf::Font> SceneDescription::readFonts() const
{
	std::map<std::string, sf::Font> results;
	auto fontsNode = getSingleChild(this->doc.RootElement(), FontsTableNodeName, true);
	if (fontsNode == nullptr)
		return results;
	for (auto child = fontsNode->FirstChildElement();
		 child != nullptr;
		 child = child->NextSiblingElement())
	{
		std::string name = getAttribute(child, IdAttributeName);
		std::string path = getAttribute(child, PathAttributeName);
		auto newPath = this->convertPath(path);
		sf::Font fnt;
		if (!fnt.loadFromFile(newPath))
		{
			throw SceneDescriptionException("Couldn't load font file '" + newPath + "'.");
		}
		results[name] = fnt;
	}
	return results;
}

/// Reads all resources defined in this
/// scene description document.
SceneResources SceneDescription::readResources() const
{
	return{ this->readTextures(), this->readFonts() };
}

/// Reads all renderable elements definitions in this
/// scene description document.
std::map<std::string, Factory<si::view::IRenderable_ptr>> SceneDescription::readRenderables(
	const SceneResources& resources) const
{
	std::map<std::string, Factory<si::view::IRenderable_ptr>> results;
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
		results[name] = readRenderable(child, resources);
	}
	
	return results;
}

/// Reads the scene described by this document.
std::unique_ptr<Scene> SceneDescription::readScene() const
{
	auto name = getAttribute(this->doc.RootElement(), NameAttributeName);

	// Start by reading all resources and assets (renderable view elements).
	auto resources = this->readResources();
	auto assets = this->readRenderables(resources);

	auto scene = std::make_unique<Scene>(name);

	// Find and parse the player node, then add it to the 
	// scene.
	auto playerNode = getSingleChild(this->doc.RootElement(), PlayerNodeName);
	addPlayerToScene(playerNode, assets, *scene);

	// Lookup the (optional) background table.
	auto backgroundNode = getSingleChild(this->doc.RootElement(), BackgroundTableNodeName, true);
	if (backgroundNode != nullptr)
	{
		for (auto child = backgroundNode->FirstChildElement();
			 child != nullptr;
			 child = child->NextSiblingElement())
		{
			// Add all background renderables to the scene.
			scene->addRenderable(readAssociatedView(child, assets)());
		}
	}
	
	// Lookup the (optional) decor table.
	auto decorNode = getSingleChild(this->doc.RootElement(), DecorTableNodeName, true);
	if (decorNode != nullptr)
	{
		for (auto child = decorNode->FirstChildElement();
			 child != nullptr;
			 child = child->NextSiblingElement())
		{
			// Add all decor objects to the scene.
			addToScene(readEntity(child, assets)(), *scene);
		}
	}

	// Parse the timeline node, if any.
	auto timelineNode = getSingleChild(this->doc.RootElement(), TimelineNodeName, true);
	if (timelineNode != nullptr)
	{
		// We found a timeline. Awesome!
		// Now let's parse it and add it
		// to the scene.
		auto tLine = std::make_shared<si::timeline::Timeline>(parseTimeline(timelineNode, assets));
		scene->startEvent(tLine);
	}

	return scene;
}

/// Reads an entity node's associated view.
Factory<si::view::IRenderable_ptr> SceneDescription::readAssociatedView(
	const tinyxml2::XMLElement* node,
	const std::map<std::string, Factory<si::view::IRenderable_ptr>>& assets)
{
	return getReferenceAttribute(node, AssetAttributeName, assets);
}

/// Reads a renderable group element specified by the given node.
Factory<si::view::IRenderable_ptr> SceneDescription::readGroupRenderable(
	const tinyxml2::XMLElement* node,
	const SceneResources& resources)
{
	std::vector<Factory<si::view::IRenderable_ptr>> children;

	for (auto child = node->FirstChildElement();
		 child != nullptr;
		 child = child->NextSiblingElement())
	{
		children.push_back(readRenderable(child, resources));
	}

	return [children]()
	{
		std::vector<si::view::IRenderable_ptr> childInsts;
		for (const auto& fact : children)
		{
			childInsts.push_back(fact());
		}

		return std::make_shared<si::view::GroupRenderable>(std::move(childInsts));
	};
}

/// Reads a renderable element specified by the given node.
Factory<si::view::IRenderable_ptr> SceneDescription::readRenderable(
	const tinyxml2::XMLElement* node,
	const SceneResources& resources)
{
	std::string nodeName = node->Name();
	if (nodeName == SpriteNodeName)
	{
		auto tex = getReferenceAttribute(node, TextureAttributeName, resources.textures);
		auto result = std::make_shared<si::view::SpriteRenderable>(tex);

		return [result]() { return result; };
	}
	else if (nodeName == RibbonParticleNodeName)
	{
		auto color = getColorAttribute(node);
		duration_t interval(getDoubleAttribute(node, IntervalAttributeName, 0.01));
		duration_t lifetime(getDoubleAttribute(node, LifetimeAttributeName, 0.5));

		return [=]()
		{
			return std::make_shared<si::view::RibbonParticleRenderable>(color, interval, lifetime);
		};
	}
	else if (nodeName == FramecounterNodeName)
	{
		auto color = getColorAttribute(node);
		auto font = getReferenceAttribute(node, FontAttributeName, resources.fonts);

		return [=]()
		{
			return std::make_shared<si::view::FramecounterRenderable>(font, color);
		};
	}
	else if (nodeName == TextNodeName)
	{
		auto color = getColorAttribute(node);
		auto font = getReferenceAttribute(node, FontAttributeName, resources.fonts);
		auto text = getAttribute(node, TextAttributeName);

		return [=]()
		{
			return std::make_shared<si::view::TextRenderable>(text, font, color);
		};
	}
	else if (nodeName == BoxNodeName)
	{
		double x = getDoubleAttribute(node, PositionXAttributeName, 0.0);
		double y = getDoubleAttribute(node, PositionYAttributeName, 0.0);
		double width = getDoubleAttribute(node, WidthAttributeName, 1.0);
		double height = getDoubleAttribute(node, HeightAttributeName, 1.0);

		auto contents = readRenderable(getSingleChild(node), resources);

		return [=]()
		{
			return std::make_shared<si::view::RelativeBoxRenderable>(contents(), DoubleRect(x, y, width, height));
		};
	}
	else if (nodeName == GroupNodeName)
	{
		return readGroupRenderable(node, resources);
	}
	else
	{
		throw SceneDescriptionException("Unexpected node type: '" + nodeName + "'.");
	}
}

/// Reads a ship entity as specified by the given node.
ParsedShipFactory SceneDescription::readShipEntity(
	const tinyxml2::XMLElement* node,
	const std::map<std::string, Factory<si::view::IRenderable_ptr>>& assets)
{
	auto physProps = getPhysicsProperties(node);
	Vector2d pos(
		getDoubleAttribute(node, PositionXAttributeName, 0.5), 
		getDoubleAttribute(node, PositionYAttributeName, 0.5));
	double maxHealth = getDoubleAttribute(node, HealthAttributeName);
	auto view = readAssociatedView(node, assets);

	return [=]()
	{
		auto model = std::make_shared<si::model::ShipEntity>(physProps, pos, maxHealth);
		std::vector<si::controller::IController_ptr> controllers;
		controllers.push_back(std::make_shared<si::controller::ShipCollisionController>(model));
		controllers.push_back(std::make_shared<si::controller::OutOfBoundsController>(model, GameBounds));
		return ParsedEntity<si::model::ShipEntity>(model, view(), controllers);
	};
}

/// Reads a player entity as specified by the given node.
void SceneDescription::addPlayerToScene(
	const tinyxml2::XMLElement* node,
	const std::map<std::string, Factory<si::view::IRenderable_ptr>>& assets,
	Scene& scene)
{
	// Read the player ship node, and instantiate it.
	auto player = readShipEntity(node, assets)();

	// Add the player to the scene.
	addToScene(player, scene);

	// Register the player, and throw in a player 
	// velocity controller while we're at it.
	double playerAccel = getDoubleAttribute(node, AccelerationAttributeName);
	scene.addController(std::make_shared<si::controller::PlayerController>(player.model, playerAccel));
	scene.registerPlayer(player.model);
	
	// Create a player projectile controller for this ship.
	double fireInterval = getDoubleAttribute(node, FireIntervalAttributeName);
	auto projectileFactory = readProjectileEntity(getSingleChild(node, ProjectileNodeName), assets);
	scene.addController(std::make_shared<si::controller::IntervalActionController>(si::duration_t(fireInterval),
		[](si::model::Game&, si::duration_t)
		{
			return sf::Keyboard::isKeyPressed(sf::Keyboard::Space);
		},
		[player, projectileFactory, &scene](si::model::Game& game, si::duration_t)
		{
			auto bullet = fireProjectile(*player.model, projectileFactory);
			
			addToScene(bullet, scene);
		},
		[=](si::model::Game& game, si::duration_t)
		{
			return game.contains(player.model);
		}));
}

/// Creates a bullet that is fired from the given source. 
/// Momentum is transferred from the source entity to
/// the projectile, but the bullet is not added to the
/// scene.
ParsedEntity<si::model::ProjectileEntity> SceneDescription::fireProjectile(
	si::model::DriftingEntity& source,
	const ParsedProjectileFactory& projectileFactory)
{
	// Create a new projectile.
	auto projectile = projectileFactory();

	auto sourcePhysProps = source.getPhysicsProperties();
	auto projPhysProps = projectile.model->getPhysicsProperties();

	// Compute the projectile's position and velocity,
	// relative to its source.
	auto veloc = vecLength(projectile.model->getVelocity()) * source.getOrientation();
	auto bulletOffset = (sourcePhysProps.radius + projPhysProps.radius) * si::normalizeVec(veloc);
	
	// Set the projectile's position and velocity.
	projectile.model->setPosition(source.getPosition() + bulletOffset);
	projectile.model->setVelocity(source.getVelocity() + veloc);

	// Firing a projectile in space should make ships
	// accelerate in the opposite direction.
	// We want to preserve momentum, though, so we'll
	// multiply that acceleration by the projectile's mass,
	// and divide it by the ship's.
	source.accelerate(-si::Vector2d(veloc * projPhysProps.mass / sourcePhysProps.mass));

	// We're done here. Adding the 
	// projectile to the scene is some
	// other function's problem.
	return projectile;
}

/// Reads a projectile entity as specified by the given node.
/// The return type of this function is a parameterless function,
/// which can be used to create an arbitrary number of projectiles
/// on-demand.
ParsedProjectileFactory SceneDescription::readProjectileEntity(
	const tinyxml2::XMLElement* node,
	const std::map<std::string, Factory<si::view::IRenderable_ptr>>& assets)
{
	auto physProps = getPhysicsProperties(node);
	Vector2d pos(
		getDoubleAttribute(node, PositionXAttributeName, 0.0), 
		getDoubleAttribute(node, PositionYAttributeName, 0.0));
	Vector2d veloc(
		getDoubleAttribute(node, VelocityXAttributeName), 
		getDoubleAttribute(node, VelocityYAttributeName));
	auto view = readAssociatedView(node, assets);

	return [=]()
	{
		auto model = std::make_shared<si::model::ProjectileEntity>(physProps, pos, veloc);
		std::vector<si::controller::IController_ptr> controllers;
		controllers.push_back(std::make_shared<si::controller::ProjectileCollisionController>(model));
		controllers.push_back(std::make_shared<si::controller::OutOfBoundsController>(model, GameBounds));
		return ParsedEntity<si::model::ProjectileEntity>(model, view(), controllers);
	};
}

/// Reads a model entity specified by the given node.
ParsedEntityFactory<si::model::Entity> SceneDescription::readEntity(
	const tinyxml2::XMLElement* node,
	const std::map<std::string, Factory<si::view::IRenderable_ptr>>& assets)
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

/// Reads a timeline as specified by the given node.
si::timeline::Timeline SceneDescription::parseTimeline(
	const tinyxml2::XMLElement* node,
	const std::map<std::string, Factory<si::view::IRenderable_ptr>>& assets)
{
	std::vector<si::timeline::ITimelineEvent_ptr> events;
	for (auto child = node->FirstChildElement();
		 child != nullptr;
		 child = child->NextSiblingElement())
	{
		// Parse all events in the timeline.
		events.push_back(parseTimelineEvent(child, assets));
	}
	return si::timeline::Timeline(std::move(events));
}

/// Reads a concurrent event as specified by the given node.
si::timeline::ConcurrentEvent SceneDescription::parseConcurrentEvent(
	const tinyxml2::XMLElement* node,
	const std::map<std::string, Factory<si::view::IRenderable_ptr>>& assets)
{
	std::vector<si::timeline::ITimelineEvent_ptr> events;
	for (auto child = node->FirstChildElement();
		 child != nullptr;
		 child = child->NextSiblingElement())
	{
		// Parse all events in the timeline.
		events.push_back(parseTimelineEvent(child, assets));
	}
	return si::timeline::ConcurrentEvent(std::move(events));
}

/// Reads an invader wave event as specified by the given node.
si::timeline::ITimelineEvent_ptr SceneDescription::parseWaveEvent(
	const tinyxml2::XMLElement* node,
	const std::map<std::string, Factory<si::view::IRenderable_ptr>>& assets)
{
	auto shipFactory = readShipEntity(getSingleChild(node, ShipNodeName), assets);
	auto projectileFactory = readProjectileEntity(getSingleChild(node, ProjectileNodeName), assets);
	int rows = getIntAttribute(node, RowsAttributeName);
	int cols = getIntAttribute(node, ColumnsAttributeName);

	return std::make_shared<si::timeline::InvaderWaveEvent>(shipFactory, projectileFactory, rows, cols);
}

/// Reads a timeline event as specified by the given node.
si::timeline::ITimelineEvent_ptr SceneDescription::parseTimelineEvent(
	const tinyxml2::XMLElement* node,
	const std::map<std::string, Factory<si::view::IRenderable_ptr>>& assets)
{
	std::string nodeName = node->Name();
	if (nodeName == TimelineNodeName)
	{
		return std::make_shared<si::timeline::Timeline>(parseTimeline(node, assets));
	}
	else if (nodeName == SpawnNodeName)
	{
		auto factory = readEntity(getSingleChild(node), assets);
		return std::make_shared<si::timeline::SpawnEvent>(factory);
	}
	else if (nodeName == DeadlineNodeName)
	{
		duration_t duration(getDoubleAttribute(node, DurationAttributeName));
		auto inner = parseTimelineEvent(getSingleChild(node), assets);
		return std::make_shared<si::timeline::DeadlineEvent>(inner, duration);
	}
	else if (nodeName == ConcurrentNodeName)
	{
		return std::make_shared<si::timeline::ConcurrentEvent>(parseConcurrentEvent(node, assets));
	}
	else if (nodeName == ShowNodeName)
	{
		auto factory = getReferenceAttribute(node, AssetAttributeName, assets);
		return std::make_shared<si::timeline::ShowEvent>(factory);
	}
	else if (nodeName == WaveNodeName)
	{
		return parseWaveEvent(node, assets);
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

	return fs::absolute(relativePath, fs::path(this->path).parent_path()).string();
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
int SceneDescription::getIntAttribute(const tinyxml2::XMLElement* node, const char* name)
{
	int result;
	switch (node->QueryIntAttribute(name, &result))
	{
	case tinyxml2::XML_NO_ATTRIBUTE:
		throw SceneDescriptionException(
			"'" + std::string(node->Name()) +
			"' node did not have a '" + name + "' attribute.");
	case tinyxml2::XML_WRONG_ATTRIBUTE_TYPE:
		throw SceneDescriptionException(
			"'" + std::string(node->Name()) +
			"' node did have a '" + name +
			"' attribute, but its value was not formatted as an integer number.");
	default:
		return result;
	}
}

/// Gets the value of the floating-point attribute with the
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
			"' node did not have a '" + name + "' attribute.");
	case tinyxml2::XML_WRONG_ATTRIBUTE_TYPE:
		throw SceneDescriptionException(
			"'" + std::string(node->Name()) + 
			"' node did have a '" + name + 
			"' attribute, but its value was not formatted as a floating-point number.");
	default:
		return result;
	}
}

/// Gets the value of the floating-point attribute with the
/// given name in the given XML node. 
/// If no such attribute can be found, the given default
/// value is returned as a result.
double SceneDescription::getDoubleAttribute(const tinyxml2::XMLElement* node, const char* name, double defaultValue)
{
	double result;
	switch (node->QueryDoubleAttribute(name, &result))
	{
	case tinyxml2::XML_NO_ATTRIBUTE:
		return defaultValue;
	case tinyxml2::XML_WRONG_ATTRIBUTE_TYPE:
		throw SceneDescriptionException(
			"'" + std::string(node->Name()) +
			"' node did have a '" + name + 
			"' attribute, but its value was not formatted as a floating-point number.");
	default:
		return result;
	}
}

/// Gets the given node's color, which is defined by
/// its 'r', 'g', 'b' and 'a' attributes.
sf::Color SceneDescription::getColorAttribute(const tinyxml2::XMLElement* node)
{
	auto r = sf::Uint8(getDoubleAttribute(node, RedAttributeName, 0.0) * 255);
	auto g = sf::Uint8(getDoubleAttribute(node, GreenAttributeName, 0.0) * 255);
	auto b = sf::Uint8(getDoubleAttribute(node, BlueAttributeName, 0.0) * 255);
	auto a = sf::Uint8(getDoubleAttribute(node, AlphaAttributeName, 1.0) * 255);

	return sf::Color(r, g, b, a);
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
const tinyxml2::XMLElement* SceneDescription::getSingleChild(const tinyxml2::XMLElement* node, const char* name, bool isOptional)
{
	auto child = node->FirstChildElement(name);

	if (child == nullptr)
	{
		if (isOptional)
		{
			return nullptr;
		}
		else
		{
			throw SceneDescriptionException(
				"'" + std::string(node->Name()) + "' should have had exactly one " +
				(name == nullptr ? "child" : "'" + std::string(name) + "' child") + " node, but had none.");
		}
	}

	if (child->NextSiblingElement(name) != nullptr)
	{
		throw SceneDescriptionException(
			"'" + std::string(node->Name()) + "' should have had " + (isOptional ? std::string("at most one ") : "exactly one ") +
			(name == nullptr ? "child" : "'" + std::string(name) + "' child") + " node, but has more than one.");
	}

	return child;
}

const tinyxml2::XMLElement* SceneDescription::getTexturesNode() const
{
	return getSingleChild(this->doc.RootElement(), TextureTableNodeName, true);
}

const tinyxml2::XMLElement* SceneDescription::getRenderablesNode() const
{
	return getSingleChild(this->doc.RootElement(), AssetsTableNodeName, true);
}

/// Parses the scene description XML document at the
/// given path, and returns a unique pointer
/// to the scene it describes. An exception is
/// thrown is something goes wrong.
std::unique_ptr<Scene> si::parser::parseScene(const std::string& path)
{
	SceneDescription description(path);
	return description.readScene();
}