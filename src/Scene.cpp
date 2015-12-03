#include "Scene.h"

#include <algorithm>
#include <chrono>
#include <exception>
#include <functional>
#include <map>
#include <SFML/Graphics.hpp>
#include "Entity.h"
#include "Game.h"
#include "ProjectileEntity.h"
#include "IController.h"
#include "GameController.h"
#include "IRenderable.h"
#include "RenderContext.h"
#include "GameRenderer.h"
#include "SpriteRenderable.h"
#include "RelativeBoxRenderable.h"
#include "PathOffsetRenderable.h"
#include "Stopwatch.h"

using namespace si;

Scene::Scene()
	: Scene(sf::Color::Black)
{
}

Scene::Scene(sf::Color backgroundColor)
	: game(), renderer(backgroundColor), controller(), associatedView()
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

/// Adds the given controller to this scene.
void Scene::addController(
	const si::controller::IController_ptr& item)
{
	this->controller.add(item);
}
