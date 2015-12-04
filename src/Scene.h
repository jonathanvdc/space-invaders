#pragma once

#include <chrono>
#include <exception>
#include <functional>
#include <map>
#include <SFML/Graphics.hpp>
#include "Entity.h"
#include "Game.h"
#include "IController.h"
#include "GameController.h"
#include "IRenderable.h"
#include "RenderContext.h"
#include "GameRenderer.h"

namespace si
{
	/// Defines a "scene" class, which
	/// contains the model, view, and 
	/// controller for an entire game.
	class Scene final
	{
	public:
		/// Creates a new scene, with a
		/// black background color.
		Scene();

		/// Creates a new scene, with the 
		/// given background color.
		Scene(sf::Color backgroundColor);

		Scene(const Scene& other) = delete;

		/// Renders a single frame to the
		/// given render target. A duration
		/// tells us how much time has
		/// passed since the previous frame.
		void frame(sf::RenderTarget& renderTarget, duration_t timeDelta);

		/// Adds an entity that is associated with
		/// a view to this scene.
		void addEntity(
			const si::model::Entity_ptr& model,
			const si::view::IRenderable_ptr& view);

		/// Adds a renderable (view) element to 
		/// this scene that is not associated 
		/// with anything in the model. This
		/// can be useful to construct a 
		/// background, or an HUD.
		void addRenderable(
			const si::view::IRenderable_ptr& view);

		/// Adds the given controller to this scene.
		void addController(
			const si::controller::IController_ptr& item);

	private:
		si::model::Game game;
		si::view::GameRenderer renderer;
		si::controller::GameController controller;
		std::map<si::model::Entity_ptr, si::view::IRenderable_ptr> associatedView;
	};
}