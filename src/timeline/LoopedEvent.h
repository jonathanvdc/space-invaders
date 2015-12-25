#pragma once

#include "Common.h"
#include "ITimelineEvent.h"
#include "Scene.h"

namespace si
{
	namespace timeline
	{
		/// Defines an event that "loops" a child event by starting it,
        /// and re-starting it once it has ended.
		class LoopedEvent final : public ITimelineEvent
		{
		public:
			/// Creates a timeline event that loops the given inner event. 
			LoopedEvent(
				const si::timeline::ITimelineEvent_ptr& innerEvent);

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
		private:
			si::timeline::ITimelineEvent_ptr innerEvent;
		};
	}
}
