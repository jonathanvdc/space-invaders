#pragma once

#include <functional>
#include <memory>
#include "Common.h"
#include "ITimelineEvent.h"
#include "Scene.h"

namespace si
{
	namespace timeline
	{
		/// A type of timeline event that runs finalization logic when
		/// it is ended.
		class FinalizedEvent final : public ITimelineEvent
		{
		public:
			typedef std::function<void(Scene&)> SceneAction;

			/// Creates a finalizing event from the given inner event
			/// and finalization function.
			FinalizedEvent(
				const ITimelineEvent_ptr& event,
				const SceneAction& finalize);

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
			bool isRunning;
			ITimelineEvent_ptr innerEvent;
            std::function<void(Scene&)> finalize;
		};

		/// Creates an event that performs an event, and then
		/// applies finalization logic.
		std::shared_ptr<FinalizedEvent> finalize(
			const ITimelineEvent_ptr& event,
			const FinalizedEvent::SceneAction& finalization);
	}
}
