#ifndef ANIMATION_H
#define ANIMATION_H

#include <map>
#include <vector>

namespace GameSpace
{
	class Animation
	{
	public:
		struct Frame
		{
			int m_x;
			int m_y;
			int m_width;
			int m_height;
			Frame(int x, int y, int width, int height) 
				: m_x(x), m_y(y), m_width(width), m_height(height)
			{}
		};
		enum class MoveState{	MS_STAY_LEFT, MS_STAY_RIGHT, 
								MS_MOVE_LEFT, MS_MOVE_RIGHT,
								MS_JUMP_LEFT, MS_JUMP_RIGHT, 
								MS_FALL_LEFT, MS_FALL_RIGHT };
	private:
		// кадр который выводится по умолчанию
		Frame m_defaultFrame;
		// текущее состояние
		MoveState m_currentState;
		// текущий кадр
		float m_currentFrame;
		// ассоциация состояний и кадров и скоростей
		// каждому состоянию соответствует пара скорость анимации и координаты анимации
		std::map<MoveState, std::pair<float, std::vector<Frame>>> m_animations;
	public:
		/*
			@brief создает аниматора
			@param defaultFrame кадр, выводимый по умолчанию
			@param initState начальное состояние объекта
		*/
		Animation(const Frame& defaultFrame, MoveState initState);
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
		void addFrame(MoveState state, const Frame& frame);
		/*
			@brief вызывается при шаге анимации
			@param state текущее состояние в котором пребывает объект
			@param delta такт времени анимации в секундах
		*/
		void animate(MoveState state, float delta);
		/*
			@param	useDefault использовать для возврата кадр по умолчанию
			@brief	возвращает кадр анимации для отрисовки
					в случае отсутствия анимации для состояния дефолтный кадр
		*/
		Frame getFrame(bool useDefault = false) const;
		/*
			@brief возвращает текущее состояние
		*/
		MoveState getState() const;
	};
}

#endif // !ANIMATION_H
