#pragma once

#include <cstddef>
#include <set>
#include <vector>
#include "Common.h"
#include "ITimelineEvent.h"
#include "Scene.h"

namespace si
{
	namespace timeline
	{
		/// Defines a concurrent event: an event that launches
		/// a variable number of sub-events when started, and
		/// remains active until the last sub-event has
		/// ended.
		class ConcurrentEvent final : public ITimelineEvent
		{
		public:
			/// Creates a new concurrent event from the given
			/// vector of sub-events.
			ConcurrentEvent(const std::vector<ITimelineEvent_ptr>& subEvents);

			/// Creates a new concurrent event from the given
			/// vector of sub-events.
			ConcurrentEvent(std::vector<ITimelineEvent_ptr>&& subEvents);

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

			/// Gets a boolean value that tells if this
			/// concurrent event is running right now.
			bool isRunning() const;
		private:
			const std::vector<ITimelineEvent_ptr> subEvents;
			std::set<std::size_t> runningEventIndices;
		};
	}
}