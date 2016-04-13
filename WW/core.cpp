#include "core.h"

namespace GameSpace
{
	//////////////////////////////////////////////////////////////////////////
	const int GameCore::m_optimalFPS = 60;
	//////////////////////////////////////////////////////////////////////////
	GameCore::GameCore(Config* mainConfig)
		:	m_mainRender(sf::VideoMode(mainConfig->getScreenConfig().m_width, mainConfig->getScreenConfig().m_height), 
				mainConfig->getScreenConfig().m_caption,
				mainConfig->getScreenConfig().m_isFullScreen ? sf::Style::Fullscreen : sf::Style::Default),
			m_mainView(sf::FloatRect(0.f, 0.f, 
									static_cast<float>(mainConfig->getScreenConfig().m_width), 
									static_cast<float>(mainConfig->getScreenConfig().m_height))),
			m_currentState(GameState::GS_MENU),
			m_mainConfig(mainConfig)
	{
		m_mainRender.setView(m_mainView);
		m_mainRender.setFramerateLimit(m_optimalFPS);

		m_states[GameState::GS_MENU] = std::make_shared<StateMenu>(&m_mainRender, &m_musicPlayer, m_mainConfig);
		m_states[GameState::GS_LEVEL] = std::make_shared<StateLevel>(&m_mainRender, &m_musicPlayer, m_mainConfig, &m_gameWorld);
		m_states[GameState::GS_GAME] = std::make_shared<StateGame>(&m_mainRender, &m_musicPlayer, m_mainConfig, &m_gameWorld);
		m_states[GameState::GS_GAMEFAIL] = std::make_shared<StateFail>(&m_mainRender, &m_musicPlayer, m_mainConfig);
		m_states[GameState::GS_GAMEWIN] = std::make_shared<StateWin>(&m_mainRender, &m_musicPlayer, m_mainConfig);
	}
	//////////////////////////////////////////////////////////////////////////
	void GameCore::runGameLoop()
	{
		assert(m_states.find(m_currentState) != m_states.end());
		auto prevState = m_currentState;
		m_states[m_currentState]->statePrepare();

		while (	m_mainRender.isOpen() && 
				m_currentState != GameState::GS_EXIT)
		{
			assert(m_states.find(m_currentState) != m_states.end());
			if (m_currentState != prevState)
			{
				m_states[m_currentState]->statePrepare();
				prevState = m_currentState;
			}
			m_currentState = m_states[m_currentState]->update();
		}
	}
	//////////////////////////////////////////////////////////////////////////
}