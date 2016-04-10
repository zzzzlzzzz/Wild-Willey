#ifndef WORLD_H
#define WORLD_H

#include <memory>
#include <list>
#include <string>
#include <algorithm>
#include <vector>
#include <map>
#include <functional>
#include <iostream>

#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include <tinyxml.h>

#include "object.h"
#include "animation.h"
#include "hud.h"

namespace GameSpace
{
	//////////////////////////////////////////////////////////////////////////
	// Класс - диспетчер столкновений
	//////////////////////////////////////////////////////////////////////////
	class ContactDispatcher : public b2ContactListener
	{
		void BeginContact(b2Contact* contact) override;
		void EndContact(b2Contact* contact) override;
	};
	//////////////////////////////////////////////////////////////////////////
	// Класс игрового мира
	//////////////////////////////////////////////////////////////////////////
	class World final
	{
		ContactDispatcher m_contactDispatcher;

		std::unique_ptr<b2World> m_physWorld;

		std::list<std::shared_ptr<Object>> m_gameObjects;

		std::string m_lastLevel;

		World(const World& other) = delete;
		World& operator=(const World& other) = delete;

		/*
			@brief читает свойство из XML элемента
			@param element XML элемент
			@param name название свойство
			@ret   свойство
		*/
		std::string readValFromXML(TiXmlElement* element, const std::string& name);
		/*
			@brief создает анимацию из XML элемента
			@param rootElement корневой XML элемент
			@ret   анимация
		*/
		Animation readAnimationFromXML(TiXmlElement* rootElement);
		/*
			@brief читает HUD из XML элемента
			@param rootElement коренной элемент, описывающий HUD
			@ret   объект HUD
		*/
		HUD readHUDFromXML(TiXmlElement* rootElement);
		/*
			@brief выполняет загрузку игрока и добавление его в мир
			@param playerCfg файл, дескриптор игрока
			@param leftX левый край
			@param bottomY нижний край
		*/
		void loadPlayerFromFile(const std::string& playerCfg, int leftX, int bottomY);
		/*
			@brief	функция для контроля вида
					вызывается перед рисованием
		*/
		std::function<void(sf::RenderWindow&)> m_controlView;
		/*
			@brief функция для отрисовки информационных панелей пользователя
		*/
		std::function<void(sf::RenderWindow&)> m_drawHUD;
		/*
			@brief проверка игрока на выйгрыш
		*/
		std::function<bool()> m_playerWin;
		/*
			@brief проверка игрока на проигрыш
		*/
		std::function<bool()> m_playerFail;
		/*
			@brief цвет заднего фона
		*/
		sf::Color m_mapBgColor;
		/*
			@brief параметр модификации времени
		*/
		float m_timeWarp;
		/*
			@brief выполняет чтение цвета
			@param element элемент в котором есть свойство
			@param name имя свойства
			@ret   цвет
		*/
		sf::Color readColorFromXML(TiXmlElement* element, const std::string& name);
		/*
			@brief выполняет чтение свойств
			@param element элемент со свойствами
			@param data результирующий ассоциативный массив
		*/
		void readPropFromXML(TiXmlElement* element, std::map<std::string, std::string>& data);
		/*
			@brief выполняет загрузку карты из tmx файла
			@param mapCfg файл-карта
		*/
		void loadMapFromFile(const std::string& mapCfg);
	public:
		World() = default;
		~World() = default;
		/*
			@brief выполняет загрузку уровня
			@param level файл с уровнем
		*/
		void loadFromFile(const std::string& level);
		/*
			@brief выполняет проработку игрового времени
			@param delta длительность игрового времени в секундах
		*/
		void runGameTiming(float delta);
		/*
			@brief выполняет прорисовку объектов
		*/
		void runDrawing(sf::RenderWindow& render);
		/*
			@brief проверка на выйгрышь
			@ret   выйгран ли уровень
		*/
		bool isWin();
		/*
			@brief проверка на проигрышь
			@ret   проигран ли уровень
		*/
		bool isFail();
		/*
			@brief возвращает путь, по которому был загружен уровень
			@ret   путь к уровню
		*/
		const std::string& getLastLevel() const;
	};
	//////////////////////////////////////////////////////////////////////////
}

#endif // !WORLD_H