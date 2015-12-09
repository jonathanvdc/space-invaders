#pragma once

#include <chrono>
#include <exception>
#include <functional>
#include <map>
#include <string>
#include <SFML/Graphics.hpp>
#include "Entity.h"
#include "ShipEntity.h"
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
		/// black background color, and
		/// the given name.
		Scene(const std::string& name);

		/// Creates a new scene, with the 
		/// given background color, and the
		/// given name.
		Scene(const std::string& name, sf::Color backgroundColor);

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
		
		/// Adds an entity that is associated with as
		/// view to this scene. The view will
		/// be wired to track the entity's position.
		void addTrackedEntity(
			const si::model::Entity_ptr& model,
			const si::view::IRenderable_ptr& view);

		/// Adds a renderable (view) element to 
		/// this scene that is not associated 
		/// with anything in the model. This
		/// can be useful to construct a 
		/// background, or an HUD.
		void addRenderable(
			const si::view::IRenderable_ptr& view);

		/// Constrains the given entity to the
		/// given bounds (in relative coordinates).
		/// Once exceeded, the entity is removed
		/// from the game.
		void addBoundsConstraint(
			const si::model::Entity_ptr& model,
			si::DoubleRect bounds);

		/// Adds the given controller to this scene.
		void addController(
			const si::controller::IController_ptr& item);

		/// Gets a vector containing all players that
		/// are still alive in this scene.
		std::vector<std::shared_ptr<si::model::ShipEntity>> getPlayers() const;

		/// Checks if any player ships are still alive.
		bool anyPlayersAlive() const;

		/// Registers the given ship as a player ship.
		void registerPlayer(const std::shared_ptr<si::model::ShipEntity>& player);

	private:
		std::string name;
		si::model::Game game;
		si::view::GameRenderer renderer;
		si::controller::GameController controller;
		std::vector<std::shared_ptr<si::model::ShipEntity>> players;
		std::map<si::model::Entity_ptr, si::view::IRenderable_ptr> associatedView;
	};
}