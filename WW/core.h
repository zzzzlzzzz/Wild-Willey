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
		// @brief ����������� FPS ��� ����
		static const int m_optimalFPS;

		// @brief ������� ������� ������
		sf::RenderWindow m_mainRender;

		// @brief ������� ���
		sf::View m_mainView;

		// @brief ��������� ������� ���������, � ������� ��������� ����
		State::GameState m_currentState;
		// @brief ������ ���������
		std::map<State::GameState, std::shared_ptr<State>> m_states;

		// @brief ��������� ���������������� ����
		Config* m_mainConfig;

		// @brief ������� ���
		World m_gameWorld;

		GameCore(const GameCore&) = delete;
		GameCore& operator=(const GameCore&) = delete;
	public:
		/*
			@brief �������� ���� ����
			@param mainConfig ������������ ��� �������
		*/
		GameCore(Config* mainConfig);
		// @brief ��������� ������ �������� �����
		void runGameLoop();
		~GameCore() = default;
	};
}

#endif // !CORE_H