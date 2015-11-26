#pragma once
#include <string>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Font.hpp>
#include "IRenderable.h"
#include "RenderContext.h"

namespace si
{
	namespace view
	{
		/// Defines a renderable text object.
		/// The text it contains is scaled automatically
		/// to fit its bounding box. The string of text
		/// is always written on a single line, and is
		/// never word-wrapped.
		class TextRenderable final : public IRenderable
		{
		public:
			/// Creates a renderable text object from
			/// the given string and color.
			TextRenderable(const std::string& text, const sf::Font& font, sf::Color textColor);

			/// Renders this text object.
			void render(RenderContext& target, DoubleRect bounds) final override;

			/// Gets the text that this 
			/// object renders.
			std::string getText() const;

			/// Sets this renderable text object's text.
			void setText(const std::string& text);

			/// Gets the font that this 
			/// object renders text in.
			sf::Font getFont() const;

			/// Sets this renderable text object's 
			/// font.
			void setFont(sf::Font font);

			/// Gets the color that this 
			/// object renders text in.
			sf::Color getColor() const;

			/// Sets this renderable text object's 
			/// text color.
			void setColor(sf::Color color);
		private:
			std::string text;
			sf::Color textColor;
			sf::Font font;
		};
	}
}