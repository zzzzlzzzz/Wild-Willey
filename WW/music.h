#ifndef MUSIC_H
#define MUSIC_H

#include <string>
#include <stdexcept>

#include <SFML/Audio.hpp>

namespace GameSpace
{
	class PlayerSound final
	{
		sf::SoundBuffer m_walkSound;
		sf::SoundBuffer m_jumpSound;

		sf::Sound m_soundPlayer;
	public:
		PlayerSound(const std::string& walkFile, const std::string& jumpFile);
		PlayerSound(const PlayerSound& other) = default;
		PlayerSound& operator=(const PlayerSound& other) = default;

		void walk();
		void jump();

		~PlayerSound() = default;
	};
}

#endif // !MUSIC_H
