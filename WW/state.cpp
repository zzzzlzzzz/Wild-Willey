#include "state.h"

namespace GameSpace
{
	//////////////////////////////////////////////////////////////////////////
	void State::eventProcessing()
	{
		sf::Event gameEvent;
		while (getRender()->pollEvent(gameEvent))
		{
			stateEventProcessing(gameEvent);
			if (gameEvent.type == sf::Event::Closed)
			{
				getRender()->close();
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void State::drawing()
	{
		getRender()->clear(sf::Color(255, 255, 255, 0));
		stateDrawing();
		getRender()->display();
	}
	//////////////////////////////////////////////////////////////////////////
	GameState State::update()
	{
		eventProcessing();
		drawing();
		return m_nextState;
	}
	//////////////////////////////////////////////////////////////////////////
	State::State(sf::RenderWindow* render, GameSound* soundPlayer)
		: m_render(render), m_soundPlayer(soundPlayer)
	{
		assert(m_render && m_soundPlayer);
	}
	//////////////////////////////////////////////////////////////////////////
	void State::setNextState(GameState state)
	{
		m_nextState = state;
	}
	//////////////////////////////////////////////////////////////////////////
	sf::RenderWindow* State::getRender()
	{
		return m_render;
	}
	//////////////////////////////////////////////////////////////////////////
	GameSound* State::getSoundPlayer()
	{
		return m_soundPlayer;
	}
	//////////////////////////////////////////////////////////////////////////
	// игра
	const float StateGame::m_lowFilter = 0.4f;

	StateGame::StateGame(sf::RenderWindow* render, GameSound* soundPlayer, Config* mainConfig, World* gameWorld)
		: State(render, soundPlayer), m_mainConfig(mainConfig), m_gameWorld(gameWorld), m_fdt(0.0f)
	{
		assert(m_mainConfig && m_gameWorld);
	}

	void StateGame::statePrepare()
	{
		setNextState(GameState::GS_GAME);
		getSoundPlayer()->play(GameState::GS_GAME);

		m_fdt = 0.0f;
		m_gameClock.restart();
	}

	GameState StateGame::update()
	{
		m_fdt = m_gameClock.restart().asSeconds() * m_lowFilter + m_fdt * (1 - m_lowFilter);
		m_gameWorld->runGameTiming(m_fdt);

		if (m_gameWorld->isFail())
		{
			setNextState(GameState::GS_GAMEFAIL);
		}
		else if (m_gameWorld->isWin())
		{
			m_mainConfig->markLevelAsCompleted(m_gameWorld->getLastLevel());
			setNextState(GameState::GS_GAMEWIN);
		}

		return State::update();
	}

	void StateGame::stateEventProcessing(sf::Event& event)
	{
		if (event.type == sf::Event::KeyPressed)
		{
			if (event.key.code == sf::Keyboard::Escape)
			{
				setNextState(GameState::GS_LEVEL);
			}
		}
	}

	void StateGame::stateDrawing()
	{
		m_gameWorld->runDrawing(*getRender());
	}
	//////////////////////////////////////////////////////////////////////////
	// проигрыш
	StateFail::StateFail(sf::RenderWindow* render, GameSound* soundPlayer, Config* mainConfig)
		: State(render, soundPlayer)
	{
		assert(mainConfig);

		getSoundPlayer()->setMusic(GameState::GS_GAMEFAIL, mainConfig->getFailConfig().m_sound);

		if (!m_bgTexture.loadFromFile(mainConfig->getFailConfig().m_backgroundImage))
		{
			throw std::runtime_error("GameSpace::StateFail::StateFail if (!m_bgTexture.loadFromFile(...))");
		}
		if (!m_infoTexture.loadFromFile(mainConfig->getFailConfig().m_image))
		{
			throw std::runtime_error("GameSpace::StateFail::StateFail if (!m_infoTexture.loadFromFile(...))");
		}

		m_background.setTexture(m_bgTexture, true);
		
		m_info.setTexture(m_infoTexture);
		m_info.setOrigin(m_infoTexture.getSize().x / 2.0f, m_infoTexture.getSize().y / 2.0f);
	}

	void StateFail::statePrepare()
	{
		setNextState(GameState::GS_GAMEFAIL);
		getSoundPlayer()->play(GameState::GS_GAMEFAIL);
	}

	void StateFail::stateEventProcessing(sf::Event& event)
	{
		if (event.type == sf::Event::KeyPressed)
		{
			if (event.key.code == sf::Keyboard::Escape)
			{
				setNextState(GameState::GS_LEVEL);
			}
		}
	}
	
	void StateFail::stateDrawing()
	{
		const auto viewDim = getRender()->getView().getSize();
		const auto bgDim = m_background.getLocalBounds();
		const auto topLeftPos = getRender()->mapPixelToCoords(sf::Vector2i(0, 0));
		m_background.setPosition(topLeftPos);
		m_background.setScale(viewDim.x / bgDim.width, viewDim.y / bgDim.height);
		getRender()->draw(m_background);

		m_info.setPosition(getRender()->getView().getCenter());
		getRender()->draw(m_info);
	}
	//////////////////////////////////////////////////////////////////////////
	// выйгрыш

	StateWin::StateWin(sf::RenderWindow* render, GameSound* soundPlayer, Config* mainConfig)
		: State(render, soundPlayer)
	{
		assert(mainConfig);

		getSoundPlayer()->setMusic(GameState::GS_GAMEWIN, mainConfig->getWinConfig().m_sound);

		if (!m_bgTexture.loadFromFile(mainConfig->getWinConfig().m_backgroundImage))
		{
			throw std::runtime_error("GameSpace::StateWin::StateWin if (!m_bgTexture.loadFromFile(...))");
		}
		if (!m_infoTexture.loadFromFile(mainConfig->getWinConfig().m_image))
		{
			throw std::runtime_error("GameSpace::StateWin::StateWin if (!m_infoTexture.loadFromFile(...))");
		}

		m_background.setTexture(m_bgTexture);

		m_info.setTexture(m_infoTexture);
		m_info.setOrigin(m_infoTexture.getSize().x / 2.0f, m_infoTexture.getSize().y / 2.0f);
	}

	void StateWin::statePrepare()
	{
		setNextState(GameState::GS_GAMEWIN);
		getSoundPlayer()->play(GameState::GS_GAMEWIN);
	}

	void StateWin::stateEventProcessing(sf::Event& event)
	{
		if (event.type == sf::Event::KeyPressed)
		{
			if (event.key.code == sf::Keyboard::Escape)
			{
				setNextState(GameState::GS_LEVEL);
			}
		}
	}

	void StateWin::stateDrawing()
	{
		const auto viewDim = getRender()->getView().getSize();
		const auto bgDim = m_background.getLocalBounds();
		const auto topLeftPos = getRender()->mapPixelToCoords(sf::Vector2i(0, 0));
		m_background.setPosition(topLeftPos);
		m_background.setScale(viewDim.x / bgDim.width, viewDim.y / bgDim.height);
		getRender()->draw(m_background);

		m_info.setPosition(getRender()->getView().getCenter());
		getRender()->draw(m_info);
	}
	//////////////////////////////////////////////////////////////////////////
	// меню
	StateMenu::StateMenu(sf::RenderWindow* render, GameSound* soundPlayer, Config* mainConfig)
		: State(render, soundPlayer)
	{
		assert(mainConfig);

		getSoundPlayer()->setMusic(GameState::GS_MENU, mainConfig->getMenuConfig().m_sound);

		if (!m_bgTexture.loadFromFile(mainConfig->getMenuConfig().m_backgroundImage))
		{
			throw std::runtime_error("GameSpace::StateMenu::StateMenu if (!m_bgTexture.loadFromFile(...))");
		}
		if (!m_startTexture.loadFromFile(mainConfig->getMenuConfig().m_startImage))
		{
			throw std::runtime_error("GameSpace::StateMenu::StateMenu if (!m_startTexture.loadFromFile(...))");
		}
		if (!m_exitTexture.loadFromFile(mainConfig->getMenuConfig().m_exitImage))
		{
			throw std::runtime_error("GameSpace::StateMenu::StateMenu if (!m_exitTexture.loadFromFile(...))");
		}

		m_background.setTexture(m_bgTexture, true);
		
		m_start.setTexture(m_startTexture);
		m_start.setOrigin(m_startTexture.getSize().x / 2.0f, 0);

		m_exit.setTexture(m_exitTexture);
		m_exit.setOrigin(m_exitTexture.getSize().x / 2.0f, 0);
	}

	void StateMenu::statePrepare()
	{
		setNextState(GameState::GS_MENU);
		getSoundPlayer()->play(GameState::GS_MENU);
	}
	
	void StateMenu::stateEventProcessing(sf::Event& event)
	{
		if (event.type == sf::Event::MouseButtonPressed &&
			event.mouseButton.button == sf::Mouse::Button::Left)
		{
			const auto coord = getRender()->mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
			if (m_start.getGlobalBounds().contains(coord))
			{
				setNextState(GameState::GS_LEVEL);
			}
			else if (m_exit.getGlobalBounds().contains(coord))
			{
				setNextState(GameState::GS_EXIT);
			}
		}
	}
	void StateMenu::stateDrawing()
	{
		const auto viewDim = getRender()->getView().getSize();
		const auto bgDim = m_background.getLocalBounds();
		const auto topLeftPos = getRender()->mapPixelToCoords(sf::Vector2i(0, 0));
		m_background.setPosition(topLeftPos);
		m_background.setScale(viewDim.x / bgDim.width, viewDim.y / bgDim.height);
		getRender()->draw(m_background);

		const float ypart = viewDim.y / 4.0f;	// 1 пустой спрайт + 2 значимых + 1 пустой
		sf::Vector2f centerPos(topLeftPos.x + (viewDim.x / 2.0f), topLeftPos.y + ypart);

		auto drawSprite = [&](sf::Sprite& m_sprite)
		{
			m_sprite.setPosition(centerPos);
			getRender()->draw(m_sprite);
			centerPos.y += ypart;
		};

		drawSprite(m_start);
		drawSprite(m_exit);
	}
	//////////////////////////////////////////////////////////////////////////
	// выбор уровня
	StateLevel::LevelsImage::LevelsImage(const std::string& image)
	{
		if (!m_texture.loadFromFile(image))
		{
			throw std::runtime_error("GameSpace::StateLevel::LevelsImage::LevelsImage if (!m_texture.loadFromFile(image))");
		}
		m_sprite.setTexture(m_texture);
	}
	StateLevel::LevelsImage::LevelsImage(const LevelsImage& other)
		: m_texture(other.m_texture), m_sprite(other.m_sprite)
	{
		m_sprite.setTexture(m_texture);
	}
	StateLevel::LevelsImage& StateLevel::LevelsImage::operator =(const LevelsImage& other)
	{
		if (this != &other)
		{
			m_texture = other.m_texture;
			m_sprite = other.m_sprite;
			m_sprite.setTexture(m_texture);
		}
		return *this;
	}

	StateLevel::StateLevel(sf::RenderWindow* render, GameSound* soundPlayer, Config* mainConfig, World* gameWorld)
		: State(render, soundPlayer), m_mainConfig(mainConfig), m_gameWorld(gameWorld)
	{
		assert(m_mainConfig && m_gameWorld);

		getSoundPlayer()->setMusic(GameState::GS_LEVEL, mainConfig->getLevelsConfig().m_sound);

		if (!m_bgTexture.loadFromFile(m_mainConfig->getLevelsConfig().m_backgroundImage))
		{
			throw std::runtime_error("GameSpace::StateLevel::StateLevel if (!m_bgTexture.loadFromFile(...))");
		}

		m_background.setTexture(m_bgTexture, true);

		if (!m_loadingTexture.loadFromFile(m_mainConfig->getLevelsConfig().m_loadImage))
		{
			throw std::runtime_error("GameSpace::StateLevel::StateLevel if (!m_loadingTexture.loadFromFile(...))");
		}
		m_loading.setTexture(m_loadingTexture);
		m_loading.setOrigin(m_loadingTexture.getSize().x / 2.0f, m_loadingTexture.getSize().y / 2.0f);

		for (const auto& itLevel : m_mainConfig->getLevelsConfig().m_levels)
		{
			m_levels.emplace_back(itLevel.m_image);
		}
	}

	void StateLevel::statePrepare()
	{
		setNextState(GameState::GS_LEVEL);
		getSoundPlayer()->play(GameState::GS_LEVEL);
	}

	void StateLevel::stateEventProcessing(sf::Event& event)
	{
		if (event.type == sf::Event::MouseButtonPressed &&
			event.mouseButton.button == sf::Mouse::Button::Left)
		{
			const auto coord = getRender()->mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
			for (size_t i = 0; i < m_levels.size(); ++i)
			{
				if (m_levels[i].m_sprite.getGlobalBounds().contains(coord) &&
					(i == 0 || m_mainConfig->getLevelsConfig().m_levels.at(i - 1).m_isCompleted))
				{
					m_loading.setPosition(getRender()->getView().getCenter());
					getRender()->draw(m_loading);
					getRender()->display();

					getSoundPlayer()->setMusic(GameState::GS_GAME, m_mainConfig->getLevelsConfig().m_levels.at(i).m_levelSound);
					m_gameWorld->loadFromFile(m_mainConfig->getLevelsConfig().m_levels.at(i).m_levelFile);
					setNextState(GameState::GS_GAME);

					break;
				}
			}
		}
		else if (event.type == sf::Event::KeyPressed)
		{
			if (event.key.code == sf::Keyboard::Escape)
			{
				setNextState(GameState::GS_MENU);
			}
		}
	}

	void StateLevel::stateDrawing()
	{
		const auto viewDim = getRender()->getView().getSize();
		const auto bgDim = m_background.getLocalBounds();
		const auto topleftPos = getRender()->mapPixelToCoords(sf::Vector2i(0, 0));
		m_background.setPosition(topleftPos);
		m_background.setScale(viewDim.x / bgDim.width, viewDim.y / bgDim.height);
		getRender()->draw(m_background);

		const int blockPerLine = m_mainConfig->getLevelsConfig().m_blockPerLine;
		const float xpart = viewDim.x / (m_mainConfig->getLevelsConfig().m_blockPerLine + 2.0f);	// 1 боковой + n значимых + 1 боковой
		const float ypart = viewDim.y / (m_mainConfig->getLevelsConfig().m_linePerScreen + 2.0f);	// 1 верхний + k значимых + 1 нижний

		for (size_t i = 0; i < m_levels.size(); ++i)
		{
			m_levels[i].m_sprite.setColor(sf::Color::White);
			m_levels[i].m_sprite.setPosition(	topleftPos.x + xpart + (i % blockPerLine)*xpart,
												topleftPos.y + ypart + (i / blockPerLine)*ypart);
			if (i > 0 && !m_mainConfig->getLevelsConfig().m_levels.at(i - 1).m_isCompleted)
			{
				m_levels[i].m_sprite.setColor(sf::Color::Blue);
			}
			getRender()->draw(m_levels[i].m_sprite);
		}
	}
	//////////////////////////////////////////////////////////////////////////
}