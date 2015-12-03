#pragma once

#include <functional>
#include <vector>

namespace si
{	
	/// Defines a templated event class.
	/// Events consist of a variable number of
	/// event handlers, which are called
	/// when the event is raised.
	template<typename F>
	class Event;
	// No generic implementation for Event<F>.
	// Use specific specializations instead.

	/// Defines a base class for event handlers,
	/// which supports creating a list of handlers, but
	/// not calling them.
	template<typename TRet, typename... TArgs>
	class EventBase
	{
	public:
		/// Defines a type for event handler functions.
		typedef std::function<TRet(TArgs...)> EventHandler;

		/// Adds a handler to this event's list of handlers.
		void addHandler(const EventHandler& handler)
		{
			this->handlers.push_back(handler);
		}
	protected:
		std::vector<EventHandler> handlers;
	};

	/// Defines a templated event class.
	/// Events consist of a variable number of
	/// event handlers, which are called
	/// when the event is raised.
	template<typename TRet, typename... TArgs>
	class Event<TRet(TArgs...)> final : public EventBase<TRet, TArgs...>
	{
	public:
		/// Raises this event. All handlers are
		/// notified. The last invoked function's
		/// return value is returned.
		TRet operator()(TArgs... args)
		{
			// Copy the handler vector.
			// Since function calls can do just about 
			// anything, we need to make sure that they 
			// do not in any way modify the handler list,
			// as this could corrupt the iterators
			// we're using in the for loop below.
			auto copy = this->handlers;
			TRet result;
			for (const auto& item : copy)
			{
				result = item(args...);
			}
			return result;
		}
	};

	/// Defines a templated event class.
	/// Events consist of a variable number of
	/// event handlers, which are called
	/// when the event is raised.
	template<typename... TArgs>
	class Event<void(TArgs...)> final : public EventBase<void, TArgs...>
	{
	public:
		/// Raises this event. All handlers are
		/// notified.
		void operator()(TArgs... args)
		{
			// Copy the handler vector.
			// Since function calls can do just about 
			// anything, we need to make sure that they 
			// do not in any way modify the handler list,
			// as this could corrupt the iterators
			// we're using in the for loop below.
			auto copy = this->handlers;
			for (const auto& item : copy)
			{
				item(args...);
			}
		}
	};
}