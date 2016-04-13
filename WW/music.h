#ifndef MUSIC_H
#define MUSIC_H

#include <string>
#include <stdexcept>
#include <map>
#include <list>

#include <SFML/Audio.hpp>

#include "config.h"

namespace GameSpace
{
	class PlayerSound final
	{
		sf::SoundBuffer m_walkSound;
		sf::SoundBuffer m_jumpSound;
		sf::SoundBuffer m_getLiveSound;
		sf::SoundBuffer m_getCoinSound;

		sf::Sound m_soundPlayer;

		void playBuffer(const sf::SoundBuffer& buffer);
	public:
		PlayerSound(const std::string& walkFile, const std::string& jumpFile,
					const std::string& getLiveFile, const std::string& getCoinFile);
		PlayerSound(const PlayerSound& other) = default;
		PlayerSound& operator=(const PlayerSound& other) = default;

		/*
			@brief озвучка шага
		*/
		void walk();
		/*
			@brief озвучка прыжка
		*/
		void jump();
		/*
		@brief озвучка подбора жизни
		*/
		void getLive();
		/*
			@brief озвучка подбора монетки
		*/
		void getCoin();

		~PlayerSound() = default;
	};

	enum class GameState;

	class GameSound
	{
		std::map<GameState, std::string> m_playList;

		sf::Music m_musicPlayer;
	public:
		void setMusic(GameState state, const std::string& music);
		void clearMusic(GameState state);
		void play(GameState state);
	};
}

#endif // !MUSIC_H
