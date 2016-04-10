#ifndef CORE_H
#define CORE_H

#include <string>
#include <iostream>
#include <memory>
#include <map>
#include <cassert>

#include <SFML/Graphics.hpp>

#include "config.h"
#include "state.h"
#include "world.h"

namespace GameSpace
{
	class GameCore final
	{
		// @brief оптимальный FPS для игры
		static const int m_optimalFPS;

		// @brief главный оконный рендер
		sf::RenderWindow m_mainRender;

		// @brief главный вид
		sf::View m_mainView;

		// @brief описывает текущее состояние, в котором пребывает ядро
		State::GameState m_currentState;
		// @brief реестр состояний
		std::map<State::GameState, std::shared_ptr<State>> m_states;

		// @brief описывает конфигурационный файл
		Config* m_mainConfig;

		// @brief игровой мир
		World m_gameWorld;

		GameCore(const GameCore&) = delete;
		GameCore& operator=(const GameCore&) = delete;
	public:
		/*
			@brief создание ядра игры
			@param mainConfig конфигурация для экранов
		*/
		GameCore(Config* mainConfig);
		// @brief выполняет запуск игрового цикла
		void runGameLoop();
		~GameCore() = default;
	};
}

#endif // !CORE_H