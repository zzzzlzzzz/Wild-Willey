#include "music.h"

namespace GameSpace
{
	//////////////////////////////////////////////////////////////////////////
	PlayerSound::PlayerSound(const std::string& walkFile, const std::string& jumpFile)
	{
		if (!m_walkSound.loadFromFile(walkFile))
		{
			throw std::runtime_error("GameSpace::PlayerSound::PlayerSound if (!m_walkSound.loadFromFile(walkFile))");
		}
		if (!m_jumpSound.loadFromFile(jumpFile))
		{
			throw std::runtime_error("GameSpace::PlayerSound::PlayerSound if (!m_jumpSound.loadFromFile(jumpFile))");
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void PlayerSound::walk()
	{
		if (m_soundPlayer.getStatus() != sf::SoundSource::Playing)
		{	// подождать завершения звукового сопровождения
			m_soundPlayer.stop();
			m_soundPlayer.setBuffer(m_walkSound);
			m_soundPlayer.play();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void PlayerSound::jump()
	{
		m_soundPlayer.stop();
		m_soundPlayer.setBuffer(m_jumpSound);
		m_soundPlayer.play();
	}
	//////////////////////////////////////////////////////////////////////////
}