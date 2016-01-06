#pragma once

#include <memory>
#include "model/Entity.h"
#include "model/ShipEntity.h"
#include "model/DriftingEntity.h"
#include "model/ObstacleEntity.h"
#include "controller/IController.h"
#include "view/IRenderable.h"
#include "timeline/ITimelineEvent.h"
#include "timeline/ConcurrentEvent.h"
#include "timeline/SpawnEvent.h"
#include "timeline/InstantaneousEvent.h"
#include "timeline/FinalizedEvent.h"
#include "Scene.h"

namespace si
{
	namespace parser
	{
		/// Defines a struct that holds data for
		/// entities that have been parsed.
		template<typename T>
		struct ParsedEntity
		{
			/// Creates a parsed entity from the given components.
			ParsedEntity(
				const std::shared_ptr<T>& model,
				const si::timeline::ITimelineEvent_ptr& creationEvent)
				: model(model), creationEvent(creationEvent)
			{ }

			/// Creates a parsed entity from the given components.
			ParsedEntity(
				std::shared_ptr<T>&& model,
				si::timeline::ITimelineEvent_ptr&& creationEvent)
				: model(model), creationEvent(creationEvent)
			{ }

			/// Copies a parsed entity.
			template<typename V>
			ParsedEntity(const ParsedEntity<V>& other)
				: ParsedEntity(other.model, other.creationEvent)
			{ }

			/// Moves a parsed entity.
			template<typename V>
			ParsedEntity(ParsedEntity<V>&& other)
				: ParsedEntity(other.model, other.creationEvent)
			{ }

			/// Copy-assigns a parsed entity.
			template<typename V>
			ParsedEntity<T>& operator=(const ParsedEntity<V>& other)
			{
				this->model = other.model;
				this->creationEvent = other.creationEvent;
			}

			/// Move-assigns a parsed entity.
			template<typename V>
			ParsedEntity<T>& operator=(ParsedEntity<V>&& other)
			{
				this->model = std::move(other.model);
				this->creationEvent = std::move(other.creationEvent);
			}

			/// The parsed entity's model.
			std::shared_ptr<T> model;
			/// An event that adds the parsed entity
			/// to the scene.
			si::timeline::ITimelineEvent_ptr creationEvent;
		};

		template<typename T>
		using Factory = std::function<T()>;

		using EventFactory = Factory<si::timeline::ITimelineEvent_ptr>;

		template<typename T>
		using ParsedEntityFactory = Factory<ParsedEntity<T>>;

		using ParsedShipFactory = ParsedEntityFactory<si::model::ShipEntity>;
		using ParsedObstacleFactory = ParsedEntityFactory<si::model::ObstacleEntity>;
		using ParsedDriftingEntityFactory = ParsedEntityFactory<si::model::DriftingEntity>;

		using UnboundController =
			std::function<si::controller::IController_ptr(Scene&)>;

		using ControllerBuilder =
			std::function<UnboundController(const std::shared_ptr<si::model::PhysicsEntity>&)>;

		/// Creates a parsed entity from the given physics model
		/// and view. The created timeline will spawn the model,
		/// and associate it with a renderable that tracks the
		/// model's position.
		template<typename T>
		ParsedEntity<T> createTrackedEntity(
			const std::shared_ptr<T>& model,
			const si::view::IRenderable_ptr& view)
		{
			return ParsedEntity<T>(
				model,
				std::make_shared<si::timeline::SpawnEvent>(
					model,
					Scene::track(model, view)));
		}

		/// Creates a parsed entity from the given physics model
		/// and view. The created timeline will spawn the model,
		/// and associate it with a renderable that tracks the
		/// model's position and orientation.
		template<typename T>
		ParsedEntity<T> createDirectedEntity(
			const std::shared_ptr<T>& model,
			const si::view::IRenderable_ptr& view)
		{
			return ParsedEntity<T>(
				model,
				std::make_shared<si::timeline::SpawnEvent>(
					model,
					Scene::direct(model, view)));
		}

		/// Creates an event that adds the given vector
		/// of controllers to the scene.
		si::timeline::ITimelineEvent_ptr createAddControllersEvent(
			const std::vector<UnboundController>& items);

		/// Creates an event that adds the given vector
		/// of controllers to the scene.
		si::timeline::ITimelineEvent_ptr createAddControllersEvent(
			const std::vector<si::controller::IController_ptr>& items);

		/// Adds the given vector of controllers to a parsed entity's
		/// creation event.
		template<typename T>
		ParsedEntity<T> addControllers(
			const ParsedEntity<T>& target,
			const std::vector<UnboundController>& items)
		{
			return ParsedEntity<T>(
				target.model,
				si::timeline::concurrent({
					target.creationEvent,
					createAddControllersEvent(items)
				}));
		}

		/// Appends an event that drains the given parsed entity's
		/// health to this parsed entity.
		template<typename T>
		ParsedEntity<T> addDrainHealth(const ParsedEntity<T>& target)
		{
			auto model = target.model;

			return ParsedEntity<T>(
				model,
				si::timeline::finalize(
					target.creationEvent,
					[model](Scene&) -> void
					{
						model->getHealth().setHealth(0.0);
					}));
		}

		/// Applies the given parsed entity's creation event
		/// to a scene.
		void addToScene(
			const ParsedEntity<si::model::Entity>& entity,
			Scene& target);

		/// Creates a bullet that is fired from the given source.
		/// Momentum is transferred from the source entity to
		/// the projectile, but the bullet is not added to the
		/// scene.
		ParsedEntity<si::model::DriftingEntity> fireProjectile(
			si::model::DriftingEntity& source,
			const ParsedDriftingEntityFactory& projectileFactory);

		/// Creates a bullet that is fired from the given source.
		/// Momentum is transferred from the source entity to
		/// the projectile, after which the projectile is added
		/// to the scene.
		void fireAndAddProjectile(
			si::model::DriftingEntity& source,
			const ParsedDriftingEntityFactory& projectileFactory,
			Scene& target);

		/// Creates a function object that returns a copy of the given value. 
		template<typename T, typename... TArgs>
		std::function<T(TArgs...)> constantFunction(T result)
		{
			return [=](TArgs...) { return result; };
		}
	}
}
