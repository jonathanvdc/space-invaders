#pragma once

#include <cstddef>
#include <vector>
#include "Common.h"
#include "Container.h"
#include "ITimelineEvent.h"

namespace si
{
	namespace timeline
	{
		/// Defines a timeline: a class that executes
		/// events in sequence.
		class Timeline final : public ITimelineEvent 
		{
		public:
			/// Creates a new timeline from the given events, which
			/// are executed in sequence.
			Timeline(const std::vector<ITimelineEvent_ptr>& allEvents);

			/// Starts the timeline event.
			void start(Scene& target) final override;

			/// Has this timeline update the given scene.
			/// The event's associated scene, as well as
			/// the amount of time that has passed since the
			/// last update, are given. A boolean flag is
			/// returned that tells if this timeline event
			/// is still running.
			bool update(Scene& target, duration_t timeDelta) final override;

			/// Applies this timeline's finalization 
			/// logic to the given scene: objects associated
			/// with this event are removed from the scene.
			/// This method can either be called to perform
			/// cleanup after an event has ended, or 
			/// to forcibly end the event. It should
			/// be called exactly once for every event that
			/// has been started.
			void end(Scene& target) final override;

			/// Gets a boolean value that tells if this
			/// timeline is running right now.
			bool isRunning() const;

		private:
			/// Gets the currently executing event, assuming 
			/// that the timeline has not ended yet.
			ITimelineEvent& getCurrentEvent();

			const std::vector<ITimelineEvent_ptr> allEvents;
			std::size_t currentEventIndex;
		};
	}
}
