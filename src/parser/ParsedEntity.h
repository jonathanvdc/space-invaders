#pragma once

#include <memory>
#include "model/Entity.h"
#include "model/ShipEntity.h"
#include "model/ProjectileEntity.h"
#include "model/DriftingEntity.h"
#include "controller/IController.h"
#include "view/IRenderable.h"
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
		using Factory = std::function<T()>;

		template<typename T>
		using ParsedEntityFactory = Factory<ParsedEntity<T>>;

		using ParsedShipFactory = ParsedEntityFactory<si::model::ShipEntity>;
		using ParsedProjectileFactory = ParsedEntityFactory<si::model::ProjectileEntity>;

		/// Adds the given entity's model, view and
		/// controllers to the given scene.
		void addToScene(
			const ParsedEntity<si::model::Entity>& entity,
			Scene& target);

		/// Creates a bullet that is fired from the given source. 
		/// Momentum is transferred from the source entity to
		/// the projectile, but the bullet is not added to the
		/// scene.
		ParsedEntity<si::model::ProjectileEntity> fireProjectile(
			si::model::DriftingEntity& source,
			const ParsedProjectileFactory& projectileFactory);

		/// Creates a bullet that is fired from the given source. 
		/// Momentum is transferred from the source entity to
		/// the projectile, after which the projectile is added
		/// to the scene.
		void fireAndAddProjectile(
			si::model::DriftingEntity& source,
			const ParsedProjectileFactory& projectileFactory,
			Scene& target);
	}
}