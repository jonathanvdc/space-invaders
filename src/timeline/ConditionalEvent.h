#pragma once

#include <functional>
#include <memory>
#include "ITimelineEvent.h"
#include "Scene.h"

namespace si
{
	namespace timeline
	{
		/// Defines a conditional event: an event that yields control
		/// to one of its child events, based on whether a predicate
		/// function evaluates to true or false.
		class ConditionalEvent final : public ITimelineEvent 
		{
		public:
			typedef std::function<bool(const Scene&)> ScenePredicate;

			/// Creates a conditional event from the given
			/// condition, if-clause event, and else-clause
			/// event. An optional boolean specifies whether
			/// the condition should be re-evaluated even
			/// after an event has already been selected.
			ConditionalEvent(
				const ScenePredicate& condition,
				const ITimelineEvent_ptr& ifEvent,
				const ITimelineEvent_ptr& elseEvent,
				bool shouldReevaluate = true);

			/// Starts the timeline event.
			void start(Scene& target) final override;

			/// Has this timeline event update the given scene.
			/// The event's associated scene, as well as
			/// the amount of time that has passed since the
			/// last update, are given. A boolean flag is
			/// returned that tells if this timeline event
			/// is still running.
			bool update(Scene& target, duration_t timeDelta) final override;

			/// Applies this timeline event's finalization 
			/// logic to the given scene: objects associated
			/// with this event are removed from the scene.
			/// This method can either be called to perform
			/// cleanup after an event has ended, or 
			/// to forcibly end the event. It should
			/// be called exactly once for every event that
			/// has been started.
			void end(Scene& target) final override;

			/// Checks if this event is currently running.
			bool isRunning() const;

		private:
			/// Re-evaluates the condition, ending the 
			/// current event and switching to the other
			/// event if necessary.
			void reevaluateCondition(Scene& target);

			/// Gets the currently selected event.
			const ITimelineEvent_ptr& selectedEvent() const;

			const ScenePredicate condition;
			const ITimelineEvent_ptr ifEvent;
			const ITimelineEvent_ptr elseEvent;
			const bool shouldReevaluate;
			enum { NotRunning, IfClause, ElseClause } state;
		};
	}
}