#include "MusicEvent.h"

#include <memory>
#include <SFML/Audio/Music.hpp>
#include "Common.h"
#include "ITimelineEvent.h"
#include "Scene.h"

using namespace si;
using namespace si::timeline;

/// Creates a music event from the given buffer.
MusicEvent::MusicEvent(const std::shared_ptr<sf::Music>& music)
	: music(music)
{ }

/// Starts the timeline event.
void MusicEvent::start(Scene&)
{
	this->music->play();
}

/// Has this timeline event update the given scene.
bool MusicEvent::update(Scene&, duration_t)
{
	return this->music->getStatus() == sf::SoundSource::Playing;
}

/// Applies this timeline event's finalization
/// logic to the given scene.
void MusicEvent::end(Scene&)
{
	this->music->stop();
}
