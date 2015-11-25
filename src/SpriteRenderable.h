#pragma once
#include <functional>
#include <memory>
#include <SFML/Graphics.hpp>
#include "Common.h"
#include "RenderContext.h"
#include "IRenderable.h"

namespace si
{
	namespace view 
	{
		/// Defines a renderable sprite object.
		class SpriteRenderable final : public IRenderable
		{
		public:
			/// Creates a renderable sprite from the given texture.
			SpriteRenderable(const std::shared_ptr<sf::Texture>& texture);

			/// Renders this sprite.
			void render(RenderContext& target, DoubleRect bounds) const final override;
		private:
			std::shared_ptr<sf::Texture> texture;
		};
	}
}