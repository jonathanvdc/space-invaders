#pragma once

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

			/// Checks if this timeline event has ended,
			/// based on the given scene and a duration
			/// that specifies the amount of time that
			/// has passed since the event was started.
			virtual bool hasEnded(const Scene& target, duration_t duration) const = 0;

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
	}
}