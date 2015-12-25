#include "SoundEvent.h"

#include <memory>
#include <SFML/Audio/Sound.hpp>
#include "Common.h"
#include "ITimelineEvent.h"
#include "Scene.h"

using namespace si;
using namespace si::timeline;

/// Creates a sound event from the given buffer.
SoundEvent::SoundEvent(const std::shared_ptr<sf::SoundBuffer>& buffer)
	: buffer(buffer), sound(*buffer)
{ }

/// Starts the timeline event.
void SoundEvent::start(Scene&)
{
	this->sound.play();
}

/// Has this timeline event update the given scene.
bool SoundEvent::update(Scene&, duration_t)
{
	return this->sound.getStatus() == sf::SoundSource::Playing;
}

/// Applies this timeline event's finalization
/// logic to the given scene.
void SoundEvent::end(Scene&)
{
	this->sound.stop();
}
