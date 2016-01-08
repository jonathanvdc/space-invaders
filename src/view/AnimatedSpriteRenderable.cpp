#include "AnimatedSpriteRenderable.h"

#include <cmath>
#include <functional>
#include <memory>
#include <SFML/Graphics.hpp>
#include "Common.h"
#include "RenderContext.h"
#include "Transformation.h"
#include "SpriteRenderable.h"

using namespace si;
using namespace si::view;

AnimatedSpriteRenderable::AnimatedSpriteRenderable(
    const std::shared_ptr<sf::Texture>& texture, int frames,
    duration_t cycleDuration)
	: SpriteRenderableBase(texture), frames(frames),
      cycleDuration(cycleDuration), totalTime(0.0)
{ }

/// Gets the rectangular area of the texture to render.
sf::IntRect AnimatedSpriteRenderable::getTextureRectangle(duration_t timeDelta)
{
	auto textureSize = this->getTexture()->getSize();

    this->totalTime += timeDelta;

    // Computes a floating-point number that represents the number of cycles that
    // have been completed.
    double cycleTime = this->totalTime / this->cycleDuration;

    // Extract the fractional part from the relative number of completed cycles.
    // This is a number between zero and one that tells us what percentage of
    // this cycle has been completed.
    double intPart;
    double fracPart = std::modf(cycleTime, &intPart);

    // Calculate the index of the current frame.
    int frameIndex = static_cast<int>(
        std::round(fracPart * static_cast<double>(this->frames)));

    // Now we can ascertain the texture rectangle.
    int texX = static_cast<int>(textureSize.x);
    int texY = static_cast<int>(textureSize.y);

    int frameX = texX / this->frames;
    int offsetX = frameX * frameIndex;

	return{ offsetX, 0, offsetX + frameX, texY };
}
