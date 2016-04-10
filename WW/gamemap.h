#ifndef GAMEMAP_H
#define GAMEMAP_H

#include <list>
#include <map>
#include <string>
#include <algorithm>
#include <functional>
#include <stdexcept>

#include "tinyxml/tinyxml.h"

#include "object.h"

namespace GameSpace
{
	/*
		@brief	карта является файлом формата xml, генерируемого Tiled
				карта является частью уровня
				если имя слоя начинается с opaque то объекты на нем непрозрачны
				если имя слоя начинается с transparent то объекты на нем прозрачны
	*/
	class GameMap final
	{
		// список непрозрачных объектов
		std::list<Object> m_opaques;
		// список прозрачных объектов
		std::list<Object> m_transparents;
		// бонусы также указываются списком но не объектов
		// цвет фона
		sf::Color m_bgColor;

		/*
			@brief читает цвет из элемента и выбрасывает исключение в случае чего
			@param element место, откуда читать
			@param attribName имя аттрибута
			@ret   число
		*/
		sf::Color getColorAttribute(TiXmlElement* element, const char* attribName);
		/*
		@brief читает аттрибут строка из элемента и выбрасывает исключение в случае чего
		@param element место, откуда читать
		@param attribName имя атрибута
		@ret   строка
		*/
		std::string getStrAttribute(TiXmlElement* element, const char* attribName);
	public:
		GameMap() = default;
		GameMap(const GameMap&) = default;
		GameMap& operator=(const GameMap&) = default;
		~GameMap() = default;
		/*
			@brief загружает карту из xml файла
			@param mapFile имя файла с картой
		*/
		void loadFromFile(const std::string& mapFile);
		/*
			@brief отрисовывает карту
			@param render графический рендер
		*/
		void draw(sf::RenderWindow& render) const;
		/*
			@brief возвращает фоновый цвет
			@ret   значение цвета
		*/
		const sf::Color& getBackground() const;
	};
}

#endif // !GAMEMAP_H
