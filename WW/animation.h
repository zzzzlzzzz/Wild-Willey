#ifndef ANIMATION_H
#define ANIMATION_H

#include <map>
#include <vector>

#include <SFML/Graphics.hpp>

namespace GameSpace
{
	class Animation
	{
	public:
		enum class MoveState{	MS_STAY_LEFT, MS_STAY_RIGHT,	// анимация стойки
								MS_MOVE_LEFT, MS_MOVE_RIGHT,	// анимация движения
								MS_JUMP_LEFT, MS_JUMP_RIGHT,	// анимация прыжка
								MS_FALL_LEFT, MS_FALL_RIGHT		// анимация падения
							};
	private:
		// кадр который выводится по умолчанию
		sf::IntRect m_defaultFrame;
		// текущее состояние
		MoveState m_currentState;
		// текущий кадр
		float m_currentFrame;
		// ассоциация состояний и кадров и скоростей
		// каждому состоянию соответствует пара скорость анимации и координаты анимации
		std::map<MoveState, std::pair<float, std::vector<sf::IntRect>>> m_animations;
	public:
		/*
			@brief создает аниматора
			@param defaultFrame кадр, выводимый по умолчанию
			@param initState начальное состояние объекта
		*/
		Animation(const sf::IntRect& defaultFrame, MoveState initState);
		/*
			@brief задает скорость анимации для состояния
			@param state состояние
			@param animationSpeed скорость анимации
		*/
		void setStateSpeed(MoveState state, float animationSpeed);
		/*
			@brief добавляет кадр в конец определенного состояния
			@param state состояние, к которому нужно добавить кадр
			@param frame сам кадр
		*/
		void addFrame(MoveState state, const sf::IntRect& frame);
		/*
			@brief вызывается при шаге анимации
			@param state текущее состояние в котором пребывает объект
			@param delta такт времени анимации в секундах
		*/
		void animate(MoveState state, float delta);
		/*
			@brief возвращает кадр анимации для отрисовки
			@ret   в случае отсутствия анимации для состояния дефолтный кадр, иначе нужный
		*/
		sf::IntRect getFrame() const;
		/*
			@brief кадр анимации по умолчанию
		*/
		const sf::IntRect& getDefaultFrame() const;
		/*
			@brief возвращает текущее состояние
		*/
		MoveState getState() const;
	};
}

#endif // !ANIMATION_H
