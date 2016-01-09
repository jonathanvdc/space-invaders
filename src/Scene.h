#pragma once

#include <chrono>
#include <exception>
#include <functional>
#include <map>
#include <string>
#include <SFML/Graphics.hpp>
#include "model/Entity.h"
#include "model/ShipEntity.h"
#include "model/Game.h"
#include "controller/IController.h"
#include "controller/GameController.h"
#include "view/IRenderable.h"
#include "view/RenderContext.h"
#include "view/GameRenderer.h"
#include "timeline/Timeline.h"

namespace si
{
	/// Defines a "scene" class, which
	/// contains the model, view, and
	/// controller for an entire game.
	class Scene final
	{
	public:
		/// Creates a new scene, with the given background color, and the
		/// given name. The scene also recommends the given dimensions
		/// for its render target.
		Scene(
			const std::string& name,
			sf::Vector2u dimensions = sf::Vector2u(800, 800),
			sf::Color backgroundColor = sf::Color::Black);

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

		/// Starts the given timeline event for this
		/// scene. The timeline event will be updated
		/// on every frame, until it has ended, at
		/// which point its `end` method will be called.
		void startEvent(
			const si::timeline::ITimelineEvent_ptr& item);

		/// Gets the boolean flag with the given name.
		bool getFlag(const std::string& name) const;

		/// Sets the boolean flag with the given name to the
		/// given value.
		void setFlag(const std::string& name, bool value);

		/// Gets this scene's name.
		std::string getName() const;

		/// Gets recommended dimensions for this scene's render target.
		sf::Vector2u getDimensions() const;

		/// Gets this scene's game.
		si::model::Game& getGame();

		/// Gets this scene's game.
		const si::model::Game& getGame() const;

		/// Gets this scene's renderer.
		si::view::GameRenderer& getRenderer();

		/// Gets this scene's renderer.
		const si::view::GameRenderer& getRenderer() const;

		/// Gets this scene's controller.
		si::controller::GameController& getController();

		/// Gets this scene's controller.
		const si::controller::GameController& getController() const;

		/// Creates a renderable from the given view that
		/// traces the given entity's position.
		static si::view::IRenderable_ptr track(
			const si::model::Entity_ptr& model,
			const si::view::IRenderable_ptr& view);

		/// Creates a renderable from the given view that
		/// traces the given entity's position and
		/// orientation.
		static si::view::IRenderable_ptr direct(
			const std::shared_ptr<si::model::PhysicsEntity>& model,
			const si::view::IRenderable_ptr& view);

	private:
		/// Updates all events that are currently running,
		/// and removes any events that have ended.
		void updateEvents(duration_t timeDelta);

		std::string name;
		sf::Vector2u dimensions;

		si::model::Game game;
		si::view::GameRenderer renderer;
		si::controller::GameController controller;
		std::vector<si::timeline::ITimelineEvent_ptr> sceneEvents;
		std::map<si::model::Entity_ptr, si::view::IRenderable_ptr> associatedView;
		std::map<std::string, bool> flags;
	};
}
