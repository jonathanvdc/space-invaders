#pragma once

#include <memory>
#include "Common.h"
#include "Scene.h"

namespace si
{
	namespace timeline
	{
		/// A base class for events on a timeline.
		class ITimelineEvent
		{
		public:
			/// Destroys a timeline event.
			virtual ~ITimelineEvent() { }

			/// Starts the timeline event.
			virtual void start(Scene& target) = 0;

			/// Has this timeline event update the given scene.
			/// The event's associated scene, as well as
			/// the amount of time that has passed since the
			/// last update, are given. A boolean flag is
			/// returned that tells if this timeline event
			/// is still running.
			virtual bool update(Scene& target, duration_t timeDelta) = 0;

			/// Applies this timeline event's finalization 
			/// logic to the given scene: objects associated
			/// with this event are removed from the scene.
			/// This method can either be called to perform
			/// cleanup after an event has ended, or 
			/// to forcibly end the event. It should
			/// be called exactly once for every event that
			/// has been started.
			virtual void end(Scene& target) = 0;
		};

		typedef std::shared_ptr<ITimelineEvent> ITimelineEvent_ptr;
	}
}