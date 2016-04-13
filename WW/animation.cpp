#include "animation.h"

namespace GameSpace
{
	//////////////////////////////////////////////////////////////////////////
	Animation::Animation(const sf::IntRect& defaultFrame, MoveState initState)
		:	m_defaultFrame(defaultFrame), 
			m_currentState(initState), 
			m_currentFrame(0.0f)
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::setStateSpeed(MoveState state, float animationSpeed)
	{
		m_animations[state].first = animationSpeed;
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::addFrame(MoveState state, const sf::IntRect& frame)
	{
		m_animations[state].second.push_back(frame);
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::animate(MoveState state, float delta)
	{
		if (m_currentState == state)
		{
			m_currentFrame += delta * m_animations[state].first;
			if (m_currentFrame >= m_animations[m_currentState].second.size())
			{
				m_currentFrame = 0.0f;
			}
		}
		else
		{
			m_currentState = state;
			m_currentFrame = 0.0f;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	sf::IntRect Animation::getFrame() const
	{
		if (m_animations.at(m_currentState).second.empty())
		{
			return getDefaultFrame();
		}
		return m_animations.at(m_currentState).second.at(static_cast<size_t>(m_currentFrame));
	}
	//////////////////////////////////////////////////////////////////////////
	const sf::IntRect& Animation::getDefaultFrame() const
	{
		return m_defaultFrame;
	}
	//////////////////////////////////////////////////////////////////////////
	Animation::MoveState Animation::getState() const
	{
		return m_currentState;
	}
	//////////////////////////////////////////////////////////////////////////
}