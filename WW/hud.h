#ifndef HUD_H
#define HUD_H

#include <vector>
#include <stdexcept>
#include <cassert>

#include <SFML/Graphics.hpp>

namespace GameSpace
{
	/*
		@brief отвечает за отображение пользовательского интерфейса
	*/
	class HUD final
	{
		sf::Font m_font;
		int m_fontSize;

		sf::Texture m_texture;
		
		sf::IntRect m_liveRect;
		sf::IntRect m_coinRect;
	public:
		/*
			@brief создает отображение жизней и прочую информацию о игроке
			@param source исходное изображение
			@param liveXoffset координата ’ единицы жизней на изображении
			@param liveYoffset координата ” единицы жизней на изображении
			@param liveWidth ширина единицы жизней
			@param liveHeight высота единицы жизней
			@param font путь к файлу шрифтов
			@param coinXoffset
			@param coinYoffset
			@param coinWidth ширина
			@param coinHeight высота
		*/
		HUD(const std::string& font, int fontSize, const sf::Image& source,
			const sf::IntRect& liveRect, const sf::IntRect& coinRect);

		HUD(const HUD& other) = default;
		HUD& operator=(const HUD& other) = default;
		~HUD() = default;
		/*
			@brief отрисовывает HUD
			@param render объект дл€ отрисовки
			@param curLive количество жизней
			@param totalLive всего жизней
			@param coin количество монет
		*/
		void draw(sf::RenderWindow& render, int curLive, int totalLive, int coin);
	};
}

#endif // !HUD_H
