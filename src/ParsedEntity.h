#pragma once

#include <memory>
#include "Entity.h"
#include "IController.h"
#include "IRenderable.h"

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
	}
}