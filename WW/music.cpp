#include "music.h"

namespace GameSpace
{
	//////////////////////////////////////////////////////////////////////////
	PlayerSound::PlayerSound(	const std::string& walkFile, const std::string& jumpFile,
								const std::string& getLiveFile, const std::string& getCoinFile	)
	{
		if (!m_walkSound.loadFromFile(walkFile))
		{
			throw std::runtime_error("GameSpace::PlayerSound::PlayerSound if (!m_walkSound.loadFromFile(walkFile))");
		}
		if (!m_jumpSound.loadFromFile(jumpFile))
		{
			throw std::runtime_error("GameSpace::PlayerSound::PlayerSound if (!m_jumpSound.loadFromFile(jumpFile))");
		}
		if (!m_getLiveSound.loadFromFile(getLiveFile))
		{
			throw std::runtime_error("GameSpace::PlayerSound::PlayerSound if (!m_getLiveSound.loadFromFile(getLiveFile))");
		}
		if (!m_getCoinSound.loadFromFile(getCoinFile))
		{
			throw std::runtime_error("GameSpace::PlayerSound::PlayerSound if (!m_getCoinSound.loadFromFile(getCoinFile))");
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void PlayerSound::playBuffer(const sf::SoundBuffer& buffer)
	{
		m_soundPlayer.stop();
		m_soundPlayer.setBuffer(buffer);
		m_soundPlayer.play();
	}
	//////////////////////////////////////////////////////////////////////////
	void PlayerSound::walk()
	{
		if (m_soundPlayer.getStatus() != sf::SoundSource::Playing)
		{	// подождать завершения звукового сопровождения
			playBuffer(m_walkSound);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void PlayerSound::jump()
	{
		playBuffer(m_jumpSound);
	}
	//////////////////////////////////////////////////////////////////////////
	void PlayerSound::getLive()
	{
		playBuffer(m_getLiveSound);
	}
	//////////////////////////////////////////////////////////////////////////
	void PlayerSound::getCoin()
	{
		playBuffer(m_getCoinSound);
	}
	//////////////////////////////////////////////////////////////////////////
	void GameSound::setMusic(GameState state, const std::string& music)
	{
		m_playList[state] = music;
	}
	//////////////////////////////////////////////////////////////////////////
	void GameSound::clearMusic(GameState state)
	{
		const auto itMusic = m_playList.find(state);
		if (itMusic != m_playList.end())
		{
			m_playList.erase(itMusic);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void GameSound::play(GameState state)
	{
		m_musicPlayer.stop();
		const auto itMusic = m_playList.find(state);
		if (itMusic != m_playList.end())
		{
			if (!m_musicPlayer.openFromFile(itMusic->second))
			{
				throw std::runtime_error("GameSpace::GameSound::play if (!m_musicPlayer.openFromFile(itMusic->second))");
			}
			m_musicPlayer.setLoop(true);
			m_musicPlayer.play();
		}
	}
	//////////////////////////////////////////////////////////////////////////
}