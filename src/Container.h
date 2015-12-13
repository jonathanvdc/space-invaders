#pragma once
#include <memory>
#include <vector>
#include "Event.h"

namespace si
{
	/// Defines a base class for objects that
	/// store a number of shared pointers
	/// to items.
	template<typename T>
	class Container
	{
	public:
		/// Adds the given item to this container.
		void add(const std::shared_ptr<T>& item)
		{
			items.push_back(item);
		}
		/// Adds the given item to this container.
		void add(std::shared_ptr<T>&& item)
		{
			items.push_back(item);
		}

		/// Registers the given event handler, which
		/// will be called whenever an item is removed
		/// from this container.
		void registerRemoveHandler(std::function<void(const std::shared_ptr<T>&)> handler)
		{
			removedEvent.addHandler(handler);
		}

		/// Tries to remove the given item from this
		/// container. If this cannot be done, false 
		/// is returned. Otherwise, true is returned.
		bool remove(const std::shared_ptr<T>& item)
		{
			auto preCount = this->items.size();
			this->items.erase(
				std::remove(this->items.begin(), this->items.end(), item), 
				this->items.end());
			if (preCount > this->items.size())
			{
				removedEvent(item);
				return true;
			}
			else
			{
				return false;
			}
		}

		/// Tests if the given item is currently in
		/// this container.
		bool contains(const std::shared_ptr<T>& item) const
		{
			return std::find(this->items.begin(), this->items.end(), item) != this->items.end();
		}

		/// Gets all items in this container that
		/// are of the given type.
		template<typename V>
		std::vector<std::shared_ptr<V>> getAll() const
		{
			std::vector<std::shared_ptr<V>> results;
			for (const auto& item : this->items)
			{
				auto inst = std::dynamic_pointer_cast<V>(item);
				if (inst != nullptr)
				{
					results.push_back(inst);
				}
			}
			return results;
		}

	protected:
		std::vector<std::shared_ptr<T>> items;

	private:
		Event<void(const std::shared_ptr<T>&)> removedEvent;
	};
}