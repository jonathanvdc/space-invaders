#pragma once

#include <vector>
#include "Common.h"
#include "model/Entity.h"
#include "model/Game.h"
#include "model/DriftingEntity.h"
#include "view/IRenderable.h"
#include "controller/IController.h"
#include "parser/ParsedEntity.h"
#include "ITimelineEvent.h"
#include "Scene.h"
#include "ConcurrentEvent.h"
#include "SpawnEvent.h"

namespace si
{
	namespace timeline
	{
		/// A data structure that contains information
		/// that pertains to invader ship behavior.
		struct InvaderBehavior
		{
			/// The velocity of the invader ship's path
			/// (which need not always coincide with the
			/// invader ship's actual velocity). Note that this
			/// velocity is relative to the target
			/// direction.
			Vector2d velocity;

			/// The spring constant that the invader ship's
			/// controller uses to trace its path.
			double springConstant;

			/// The rate at which invader ships are allowed
			/// to launch projectiles.
			duration_t fireInterval;

			/// A maximal deviation from the standard fire
			/// interval for this type of invader.
			/// If this is set to a nonzero value, then
			/// some invaders will fire projectiles at
			/// a higher pace than others.
			duration_t fireIntervalDeviation;

			/// A vector that specifies where this batch of invaders will spawn.
			Vector2d spawnPosition;

			/// A vector that specifies the direction invaders will move in.
			Vector2d targetDirection;
		};

		/// Defines a type of event that spawns a wave of invaders.
		/// The event ends when all invaders have been eliminated.
		class InvaderWaveEvent final : public ITimelineEvent
		{
		public:
			/// Creates an invader wave event from the given ship factory,
			/// projectile factory, row count and column count.
			/// A velocity and spring constant are also provided. They
			/// determine which path invaders follow.
			InvaderWaveEvent(
				const si::parser::ParsedShipFactory& shipFactory,
				const si::parser::ParsedDriftingEntityFactory& projectileFactory,
				int rowCount, int columnCount, const InvaderBehavior& invaderBehavior);

			InvaderWaveEvent(const InvaderWaveEvent&) = delete;

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

			/// Checks if this event is still running.
			bool isRunning() const;
		private:
			const si::parser::ParsedShipFactory shipFactory;
			const si::parser::ParsedDriftingEntityFactory projectileFactory;
			const int rowCount;
			const int columnCount;
			const InvaderBehavior invaderBehavior;

			/// Stores the wave event's ships as a vector of vectors of
			/// timeline event pointers. Essentially, every nested vector contains
			/// the life of all ships in a specific column - identified by the vector's
			/// index in the topmost vector -, which can then be used to determine
			/// which invaders are free to open fire: once an event has been ended,
			/// its value in this vector is set to null.
			std::vector<std::vector<ITimelineEvent_ptr>> shipEvents;
		};
	}
}
