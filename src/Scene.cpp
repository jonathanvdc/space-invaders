#include "Scene.h"

#include <algorithm>
#include <chrono>
#include <exception>
#include <functional>
#include <map>
#include <SFML/Graphics.hpp>
#include "Entity.h"
#include "Game.h"
#include "IController.h"
#include "GameController.h"
#include "OutOfBoundsController.h"
#include "IRenderable.h"
#include "RenderContext.h"
#include "GameRenderer.h"
#include "PathOffsetRenderable.h"

using namespace si;

Scene::Scene(const std::string& name)
	: Scene(name, sf::Color::Black)
{ }

Scene::Scene(const std::string& name, sf::Color backgroundColor)
	: name(name), game(), renderer(backgroundColor), controller(), associatedView()
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
	auto context = si::view::RenderContext(renderTarget, timeDelta);
	this->renderer.render(context, context.getBounds());
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

/// Adds an entity that is associated with as
/// view to this scene. The view will
/// be wired to track the entity's position.
void Scene::addTrackedEntity(
	const si::model::Entity_ptr& model,
	const si::view::IRenderable_ptr& view)
{
	this->addEntity(model, std::make_shared<si::view::PathOffsetRenderable>(view, [=]() { return model->getPosition(); }));
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
		[](const std::shared_ptr<si::model::ShipEntity>& item) { return item->isAlive(); });
}

/// Registers the given ship as a player ship.
void Scene::registerPlayer(const std::shared_ptr<si::model::ShipEntity>& player)
{
	this->players.push_back(player);
}