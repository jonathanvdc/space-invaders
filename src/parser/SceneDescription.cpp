#include "SceneDescription.h"

#include <exception>
#include <fstream>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include "tinyxml2/tinyxml2.h"
#include "Common.h"
#include "model/Entity.h"
#include "model/PhysicsEntity.h"
#include "model/ShipEntity.h"
#include "model/DriftingEntity.h"
#include "controller/IController.h"
#include "controller/ActionController.h"
#include "controller/IntervalActionController.h"
#include "controller/PlayerController.h"
#include "controller/OutOfBoundsController.h"
#include "controller/ShipCollisionController.h"
#include "controller/ProjectileCollisionController.h"
#include "controller/ObstacleCollisionController.h"
#include "controller/GravityController.h"
#include "view/IRenderable.h"
#include "view/AnimatedSpriteRenderable.h"
#include "view/SpriteRenderable.h"
#include "view/RelativeBoxRenderable.h"
#include "view/GroupRenderable.h"
#include "view/RibbonParticleRenderable.h"
#include "view/ParticleEmitterRenderable.h"
#include "view/FramecounterRenderable.h"
#include "timeline/ITimelineEvent.h"
#include "timeline/Timeline.h"
#include "timeline/ConcurrentEvent.h"
#include "timeline/SpawnEvent.h"
#include "timeline/ShowEvent.h"
#include "timeline/DeadlineEvent.h"
#include "timeline/InvaderWaveEvent.h"
#include "timeline/ConditionalEvent.h"
#include "timeline/PermanentEvent.h"
#include "timeline/LoopedEvent.h"
#include "timeline/BackgroundEvent.h"
#include "timeline/SoundEvent.h"
#include "timeline/MusicEvent.h"
#include "Scene.h"
#include "ParsedEntity.h"

using namespace si;
using namespace si::parser;

/// Creates an XML parsing exception from the given
/// data.
XMLParseException::XMLParseException(
	const std::string& errorName,
	const std::string& errorStr1, const std::string& errorStr2)
	: std::runtime_error(createErrorMessage(errorName, errorStr1, errorStr2)),
	  errorName(errorName), errorStr1(errorStr1), errorStr2(errorStr2)
{ }

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

std::string XMLParseException::createErrorMessage(
	const std::string& errorName,
	const std::string& errorStr1, const std::string& errorStr2)
{
	std::string msg = errorName;
	if (errorStr1.size() > 0)
	{
		msg += " - " + errorStr1;
	}
	if (errorStr2.size() > 0)
	{
		msg += " - " + errorStr2;
	}
	return msg;
}

SceneDescriptionException::SceneDescriptionException(const char* message)
	: std::runtime_error(message)
{ }

SceneDescriptionException::SceneDescriptionException(const std::string& message)
	: std::runtime_error(message)
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
const char* const AnimatedSpriteNodeName = "AnimatedSprite";
const char* const PlayerNodeName = "Player";
const char* const SpriteNodeName = "Sprite";
const char* const BoxNodeName = "Box";
const char* const GroupNodeName = "Group";
const char* const ProjectileNodeName = "Projectile";
const char* const ShipNodeName = "Ship";
const char* const ObstacleNodeName = "Obstacle";
const char* const AssetsTableNodeName = "Assets";
const char* const TextureTableNodeName = "Textures";
const char* const SoundsTableNodeName = "Sounds";
const char* const MusicTableNodeName = "Music";
const char* const FontsTableNodeName = "Fonts";
const char* const DecorTableNodeName = "Decor";
const char* const BackgroundTableNodeName = "Background";
const char* const TimelineNodeName = "Timeline";
const char* const SpawnNodeName = "Spawn";
const char* const DeadlineNodeName = "Deadline";
const char* const PermanentNodeName = "Permanent";
const char* const ConcurrentNodeName = "Concurrent";
const char* const ControllersNodeName = "Controllers";
const char* const ShowNodeName = "Show";
const char* const WaveNodeName = "Wave";
const char* const ConditionNodeName = "Condition";
const char* const ThenNodeName = "Then";
const char* const ElseNodeName = "Else";
const char* const RibbonParticleNodeName = "RibbonParticle";
const char* const ParticleEmitterNodeName = "ParticleEmitter";
const char* const FramecounterNodeName = "Framecounter";
const char* const TextNodeName = "Text";
const char* const GravityNodeName = "Gravity";
const char* const LoopNodeName = "Loop";
const char* const BackgroundNodeName = "Background";
const char* const MainNodeName = "Main";
const char* const ExtraNodeName = "Extra";
const char* const SoundNodeName = "Sound";
const char* const MusicNodeName = "Music";
const char* const SetFlagNodeName = "SetFlag";
const char* const OnLeaveNodeName = "OnLeave";
const char* const OnEnterNodeName = "OnEnter";

// Constants that define XML attribute names.
const char* const IdAttributeName = "id";
const char* const PathAttributeName = "path";
const char* const PositionXAttributeName = "posX";
const char* const PositionYAttributeName = "posY";
const char* const WidthAttributeName = "width";
const char* const HeightAttributeName = "height";
const char* const VelocityXAttributeName = "velX";
const char* const VelocityYAttributeName = "velY";
const char* const SpringConstantAttributeName = "springConstant";
const char* const AccelerationAttributeName = "accel";
const char* const MassAttributeName = "mass";
const char* const RadiusAttributeName = "radius";
const char* const HealthAttributeName = "health";
const char* const TextureAttributeName = "texture";
const char* const AssetAttributeName = "asset";
const char* const FireIntervalAttributeName = "fireInterval";
const char* const FireIntervalDeviationAttributeName = "fireIntervalDeviation";
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
const char* const PredicateAttributeName = "predicate";
const char* const SpeedAttributeName = "speed";
const char* const GravitationalConstantAttributeName = "G";
const char* const MusicAttributeName = "music";
const char* const SoundAttributeName = "sound";
const char* const FlagAttributeName = "flag";
const char* const ValueAttributeName = "value";
const char* const MaxIterationCountAttributeName = "maxIterations";
const char* const FrameCountAttributeName = "frameCount";
const char* const CycleDurationAttributeName = "cycleDuration";

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
		auto tex = std::make_shared<sf::Texture>();
		if (!tex->loadFromFile(path))
		{
			throw SceneDescriptionException("Couldn't load texture file '" + path + "'.");
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
		sf::Font fnt;
		if (!fnt.loadFromFile(path))
		{
			throw SceneDescriptionException("Couldn't load font file '" + path + "'.");
		}
		results[name] = fnt;
	}
	return results;
}

/// Reads all sound assets defined in this
/// scene description document.
std::map<std::string, std::shared_ptr<sf::SoundBuffer>> SceneDescription::readSounds() const
{
	std::map<std::string, std::shared_ptr<sf::SoundBuffer>> results;
	auto soundsNode = getSingleChild(this->doc.RootElement(), SoundsTableNodeName, true);
	if (soundsNode == nullptr)
		return results;
	for (auto child = soundsNode->FirstChildElement();
		 child != nullptr;
		 child = child->NextSiblingElement())
	{
		std::string name = getAttribute(child, IdAttributeName);
		std::string path = getAttribute(child, PathAttributeName);
		auto sound = std::make_shared<sf::SoundBuffer>();
		if (!sound->loadFromFile(path))
		{
			throw SceneDescriptionException("Couldn't load audio file '" + path + "'.");
		}
		results[name] = sound;
	}
	return results;
}

/// Reads all music assets defined in this scene
/// description document.
std::map<std::string, std::shared_ptr<sf::Music>> SceneDescription::readMusic() const
{
	std::map<std::string, std::shared_ptr<sf::Music>> results;
	auto soundsNode = getSingleChild(this->doc.RootElement(), MusicTableNodeName, true);
	if (soundsNode == nullptr)
		return results;
	for (auto child = soundsNode->FirstChildElement();
		 child != nullptr;
		 child = child->NextSiblingElement())
	{
		std::string name = getAttribute(child, IdAttributeName);
		std::string path = getAttribute(child, PathAttributeName);
		auto sound = std::make_shared<sf::Music>();
		if (!sound->openFromFile(path))
		{
			throw SceneDescriptionException("Couldn't load audio file '" + path + "'.");
		}
		results[name] = sound;
	}
	return results;
}

/// Reads all resources defined in this
/// scene description document.
SceneResources SceneDescription::readResources() const
{
	return{ this->readTextures(), this->readFonts(),
			this->readSounds(), this->readMusic() };
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
	SceneAssets assets = { this->readRenderables(resources), resources.sounds, resources.music };

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
		auto tLine = parseTimeline(timelineNode, assets)();
		scene->startEvent(tLine);
	}

	return scene;
}

/// Reads an entity node's associated view.
Factory<si::view::IRenderable_ptr> SceneDescription::readAssociatedView(
	const tinyxml2::XMLElement* node,
	const SceneAssets& assets)
{
	return getReferenceAttribute(node, AssetAttributeName, assets.renderables);
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
	else if (nodeName == AnimatedSpriteNodeName)
	{
		auto tex = getReferenceAttribute(node, TextureAttributeName, resources.textures);
		int frameCount = getIntAttribute(node, FrameCountAttributeName);
		duration_t cycleDuration(getDoubleAttribute(node, CycleDurationAttributeName, 0.2));
		auto result = std::make_shared<si::view::AnimatedSpriteRenderable>(
			tex, frameCount, cycleDuration);

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
	else if (nodeName == ParticleEmitterNodeName)
	{
		auto particleFactory = readRenderable(getSingleChild(node), resources);
		double speed = getDoubleAttribute(node, SpeedAttributeName, 0.01);
		duration_t interval(getDoubleAttribute(node, IntervalAttributeName, 0.01));
		duration_t lifetime(getDoubleAttribute(node, LifetimeAttributeName, 0.5));

		return [=]()
		{
			return std::make_shared<si::view::ParticleEmitterRenderable>(
				particleFactory, speed, interval, lifetime);
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

/// Reads an on-enter controller. Its contains-model condition can be
/// negated, which enables this function to create on-leave controllers
/// as well.
template<bool negate>
ControllerBuilder SceneDescription::readOnEnterController(
	const tinyxml2::XMLElement* node,
	const SceneAssets& assets)
{
	double x = getDoubleAttribute(node, PositionXAttributeName, 0.0);
	double y = getDoubleAttribute(node, PositionYAttributeName, 0.0);
	double width = getDoubleAttribute(node, WidthAttributeName, 1.0);
	double height = getDoubleAttribute(node, HeightAttributeName, 1.0);

	auto eventFactory = parseTimelineEvent(getSingleChild(node), assets);

	si::DoubleRect rect(x, y, width, height);

	return [=](const std::shared_ptr<si::model::PhysicsEntity>& parent) -> UnboundController
	{
		return [=](Scene& scene) -> si::controller::IController_ptr
		{
			return std::make_shared<si::controller::ActionController>(
			[=, &scene](si::model::Game& game, duration_t) -> bool
			{
				auto parentPos = parent->getPosition();
				bool cond = rect.contains(parentPos);
				if (negate ? !cond : cond)
				{
					scene.startEvent(eventFactory());
					return false;
				}
				return game.contains(parent);
			});
		};
	};
}

/// Reads the controller that is described by the given node.
ControllerBuilder SceneDescription::readController(
	const tinyxml2::XMLElement* node,
	const SceneAssets& assets)
{
	std::string nodeName = node->Name();
	if (nodeName == GravityNodeName)
	{
		double gravitationalConstant = getDoubleAttribute(node, GravitationalConstantAttributeName);

		return [=](const std::shared_ptr<si::model::PhysicsEntity>& parent) -> UnboundController
		{
			return [=](Scene&) -> si::controller::IController_ptr
			{
				return std::make_shared<si::controller::GravityController>(
					parent, gravitationalConstant);
			};
		};
	}
	else if (nodeName == OnEnterNodeName)
	{
		return readOnEnterController<false>(node, assets);
	}
	else if (nodeName == OnLeaveNodeName)
	{
		return readOnEnterController<true>(node, assets);
	}
	else
	{
		throw SceneDescriptionException("Unexpected node type: '" + nodeName + "'.");
	}
}

/// Reads the given node's vector of associated controllers.
std::vector<ControllerBuilder> SceneDescription::readAssociatedControllers(
	const tinyxml2::XMLElement* node,
	const SceneAssets& assets)
{
	std::vector<ControllerBuilder> results;
	auto controllersNode = getSingleChild(node, ControllersNodeName, true);
	if (controllersNode == nullptr)
		return results;

	for (auto child = controllersNode->FirstChildElement();
		 child != nullptr;
		 child = child->NextSiblingElement())
	{
		results.push_back(readController(child, assets));
	}

	return results;
}

/// Creates a directed parsed entity from the given parameters.
/// This logic is common to ships, obstacles and projectiles.
template<typename TModel, typename TPathController, typename... TArgs>
static ParsedEntity<TModel> instantiateDirectedEntity(
	const Factory<si::view::IRenderable_ptr>& view,
	const std::vector<ControllerBuilder>& associatedControllers,
	TArgs... args)
{
	auto model = std::make_shared<TModel>(args...);
	std::vector<UnboundController> controllers;
	controllers.push_back(
		constantFunction<si::controller::IController_ptr, Scene&>(
			std::make_shared<TPathController>(model)));
	controllers.push_back(
		constantFunction<si::controller::IController_ptr, Scene&>(
			std::make_shared<si::controller::OutOfBoundsController>(model, GameBounds)));
	for (const auto& item : associatedControllers)
	{
		controllers.push_back(item(model));
	}
	return addControllers(createDirectedEntity(model, view()), controllers);
}

/// Reads a ship entity as specified by the given node.
ParsedShipFactory SceneDescription::readShipEntity(
	const tinyxml2::XMLElement* node,
	const SceneAssets& assets)
{
	auto physProps = getPhysicsProperties(node);
	Vector2d pos(
		getDoubleAttribute(node, PositionXAttributeName, 0.5),
		getDoubleAttribute(node, PositionYAttributeName, 0.5));
	double maxHealth = getDoubleAttribute(node, HealthAttributeName);
	auto view = readAssociatedView(node, assets);
	auto assocCtrlrs = readAssociatedControllers(node, assets);

	return readAssociatedEvents<si::model::ShipEntity>(node, assets, [=]()
	{
		return addDrainHealth(
			instantiateDirectedEntity<
				si::model::ShipEntity,
				si::controller::ShipCollisionController>(
					view, assocCtrlrs, physProps, pos, maxHealth));
	});
}

/// Reads a obstacle entity as specified by the given node.
ParsedObstacleFactory SceneDescription::readObstacleEntity(
	const tinyxml2::XMLElement* node,
	const SceneAssets& assets)
{
	auto physProps = getPhysicsProperties(node);
	Vector2d pos(
		getDoubleAttribute(node, PositionXAttributeName, 0.5),
		getDoubleAttribute(node, PositionYAttributeName, 0.5));
	double maxHealth = getDoubleAttribute(node, HealthAttributeName);
	auto view = readAssociatedView(node, assets);
	auto assocCtrlrs = readAssociatedControllers(node, assets);

	return readAssociatedEvents<si::model::ObstacleEntity>(node, assets, [=]()
	{
		return addDrainHealth(
			instantiateDirectedEntity<
				si::model::ObstacleEntity,
				si::controller::ObstacleCollisionController>(
					view, assocCtrlrs, physProps, pos, maxHealth));
	});
}

/// Reads a player entity as specified by the given node.
void SceneDescription::addPlayerToScene(
	const tinyxml2::XMLElement* node,
	const SceneAssets& assets,
	Scene& scene)
{
	// Read the player ship node, and instantiate it.
	auto player = readShipEntity(node, assets)();

	double velX = getDoubleAttribute(node, VelocityXAttributeName, 0.0);
	double velY = getDoubleAttribute(node, VelocityYAttributeName, 0.0);

	player.model->accelerate(si::Vector2d(velX, velY));

	// Add the player to the scene.
	addToScene(player, scene);

	// Register the player, and throw in a player
	// velocity controller while we're at it.
	double playerAccel = getDoubleAttribute(node, AccelerationAttributeName);
	scene.addController(std::make_shared<si::controller::PlayerController>(player.model, playerAccel));

	// Create a player projectile controller for this ship.
	double fireInterval = getDoubleAttribute(node, FireIntervalAttributeName);
	auto projectileFactory = readProjectileEntity(getSingleChild(node, ProjectileNodeName), assets);
	scene.addController(std::make_shared<si::controller::IntervalActionController>(si::duration_t(fireInterval),
		[](const si::model::Game&, si::duration_t) -> bool
		{
			return sf::Keyboard::isKeyPressed(sf::Keyboard::Space);
		},
		[player, projectileFactory, &scene](si::model::Game&, si::duration_t) -> void
		{
			fireAndAddProjectile(*player.model, projectileFactory, scene);
		},
		[=](const si::model::Game&, si::duration_t) -> bool
		{
			return player.model->isAlive();
		}));
}

/// Reads a projectile entity as specified by the given node.
/// The return type of this function is a parameterless function,
/// which can be used to create an arbitrary number of projectiles
/// on-demand.
ParsedDriftingEntityFactory SceneDescription::readProjectileEntity(
	const tinyxml2::XMLElement* node,
	const SceneAssets& assets)
{
	auto physProps = getPhysicsProperties(node);
	Vector2d pos(
		getDoubleAttribute(node, PositionXAttributeName, 0.0),
		getDoubleAttribute(node, PositionYAttributeName, 0.0));
	Vector2d veloc(
		getDoubleAttribute(node, VelocityXAttributeName),
		getDoubleAttribute(node, VelocityYAttributeName));
	auto view = readAssociatedView(node, assets);
	auto assocCtrlrs = readAssociatedControllers(node, assets);

	return readAssociatedEvents<si::model::DriftingEntity>(node, assets, [=]()
	{
		return instantiateDirectedEntity<
			si::model::DriftingEntity,
			si::controller::ProjectileCollisionController>(
				view, assocCtrlrs, physProps, pos, veloc);
	});
}

/// Reads a model entity specified by the given node.
ParsedEntityFactory<si::model::Entity> SceneDescription::readEntity(
	const tinyxml2::XMLElement* node,
	const SceneAssets& assets)
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
	else if (nodeName == ObstacleNodeName)
	{
		return readObstacleEntity(node, assets);
	}
	else
	{
		throw SceneDescriptionException("Unexpected node type: '" + nodeName + "'.");
	}
}

/// Reads a timeline as specified by the given node.
EventFactory SceneDescription::parseTimeline(
	const tinyxml2::XMLElement* node,
	const SceneAssets& assets)
{
	if (node == nullptr)
		return si::timeline::emptyTimeline;

	std::vector<EventFactory> events;
	for (auto child = node->FirstChildElement();
		 child != nullptr;
		 child = child->NextSiblingElement())
	{
		// Parse all events in the timeline.
		events.push_back(parseTimelineEvent(child, assets));
	}

	return [=]()
	{
		std::vector<si::timeline::ITimelineEvent_ptr> children;
		for (const auto& factory : events)
		{
			children.push_back(factory());
		}

		return std::make_shared<si::timeline::Timeline>(std::move(children));
	};
}

/// Reads a concurrent event as specified by the given node.
EventFactory SceneDescription::parseConcurrentEvent(
	const tinyxml2::XMLElement* node,
	const SceneAssets& assets)
{
	if (node == nullptr)
		return si::timeline::emptyTimeline;

	std::vector<EventFactory> events;
	for (auto child = node->FirstChildElement();
		 child != nullptr;
		 child = child->NextSiblingElement())
	{
		// Parse all child events.
		events.push_back(parseTimelineEvent(child, assets));
	}

	return [=]()
	{
		std::vector<si::timeline::ITimelineEvent_ptr> children;
		for (const auto& factory : events)
		{
			children.push_back(factory());
		}

		return std::make_shared<si::timeline::ConcurrentEvent>(std::move(children));
	};
}

/// Reads an invader wave event as specified by the given node.
EventFactory SceneDescription::parseWaveEvent(
	const tinyxml2::XMLElement* node,
	const SceneAssets& assets)
{
	if (node == nullptr)
		return si::timeline::emptyTimeline;

	auto shipNode = getSingleChild(node, ShipNodeName);
	auto shipFactory = readShipEntity(shipNode, assets);
	auto projectileFactory = readProjectileEntity(getSingleChild(node, ProjectileNodeName), assets);
	int rows = getIntAttribute(node, RowsAttributeName);
	int cols = getIntAttribute(node, ColumnsAttributeName);
	double velX = getDoubleAttribute(shipNode, VelocityXAttributeName, 0.05);
	double velY = getDoubleAttribute(shipNode, VelocityYAttributeName, 0.05);
	double springConst = getDoubleAttribute(shipNode, SpringConstantAttributeName, 5.0);
	si::duration_t fireInterval(getDoubleAttribute(shipNode, FireIntervalAttributeName, 1.0));
	si::duration_t maxDeviation(getDoubleAttribute(shipNode, FireIntervalDeviationAttributeName, 0.0));

	si::timeline::InvaderBehavior behavior = { Vector2d(velX, velY), springConst, fireInterval, maxDeviation };

	return [=]()
	{
		return std::make_shared<si::timeline::InvaderWaveEvent>(
			shipFactory, projectileFactory, rows,
			cols, behavior);
	};
}

/// Reads a condition event as specified by the given node.
EventFactory SceneDescription::parseConditionalEvent(
	const tinyxml2::XMLElement* node,
	const SceneAssets& assets)
{
	if (node == nullptr)
		return si::timeline::emptyTimeline;

	std::string attr = getAttribute(node, PredicateAttributeName);
	auto condition = [=](const Scene& scene) { return scene.getFlag(attr); };

	auto ifEvent = parseTimelineEvent(getSingleChild(getSingleChild(node, ThenNodeName)), assets);
	auto elseEvent = parseTimelineEvent(getSingleChild(getSingleChild(node, ElseNodeName)), assets);

	return [=]()
	{
		return std::make_shared<si::timeline::ConditionalEvent>(condition, ifEvent(), elseEvent());
	};
}

/// Reads a timeline event as specified by the given node.
EventFactory SceneDescription::parseTimelineEvent(
	const tinyxml2::XMLElement* node,
	const SceneAssets& assets)
{
	if (node == nullptr)
		return si::timeline::emptyTimeline;

	std::string nodeName = node->Name();
	if (nodeName == TimelineNodeName)
	{
		return parseTimeline(node, assets);
	}
	else if (nodeName == SpawnNodeName)
	{
		auto factory = readEntity(getSingleChild(node), assets);
		return [=]()
		{
			return factory().creationEvent;
		};
	}
	else if (nodeName == ShowNodeName)
	{
		auto factory = getReferenceAttribute(node, AssetAttributeName, assets.renderables);
		return [=]()
		{
			return std::make_shared<si::timeline::ShowEvent>(factory);
		};
	}
	else if (nodeName == WaveNodeName)
	{
		return parseWaveEvent(node, assets);
	}
	else if (nodeName == DeadlineNodeName)
	{
		duration_t duration(getDoubleAttribute(node, DurationAttributeName));
		auto inner = parseTimelineEvent(getSingleChild(node), assets);
		return [=]()
		{
			return std::make_shared<si::timeline::DeadlineEvent>(inner(), duration);
		};
	}
	else if (nodeName == PermanentNodeName)
	{
		auto inner = parseTimelineEvent(getSingleChild(node), assets);
		return [=]()
		{
			return std::make_shared<si::timeline::PermanentEvent>(inner());
		};
	}
	else if (nodeName == LoopNodeName)
	{
		auto inner = parseTimelineEvent(getSingleChild(node), assets);
		int maxIterationCount = getIntAttribute(node, MaxIterationCountAttributeName, 0);
		return [=]()
		{
			return std::make_shared<si::timeline::LoopedEvent>(inner(), maxIterationCount);
		};
	}
	else if (nodeName == BackgroundNodeName)
	{
		auto mainEvent = parseTimelineEvent(getSingleChild(node, MainNodeName), assets);
		auto extraEvent = parseTimelineEvent(getSingleChild(node, ExtraNodeName), assets);
		return [=]()
		{
			return std::make_shared<si::timeline::BackgroundEvent>(mainEvent(), extraEvent());
		};
	}
	else if (nodeName == SoundNodeName)
	{
		auto sound = getReferenceAttribute(node, SoundAttributeName, assets.sounds);
		return [=]()
		{
			return std::make_shared<si::timeline::SoundEvent>(sound);
		};
	}
	else if (nodeName == MusicNodeName)
	{
		auto music = getReferenceAttribute(node, MusicAttributeName, assets.music);
		return [=]()
		{
			return std::make_shared<si::timeline::MusicEvent>(music);
		};
	}
	else if (nodeName == SetFlagNodeName)
	{
		auto flagName = getAttribute(node, FlagAttributeName);
		auto flagValue = getBooleanAttribute(node, ValueAttributeName);

		return [=]()
		{
			return std::make_shared<si::timeline::InstantaneousEvent>(
				[=](Scene& target) { target.setFlag(flagName, flagValue); });
		};
	}
	else if (nodeName == ConcurrentNodeName)
	{
		return parseConcurrentEvent(node, assets);
	}
	else if (nodeName == ConditionNodeName)
	{
		return parseConditionalEvent(node, assets);
	}
	else
	{
		throw SceneDescriptionException("Unexpected node type: '" + nodeName + "'.");
	}
}

/// Reads a timed "show" event as specified by the given node.
/// A null node is interpreted as the empty event.
std::function<si::timeline::ITimelineEvent_ptr(const si::model::Entity_ptr&)> SceneDescription::parseTimedShowEvent(
	const tinyxml2::XMLElement* node,
	const SceneAssets& assets)
{
	if (node == nullptr)
	{
		return [=](const si::model::Entity_ptr&) -> si::timeline::ITimelineEvent_ptr
		{
			return si::timeline::emptyTimeline();
		};
	}

	auto asset = getReferenceAttribute(node, AssetAttributeName, assets.renderables);
	duration_t time(getDoubleAttribute(node, DurationAttributeName));

	return [=](const si::model::Entity_ptr& parent) -> si::timeline::ITimelineEvent_ptr
	{
		return std::make_shared<si::timeline::DeadlineEvent>(
			std::make_shared<si::timeline::ShowEvent>([=]() { return Scene::track(parent, asset()); }),
			time);
	};
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

/// Gets the value of the integer attribute with the
/// given name in the given XML node.
/// If no such attribute can be found, the given default
/// value is returned as a result.
int SceneDescription::getIntAttribute(const tinyxml2::XMLElement* node, const char* name, int defaultValue)
{
	int result;
	switch (node->QueryIntAttribute(name, &result))
	{
	case tinyxml2::XML_NO_ATTRIBUTE:
		return defaultValue;
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

/// Gets the value of the boolean attribute with the given
/// name in the given XML node.
/// If no such attribute can be found, an exception is thrown.
/// If the attribute's value was not recognized as a boolean literal,
/// an exception is thrown.
bool SceneDescription::getBooleanAttribute(const tinyxml2::XMLElement* node, const char* name)
{
	auto val = getAttribute(node, name);
	if (val == "true")
		return true;
	else if (val == "false")
		return false;
	else
		throw SceneDescriptionException(
			"'" + std::string(node->Name()) +
			"' node did have a '" + name +
			"' attribute, but its value ('" + val + "')" +
			" was neither 'true' nor 'false'. Expected a boolean nonetheless.");
}

/// Gets a value from the given key-value map
/// identified by the attribute with the given name
/// in the given node.
/// If something goes wrong, an exception is thrown.
template<typename T>
T SceneDescription::getReferenceAttribute(
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
			"', but no appropriate element could be found for '" + attr + "'.");
	}

	return map.at(attr);
}

/// Reads the given node's physics properties.
si::model::PhysicsProperties SceneDescription::getPhysicsProperties(
	const tinyxml2::XMLElement* node)
{
	double mass = getDoubleAttribute(node, MassAttributeName);
	double radius = getDoubleAttribute(node, RadiusAttributeName);

	return{ mass, radius };
}


/// Gets the only child of the given XML node, optionally
/// with the given name.
/// If this cannot be done, an exception is thrown, unless the
/// isOptional argument is set to true, in which case a null pointer
/// is returned.
const tinyxml2::XMLElement* SceneDescription::getSingleChild(const tinyxml2::XMLElement* node, const char* name, bool isOptional)
{
	if (node == nullptr)
	{
		if (isOptional)
		{
			return nullptr;
		}
		else
		{
			throw SceneDescriptionException(
				"No parent node was provided to look for '" + std::string(name) +
				"' nodes in.");
		}
	}

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
