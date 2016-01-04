#include "Scene.h"

#include <algorithm>
#include <chrono>
#include <exception>
#include <functional>
#include <map>
#include <set>
#include <SFML/Graphics.hpp>
#include "model/Entity.h"
#include "model/ShipEntity.h"
#include "model/Game.h"
#include "controller/IController.h"
#include "controller/GameController.h"
#include "controller/OutOfBoundsController.h"
#include "view/IRenderable.h"
#include "view/RenderContext.h"
#include "view/GameRenderer.h"
#include "view/PathOffsetRenderable.h"
#include "view/TransformedRenderable.h"
#include "timeline/Timeline.h"

using namespace si;

Scene::Scene(const std::string& name)
	: Scene(name, sf::Color::Black)
{ }

Scene::Scene(const std::string& name, sf::Color backgroundColor)
	: name(name), game(), renderer(backgroundColor), controller(),
	  sceneEvents(), associatedView(), flags()
{
	// Create an event handler that removes the
	// associated view when the model is removed.
	game.registerRemoveHandler([&](const si::model::Entity_ptr& item)
	{
		auto assocView = this->associatedView[item];
		if (assocView != nullptr)
		{
			// Remove the associated view from the map.
			renderer.remove(assocView);
			// Remove the model from the map.
			this->associatedView.erase(item);
		}
	});
}

/// Renders a single frame to the
/// given render target. A duration
/// tells us how much time has
/// passed since the previous frame.
void Scene::frame(sf::RenderTarget& renderTarget, duration_t timeDelta)
{
	this->game.updateTime(timeDelta);
	this->controller.update(this->game, timeDelta);
	this->updateEvents(timeDelta);
	auto context = si::view::RenderContext(renderTarget, timeDelta);
	this->renderer.render(context, context.getBounds(), si::view::Transformation());
}

/// Adds an entity that is associated with
/// a view to this scene.
void Scene::addEntity(
	const si::model::Entity_ptr& model,
	const si::view::IRenderable_ptr& view)
{
	this->game.add(model);
	this->renderer.add(view);
	this->associatedView[model] = view;
}

/// Adds a renderable (view) element to
/// this scene that is not associated
/// with anything in the model. This
/// can be useful when constructing a
/// background, or an HUD.
void Scene::addRenderable(
	const si::view::IRenderable_ptr& view)
{
	this->renderer.add(view);
}

/// Constrains the given entity to the
/// given bounds (in relative coordinates).
/// Once exceeded, the entity is removed
/// from the game.
void Scene::addBoundsConstraint(
	const si::model::Entity_ptr& model,
	si::DoubleRect bounds)
{
	this->addController(std::make_shared<si::controller::OutOfBoundsController>(model, bounds));
}

/// Adds the given controller to this scene.
void Scene::addController(
	const si::controller::IController_ptr& item)
{
	this->controller.add(item);
}

/// Starts the given timeline event for this
/// scene. The timeline event will be updated
/// on every frame, until it has ended, at
/// which point its `end` method will be called.
void Scene::startEvent(
	const si::timeline::ITimelineEvent_ptr& item)
{
	// Add the event to the events vector.
	this->sceneEvents.push_back(item);
	// Start the event.
	item->start(*this);
}

/// Gets a vector containing all players that
/// are still alive in this scene.
std::vector<std::shared_ptr<si::model::ShipEntity>> Scene::getPlayers() const
{
	std::vector<std::shared_ptr<si::model::ShipEntity>> results;
	for (const auto& item : this->players)
	{
		if (item->isAlive())
		{
			results.push_back(item);
		}
	}
	return results;
}

/// Checks if any player ships are still alive.
bool Scene::anyPlayersAlive() const
{
	return std::any_of(this->players.begin(), this->players.end(),
		[&](const std::shared_ptr<si::model::ShipEntity>& item)
		{
			return item->isAlive() && this->game.contains(item);
		});
}

/// Registers the given ship as a player ship.
void Scene::registerPlayer(const std::shared_ptr<si::model::ShipEntity>& player)
{
	this->players.push_back(player);
}

/// Gets the boolean flag with the given name.
bool Scene::getFlag(const std::string& name) const
{
	return this->flags.at(name);
}

/// Sets the boolean flag with the given name to the
/// given value.
void Scene::setFlag(const std::string& name, bool value)
{
	this->flags[name] = value;
}

/// Gets this scene's name.
std::string Scene::getName() const
{
	return this->name;
}

/// Gets this scene's game.
si::model::Game& Scene::getGame()
{
	return this->game;
}

/// Gets this scene's game.
const si::model::Game& Scene::getGame() const
{
	return this->game;
}

/// Gets this scene's renderer.
si::view::GameRenderer& Scene::getRenderer()
{
	return this->renderer;
}

/// Gets this scene's renderer.
const si::view::GameRenderer& Scene::getRenderer() const
{
	return this->renderer;
}

/// Gets this scene's controller.
si::controller::GameController& Scene::getController()
{
	return this->controller;
}

/// Gets this scene's controller.
const si::controller::GameController& Scene::getController() const
{
	return this->controller;
}

void Scene::updateEvents(duration_t timeDelta)
{
	// Create a copy of the events vector, because updating events could result
	// in changes to the events vector.
	std::vector<si::timeline::ITimelineEvent_ptr> eventsCopy = this->sceneEvents;
	// Maintain a set of dead events.
	std::set<si::timeline::ITimelineEvent_ptr> deadEvents;

	// Iterate over all events.
	for (const auto& item : eventsCopy)
	{
		// Update the scene based on this event.
		if (!item->update(*this, timeDelta))
		{
			// If the event told us it's dead, then
			// insert it in the dead events set.
			deadEvents.insert(item);
		}
	}
	// Remove all events that were marked dead in the previous pass.
	this->sceneEvents.erase(std::remove_if(this->sceneEvents.begin(), this->sceneEvents.end(),
		[&](const si::timeline::ITimelineEvent_ptr& item) -> bool
		{
			return deadEvents.find(item) != deadEvents.end();
		}), this->sceneEvents.end());
}

/// Creates a renderable from the given view that
/// traces the given entity's position.
si::view::IRenderable_ptr Scene::track(
	const si::model::Entity_ptr& model,
	const si::view::IRenderable_ptr& view)
{
	return std::make_shared<si::view::PathOffsetRenderable>(
		view,
		[=]() { return model->getPosition(); });
}

/// Creates a renderable from the given view that
/// traces the given entity's position and
/// orientation.
si::view::IRenderable_ptr Scene::direct(
	const std::shared_ptr<si::model::PhysicsEntity>& model,
	const si::view::IRenderable_ptr& view)
{
	return std::make_shared<si::view::TransformedRenderable>(
		view,
		[=](DoubleRect bounds)
		{
			auto pos = model->getPosition();
			auto radius = model->getPhysicsProperties().radius;
			DoubleRect rect(
				pos.x * bounds.width + bounds.top,
				pos.y * bounds.height + bounds.left,
				bounds.width * radius * 2.0,
				bounds.height * radius * 2.0);

			Vector2d center(rect.left + rect.width / 2.0, rect.top + rect.height / 2.0);
			return std::make_pair(
				si::view::Transformation::rotate(
					model->getOrientationAngle() + si::view::Transformation::pi / 2.0,
					center),
				rect);
		});
}
