#include "RibbonParticleRenderable.h"

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <deque>
#include <tuple>
#include <SFML/Graphics.hpp>
#include "Common.h"
#include "IRenderable.h"

using namespace si;
using namespace si::view;
using namespace std::chrono_literals;

/// Creates a new ribbon particle renderable from the
/// given color, a duration that represents the interval 
/// at which new points are logged, and a duration
/// that represents the lifetime of a single point,
/// after it has been logged.
RibbonParticleRenderable::RibbonParticleRenderable(
	sf::Color color, duration_t pointInterval, 
	duration_t pointLifetime)
	: color(color), pointInterval(pointInterval), pointLifetime(pointLifetime),
	  elapsedTime(0.0s), totalElapsedTime(0.0s), prevPositions()
{ }

/// Renders this renderable object on the 
/// given render context, within the given
/// bounds, which is given in absolute 
/// coordinates.
void RibbonParticleRenderable::render(RenderContext& target, DoubleRect bounds)
{
	// Update elapsed time, previous position list
	this->updateTime(target.getTimeDelta());
	// Log current position.
	this->logPosition(bounds);

	if (this->prevPositions.size() > 1)
	{
		auto prevPts = getPositions(0);
		for (std::size_t i = 1; i < this->prevPositions.size(); i++)
		{
			auto curPts = getPositions(i);

			// Fade segments as they get older.
			double alpha = 1.0 - (this->totalElapsedTime - std::get<2>(this->prevPositions.at(i))) / this->pointLifetime;

			sf::ConvexShape shape;

			shape.setFillColor(sf::Color(this->color.r, this->color.g, this->color.b, sf::Uint8(this->color.a * alpha)));
			
			shape.setPointCount(4);
			shape.setPoint(0, sf::Vector2f(std::get<0>(prevPts)));
			shape.setPoint(1, sf::Vector2f(std::get<1>(prevPts)));

			shape.setPoint(2, sf::Vector2f(std::get<0>(curPts)));
			shape.setPoint(3, sf::Vector2f(std::get<1>(curPts)));

			target.getTarget().draw(shape);

			prevPts = curPts;
		}
	}
}

/// Logs the current position, given rectangular
/// bounds.
void RibbonParticleRenderable::logPosition(DoubleRect bounds)
{
	if (this->elapsedTime > this->pointInterval)
	{
		// The current position is the position in the middle
		// of the current bounding rectangle.
		auto pos = Vector2d(bounds.left + bounds.width, bounds.top + bounds.height) / 2.0;

		// Size of the ribbon is equal to the geomean of the 
		// bounds' width and height.
		double size = std::sqrt(bounds.width * bounds.height);
		// Create a new point.
		this->prevPositions.push_back(std::make_tuple(pos, size, this->totalElapsedTime));

		// Set the elapsed time to zero (but not the total elapsed time).
		this->elapsedTime = 0.0s;
	}
}

/// Updates the elapsed time, optionally removing timed
/// out previous positions.
void RibbonParticleRenderable::updateTime(duration_t delta)
{
	// Update the (total) elapsed time.
	this->totalElapsedTime += delta;
	this->elapsedTime += delta;

	auto totalTime = this->totalElapsedTime;
	auto lifetime = this->pointLifetime;

	// Remove old points.
	this->prevPositions.erase(std::remove_if(this->prevPositions.begin(), this->prevPositions.end(),
		[=](const std::tuple<Vector2d, double, duration_t>& item) -> bool 
		{ 
			return totalTime - std::get<2>(item) > lifetime;
		}), this->prevPositions.end());
}

/// Gets the polygon points for the position at
/// the given index. The deque of points
/// is assumed to be nonempty.
std::tuple<Vector2d, Vector2d> RibbonParticleRenderable::getPositions(std::size_t index)
{
	// Find the current and previous point. If the current point's
	// index is zero, then the next point is taken instead of the 
	// previous point (because there is no previous point).
	auto prevPt = this->prevPositions.at(index == 0 ? 1 : index - 1);
	auto curPt = this->prevPositions.at(index);

	// Compute the vector that represents the
	// offset between these two points.
	auto offset = std::get<0>(prevPt) - std::get<0>(curPt);

	// Compute a perpendicular vector,
	// normalize it, and multiply it
	// by the current segment's size.
	auto perpendicular = normalizeVec(Vector2d(-offset.y, offset.x)) * std::get<1>(curPt);

	return std::make_tuple(std::get<0>(curPt) + perpendicular, std::get<0>(curPt) - perpendicular);
}