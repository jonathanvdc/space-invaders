#pragma once
#include <vector>
#include "Common.h"
#include "Entity.h"

namespace si
{
	namespace model
	{
		/// An entity that describes the game itself.
		class Game final : public Entity
		{
		public:
			/// Gets the game's origin.
			Vector2d getPosition() const final override;

			/// Adds the given time delta to the total amount 
			/// of time elapsed.
			/// This will recursively update all entities in the
			/// game.
			void updateTime(duration_t delta) final override;

			/// Gets all entities in the game that are of 
			/// the given type.
			template<typename T>
			std::vector<std::shared_ptr<T>> getEntities() const
			{
				std::vector<std::shared_ptr<T>> results;
				for (const auto& item : this->entities)
				{
					auto inst = std::dynamic_pointer_cast<T>(item);
					if (inst != nullptr)
					{
						results.push_back(inst);
					}
				}
				return results;
			}

			/// Adds an entity to this game.
			void addEntity(Entity_ptr item);
			/// Removes an entity from the game.
			/// A boolean is returned that indicates
			/// whether the action was successful.
			bool removeEntity(Entity_ptr item);
		private:
			std::vector<Entity_ptr> entities;
		};
	}
}