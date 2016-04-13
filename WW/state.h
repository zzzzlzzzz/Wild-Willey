#ifndef STATE_H
#define STATE_H

#include <vector>
#include <iostream>
#include <cassert>

#include <SFML/Graphics.hpp>

#include "config.h"
#include "world.h"
#include "music.h"

namespace GameSpace
{
	//////////////////////////////////////////////////////////////////////////
	enum class GameState
	{
		GS_GAME,		// игровой процесс
		GS_GAMEFAIL,	// проигрыш
		GS_GAMEWIN,		// выигрыш
		GS_MENU,		// меню
		GS_LEVEL,		// выбор уровня
		GS_EXIT			// выход
	};
	//////////////////////////////////////////////////////////////////////////
	// состояние, которое будет обрабатываться в момент времени
	class State
	{
	public:
		/*
			@brief обновление состояния
			@ret   новое состояние
		*/
		virtual GameState update();

		/*
			@brief выполняет подготовку состояния
					вызывается после переключения на это состояние
		*/
		virtual void statePrepare() = 0;

		State(sf::RenderWindow* render, GameSound* soundPlayer);
		virtual ~State() = default;
	protected:
		/*
			@brief задает следующее состояние
			@param state следующее состояние
		*/
		void setNextState(GameState state);
		/*
			@brief возвращает указатель на объект для отображения
			@ret   указатель на объект для отображения
		*/
		sf::RenderWindow* getRender();
		/*
			@brief возвращает указатель на проигрыватель
		*/
		GameSound* getSoundPlayer();
	private:
		State(const State&) = delete;
		State& operator=(const State&) = delete;
		// состояние, которое будет отдано
		GameState m_nextState;
		// объект для отображения
		sf::RenderWindow* m_render;
		// объект для проигрывания музыки в каждом состоянии
		GameSound* m_soundPlayer;
		/*
			@brief обрабатывает события состояния
			@param event событие
		*/
		virtual void stateEventProcessing(sf::Event& event) = 0;
		/*
			@brief обрабатывает прорисовку
		*/
		virtual void stateDrawing() = 0;
		/*
			@brief выполняет обработку асинхронных событий
					вызывает stateEventProcessing для каждого
					затем обрабатывает дополнительно событие завершения
		*/
		void eventProcessing();
		/*
			@brief выполняет операцию прорисовки
		*/
		void drawing();
	};
	//////////////////////////////////////////////////////////////////////////
	class StateGame : public State
	{
		static const float m_lowFilter;

		Config* m_mainConfig;
		World* m_gameWorld;

		float m_fdt;
		sf::Clock m_gameClock;

		virtual void stateEventProcessing(sf::Event& event) override;
		virtual void stateDrawing() override;
	public:
		StateGame(sf::RenderWindow* render, GameSound* soundPlayer, Config* mainConfig, World* gameWorld);
		virtual GameState update() override;
		virtual void statePrepare() override;
		virtual ~StateGame() = default;
	};
	//////////////////////////////////////////////////////////////////////////
	class StateFail : public State
	{
		sf::Texture m_bgTexture;
		sf::Texture m_infoTexture;

		sf::Sprite m_background;
		sf::Sprite m_info;

		virtual void stateEventProcessing(sf::Event& event) override;
		virtual void stateDrawing() override;
	public:
		StateFail(sf::RenderWindow* render, GameSound* soundPlayer, Config* mainConfig);
		virtual void statePrepare() override;
		virtual ~StateFail() = default;
	};
	//////////////////////////////////////////////////////////////////////////
	class StateWin : public State
	{
		sf::Texture m_bgTexture;
		sf::Texture m_infoTexture;

		sf::Sprite m_background;
		sf::Sprite m_info;

		virtual void stateEventProcessing(sf::Event& event) override;
		virtual void stateDrawing() override;
	public:
		StateWin(sf::RenderWindow* render, GameSound* soundPlayer, Config* mainConfig);
		virtual void statePrepare() override;
		virtual ~StateWin() = default;
	};
	//////////////////////////////////////////////////////////////////////////
	class StateMenu : public State
	{
		sf::Texture m_bgTexture;
		sf::Texture m_startTexture;
		sf::Texture m_exitTexture;

		sf::Sprite m_background;
		sf::Sprite m_start;
		sf::Sprite m_exit;

		virtual void stateEventProcessing(sf::Event& event) override;
		virtual void stateDrawing() override;
	public:
		StateMenu(sf::RenderWindow* render, GameSound* soundPlayer, Config* mainConfig);
		virtual void statePrepare() override;
		virtual ~StateMenu() = default;
	};
	//////////////////////////////////////////////////////////////////////////
	class StateLevel : public State
	{
		Config* m_mainConfig;
		World* m_gameWorld;

		sf::Texture m_bgTexture;
		sf::Texture m_loadingTexture;

		sf::Sprite m_background;
		sf::Sprite m_loading;

		struct LevelsImage
		{
			sf::Texture m_texture;
			sf::Sprite m_sprite;
			LevelsImage(const std::string& image);
			LevelsImage(const LevelsImage& other);
			LevelsImage& operator=(const LevelsImage& other);
		};
		std::vector<LevelsImage> m_levels;	// текстуры и спрайты кэшируются

		virtual void stateEventProcessing(sf::Event& event) override;
		virtual void stateDrawing() override;
	public:
		StateLevel(sf::RenderWindow* render, GameSound* soundPlayer, Config* mainConfig, World* gameWorld);
		virtual void statePrepare() override;
		virtual ~StateLevel() = default;
	};
	//////////////////////////////////////////////////////////////////////////
}

#endif // !STATE_H
