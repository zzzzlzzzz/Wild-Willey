#include "hud.h"

namespace GameSpace
{
	//////////////////////////////////////////////////////////////////////////
	HUD::HUD(	const std::string& font, int fontSize, const sf::Image& source,
				const sf::IntRect& liveRect, const sf::IntRect& coinRect)
		:	m_fontSize(fontSize), m_liveRect(liveRect), m_coinRect(coinRect)
	{
		if (!m_font.loadFromFile(font))
		{
			throw std::runtime_error("GameSpace::HUD::HUD if (!m_font.loadFromFile(font))");
		}
		if (!m_texture.loadFromImage(source))
		{
			throw std::runtime_error("GameSpace::HUD::HUD if (!m_texture.loadFromImage(source))");
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void HUD::draw(sf::RenderWindow& render, int curLive, int totalLive, int coin)
	{
		assert(curLive <= totalLive);

		const int leftBorder = 0;
		const int rightBorder = static_cast<int>(render.getView().getSize().x);
		const int topBorder = 0;
		const sf::Color decorColor(sf::Color::Black);

		sf::Sprite liveSprite(m_texture, m_liveRect);
		for (int i = 0; i < totalLive; ++i)
		{
			liveSprite.setPosition(
				render.mapPixelToCoords(
					sf::Vector2i(leftBorder + i * m_liveRect.width, topBorder)
				)
			);
			if (i == curLive)
			{
				liveSprite.setColor(decorColor);
			}
			render.draw(liveSprite);
		}

		sf::Sprite coinSprite(m_texture, m_coinRect);
		sf::Text coinCountText(std::to_string(coin) + " x ", m_font, m_fontSize);
		coinCountText.setColor(decorColor);

		coinSprite.setPosition(
				render.mapPixelToCoords(
					sf::Vector2i(rightBorder - static_cast<int>(coinSprite.getLocalBounds().width), topBorder)
				)
			);
		render.draw(coinSprite);

		coinCountText.setPosition(
			render.mapPixelToCoords(
					sf::Vector2i(	rightBorder - 
									static_cast<int>(coinSprite.getLocalBounds().width) - 
									static_cast<int>(coinCountText.getLocalBounds().width), topBorder)
				)
			);
		render.draw(coinCountText);
	}
	//////////////////////////////////////////////////////////////////////////
}