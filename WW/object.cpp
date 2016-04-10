#include "object.h"

namespace GameSpace
{
	//////////////////////////////////////////////////////////////////////////
	sf::Texture& Object::getTexture()
	{
		return m_texture;
	}
	//////////////////////////////////////////////////////////////////////////
	sf::Sprite& Object::getSprite()
	{
		return m_sprite;
	}
	//////////////////////////////////////////////////////////////////////////
	Object::Object(	const sf::Image& source, int centerX, int centerY, 
					int frameX, int frameY, int frameWidth, int frameHeight)
	{
		if (!m_texture.loadFromImage(source))
		{
			throw std::runtime_error("GameSpace::Object::Object if (!m_texture.loadFromImage(source))");
		}
		m_sprite.setTexture(m_texture);
		m_sprite.setTextureRect(sf::IntRect(frameX, frameY, frameWidth, frameHeight));
		m_sprite.setOrigin(frameWidth / 2.0f, frameHeight / 2.0f);
		m_sprite.setPosition(static_cast<float>(centerX), static_cast<float>(centerY));
	}
	//////////////////////////////////////////////////////////////////////////
	Object::Object(const Object& other)
		:	m_texture(other.m_texture),
			m_sprite(other.m_sprite)
	{
		m_sprite.setTexture(m_texture);
	}
	//////////////////////////////////////////////////////////////////////////
	Object& Object::operator=(const Object& other)
	{
		if (this != &other)
		{
			m_texture = other.m_texture;
			m_sprite = other.m_sprite;
			m_sprite.setTexture(m_texture);
		}
		return *this;
	}
	//////////////////////////////////////////////////////////////////////////
	void Object::draw(sf::RenderWindow& render)
	{
		// отрисовка спрайта
		render.draw(m_sprite);
	}
	//////////////////////////////////////////////////////////////////////////
	void Object::animate(float delta)
	{
		// обработка анимации
	}
	//////////////////////////////////////////////////////////////////////////
	void Object::logic(float delta)
	{
		// обработка управления
	}
	//////////////////////////////////////////////////////////////////////////
	void Object::effect(int live, int coin)
	{
		// обработка события эффекта
	}
	//////////////////////////////////////////////////////////////////////////
	bool Object::destroy()
	{
		return false;	// по умолчанию объект не уничтожается
	}
	//////////////////////////////////////////////////////////////////////////
	b2Body*& PhysicObject::getBody()
	{
		return m_body;
	}
	//////////////////////////////////////////////////////////////////////////
	const double PhysicObject::DEGTORAD = 0.0174532925199432957;
	const double PhysicObject::RADTODEG = 57.295779513082320876;
	float PhysicObject::mapPixelToPhysic(float v)
	{
		return v / 10.f;
	}
	float PhysicObject::mapPixelToPhysic(int v)
	{
		return mapPixelToPhysic(static_cast<float>(v));
	}
	float PhysicObject::mapPhysicToPixel(float v)
	{
		return v * 10.f;
	}
	//////////////////////////////////////////////////////////////////////////

	PhysicObject::PhysicObject(const sf::Image& source, int centerX, int centerY, int frameX, int frameY, int frameWidth, int frameHeight)
		:	Object(source, centerX, centerY, frameX, frameY, frameWidth, frameHeight),
			m_body(nullptr)
	{
		m_objectCollision.m_mainObject = this;
		m_objectCollision.m_mainObjectType = Collision::PhysicObjectType::POT_UNDEF;
		m_objectCollision.m_onCollide = nullptr;
		m_objectCollision.m_onUnCollide = nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	void PhysicObject::draw(sf::RenderWindow& render)
	{
		// отрисовка в соответствии с положением в мире
		assert(m_body);
		b2Vec2 bodyPosition = m_body->GetPosition();
		float32 bodyAngle = m_body->GetAngle();
		
		getSprite().setPosition(floor(PhysicObject::mapPhysicToPixel(bodyPosition.x)),
								floor(PhysicObject::mapPhysicToPixel(bodyPosition.y)));
		getSprite().setRotation(static_cast<float>(bodyAngle * RADTODEG));
		Object::draw(render);
	}
	//////////////////////////////////////////////////////////////////////////
	SimpleObject::SimpleObject(	const sf::Image& source, int centerX, int centerY, 
								int frameX, int frameY, int frameWidth, int frameHeight)
		: Object(source, centerX, centerY, frameX, frameY, frameWidth, frameHeight)
	{

	}
	//////////////////////////////////////////////////////////////////////////
	SimpleObject::SimpleObject(const sf::Image& source, int centerX, int centerY)
		: SimpleObject(source, centerX, centerY, 0, 0, source.getSize().x, source.getSize().y)
	{

	}
	//////////////////////////////////////////////////////////////////////////
	HardObject::HardObject(const sf::Image& source, int centerX, int centerY, int frameX, int frameY, int frameWidth, int frameHeight, b2World* world, float friction)
		: PhysicObject(source, centerX, centerY, frameX, frameY, frameWidth, frameHeight)
	{
		b2BodyDef groundBodyDef;
		groundBodyDef.position.Set(	PhysicObject::mapPixelToPhysic(centerX), 
									PhysicObject::mapPixelToPhysic(centerY));

		getBody() = world->CreateBody(&groundBodyDef);

		b2EdgeShape groundBox;

		b2FixtureDef groundFixtureDef;
		groundFixtureDef.shape = &groundBox;
		groundFixtureDef.density = 0.0f;	// масса бесконечна
		groundFixtureDef.friction = friction;
		groundFixtureDef.userData = &m_objectCollision;

		const float halfWidth = PhysicObject::mapPixelToPhysic(abs(frameWidth)) / 2.0f;
		const float halfHeight = PhysicObject::mapPixelToPhysic(abs(frameHeight)) / 2.0f;

		groundBox.Set(b2Vec2(halfWidth, halfHeight), b2Vec2(-halfWidth, halfHeight));
		getBody()->CreateFixture(&groundFixtureDef);

		groundBox.Set(b2Vec2(-halfWidth, halfHeight), b2Vec2(-halfWidth, -halfHeight));
		getBody()->CreateFixture(&groundFixtureDef);

		groundBox.Set(b2Vec2(-halfWidth, -halfHeight), b2Vec2(halfWidth, -halfHeight));
		getBody()->CreateFixture(&groundFixtureDef);

		groundBox.Set(b2Vec2(halfWidth, -halfHeight), b2Vec2(halfWidth, halfHeight));
		getBody()->CreateFixture(&groundFixtureDef);

		m_objectCollision.m_mainObjectType = PhysicObject::Collision::PhysicObjectType::POT_HARD;
	}
	//////////////////////////////////////////////////////////////////////////
	HardObject::HardObject(const sf::Image& source, int centerX, int centerY, b2World* world, float friction)
		: HardObject(source, centerX, centerY, 0, 0, source.getSize().x, source.getSize().y, world, friction)
	{

	}
	//////////////////////////////////////////////////////////////////////////
	PlatformObject::PlatformObject(	const sf::Image& source, int centerX, int centerY, 
									int frameX, int frameY, int frameWidth, int frameHeight, 
									b2World* world, int dstCenterX, int dstCenterY, 
									float friction, float speed)
		:	PhysicObject(source, centerX, centerY, frameX, frameY, frameWidth, frameHeight),
			m_speed(speed)
	{
		b2BodyDef platformBody;
		platformBody.position.Set(	PhysicObject::mapPixelToPhysic(centerX),
									PhysicObject::mapPixelToPhysic(centerY));
		platformBody.type = b2_kinematicBody;
		
		getBody() = world->CreateBody(&platformBody);

		const float halfWidth = PhysicObject::mapPixelToPhysic(abs(frameWidth)) / 2.0f;
		const float halfHeight = PhysicObject::mapPixelToPhysic(abs(frameHeight)) / 2.0f;

		b2EdgeShape platformShape;
		
		b2FixtureDef platformFixture;
		platformFixture.shape = &platformShape;
		platformFixture.density = 0.0f;	// масса бесконечна
		platformFixture.friction = 0.0f;
		platformFixture.userData = &m_objectCollision;

		platformShape.Set(b2Vec2(halfWidth - 0.1f, -halfHeight), b2Vec2(-halfWidth + 0.1f, -halfHeight));
		getBody()->CreateFixture(&platformFixture)->SetFriction(friction);
		// не касаемся верхних точек платформы боковыми гранями
		// цель: трение работает только на верхней грани
		platformShape.Set(b2Vec2(-halfWidth + 0.1f, -halfHeight + 0.1f), b2Vec2(-halfWidth, halfHeight));
		getBody()->CreateFixture(&platformFixture);
		platformShape.Set(b2Vec2(-halfWidth, halfHeight), b2Vec2(halfWidth, halfHeight));
		getBody()->CreateFixture(&platformFixture);
		platformShape.Set(b2Vec2(halfWidth, halfHeight), b2Vec2(halfWidth - 0.1f, -halfHeight + 0.1f));
		getBody()->CreateFixture(&platformFixture);

		m_objectCollision.m_mainObjectType = PhysicObject::Collision::PhysicObjectType::POT_PLATFORM;

		m_srcPoint.Set(PhysicObject::mapPixelToPhysic(centerX), PhysicObject::mapPixelToPhysic(centerY));
		m_dstPoint.Set(PhysicObject::mapPixelToPhysic(dstCenterX), PhysicObject::mapPixelToPhysic(dstCenterY));
	}
	//////////////////////////////////////////////////////////////////////////
	void PlatformObject::logic(float delta)
	{
		auto bodyPosition = getBody()->GetPosition();
		const float distanceX = m_dstPoint.x - bodyPosition.x;
		const float distanceY = m_dstPoint.y - bodyPosition.y;
		float distance = sqrt(pow(distanceX, 2) + pow(distanceY, 2));
		if (distance > 1)
		{
			b2Vec2 bodyVel;
			bodyVel.x = delta * m_speed * (distanceX / distance);
			bodyVel.y = delta * m_speed * (distanceY / distance);
			getBody()->SetLinearVelocity(bodyVel);
		}
		else
		{
			std::swap(m_srcPoint.x, m_dstPoint.x);
			std::swap(m_srcPoint.y, m_dstPoint.y);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	JumpingObject::JumpingObject(const sf::Image& source, int centerX, int centerY, int frameX, int frameY, int frameWidth, int frameHeight, b2World* world, float friction, float restitution)
		: PhysicObject(source, centerX, centerY, frameX, frameY, frameWidth, frameHeight)
	{
		b2BodyDef jumpDef;
		jumpDef.position.Set(	PhysicObject::mapPixelToPhysic(centerX), 
								PhysicObject::mapPixelToPhysic(centerY));
		jumpDef.type = b2_staticBody;

		getBody() = world->CreateBody(&jumpDef);

		b2EdgeShape jumpShape;

		b2FixtureDef jumpFixture;
		jumpFixture.shape = &jumpShape;
		jumpFixture.density = 0.0f;
		jumpFixture.friction = 0.0f;
		jumpFixture.restitution = 0.0f;
		jumpFixture.userData = &m_objectCollision;

		const float halfWidth = PhysicObject::mapPixelToPhysic(frameWidth) / 2.0f;
		const float halfHeight = PhysicObject::mapPixelToPhysic(frameHeight) / 2.0f;

		jumpShape.Set(b2Vec2(halfWidth - 0.1f, -halfHeight), b2Vec2(-halfWidth + 0.1f, -halfHeight));
		b2Fixture* topFixture = getBody()->CreateFixture(&jumpFixture);
		topFixture->SetFriction(friction);
		topFixture->SetRestitution(restitution);

		jumpShape.Set(b2Vec2(-halfWidth - 0.1f, -halfHeight + 0.1f), b2Vec2(-halfWidth, halfHeight));
		getBody()->CreateFixture(&jumpFixture);
		jumpShape.Set(b2Vec2(-halfWidth, halfHeight), b2Vec2(halfWidth, halfHeight));
		getBody()->CreateFixture(&jumpFixture);
		jumpShape.Set(b2Vec2(halfWidth, halfHeight), b2Vec2(halfWidth - 0.1f, -halfHeight));
		getBody()->CreateFixture(&jumpFixture);

		m_objectCollision.m_mainObjectType = PhysicObject::Collision::PhysicObjectType::POT_JUMPER;
	}
	//////////////////////////////////////////////////////////////////////////
	EffectObject::EffectObject(	const sf::Image& source, int centerX, int centerY,
								int frameX, int frameY, int frameWidth, int frameHeight, 
								b2World* world, float friction, float restitution, 
								int liveEffect, int coinEffect, bool isDestroyable	)
		:	PhysicObject(source, centerX, centerY, frameX, frameY, frameWidth, frameHeight),
			m_liveEffect(liveEffect), m_coinEffect(coinEffect), m_isDestroyable(isDestroyable),
			m_selfDestroy(false)
	{
		b2BodyDef destroyerBody;
		destroyerBody.type = b2_staticBody;
		destroyerBody.position.Set(	PhysicObject::mapPixelToPhysic(centerX),
									PhysicObject::mapPixelToPhysic(centerY)	);

		getBody() = world->CreateBody(&destroyerBody);

		b2EdgeShape destroyerShape;

		b2FixtureDef destroyerFixture;
		destroyerFixture.shape = &destroyerShape;
		destroyerFixture.density = 0.0f;
		destroyerFixture.friction = friction;
		destroyerFixture.restitution = restitution;
		destroyerFixture.userData = &m_objectCollision;

		const float halfWidth = PhysicObject::mapPixelToPhysic(frameWidth) / 2.0f;
		const float halfHeight = PhysicObject::mapPixelToPhysic(frameHeight) / 2.0f;

		destroyerShape.Set(b2Vec2(halfWidth, -halfHeight), b2Vec2(-halfWidth, -halfHeight));
		getBody()->CreateFixture(&destroyerFixture);

		destroyerShape.Set(b2Vec2(-halfWidth, -halfHeight), b2Vec2(-halfWidth, halfHeight));
		getBody()->CreateFixture(&destroyerFixture);

		destroyerShape.Set(b2Vec2(-halfWidth, halfHeight), b2Vec2(halfWidth, halfHeight));
		getBody()->CreateFixture(&destroyerFixture);

		destroyerShape.Set(b2Vec2(halfWidth, halfHeight), b2Vec2(halfWidth, -halfHeight));
		getBody()->CreateFixture(&destroyerFixture);

		m_objectCollision.m_mainObjectType = PhysicObject::Collision::PhysicObjectType::POT_EFFECT;
		m_objectCollision.m_onCollide = std::bind(&EffectObject::onCollision, this, std::placeholders::_1);
		m_objectCollision.m_onUnCollide = std::bind(&EffectObject::onUnCollision, this, std::placeholders::_1);
	}
	//////////////////////////////////////////////////////////////////////////
	void EffectObject::onCollision(PhysicObject::Collision* other)
	{
		assert(other && other->m_mainObject);
		if (!m_selfDestroy && (m_contacts.find(other->m_mainObject) == m_contacts.end()))
		{
			other->m_mainObject->effect(m_liveEffect, m_coinEffect);
			m_contacts.insert(other->m_mainObject);

			if (m_isDestroyable)
			{
				m_selfDestroy = true;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void EffectObject::onUnCollision(PhysicObject::Collision* other)
	{
		assert(other && other->m_mainObject);
		m_contacts.erase(other->m_mainObject);
	}
	//////////////////////////////////////////////////////////////////////////
	bool EffectObject::destroy()
	{
		if (m_selfDestroy)
		{
			getBody()->GetWorld()->DestroyBody(getBody());
			getBody() = nullptr;
		}
		return m_selfDestroy;
	}
	//////////////////////////////////////////////////////////////////////////
	MoveObject::MoveObject(	const sf::Image& source, int centerX, int centerY, 
							int frameX, int frameY, int frameWidth, int frameHeight, 
							b2World* world, float density, float friction)
		: PhysicObject(source, centerX, centerY, frameX, frameY, frameWidth, frameHeight)
	{
		b2BodyDef bodyDef;
		bodyDef.type = b2_dynamicBody;
		bodyDef.position.Set(	PhysicObject::mapPixelToPhysic(centerX), 
								PhysicObject::mapPixelToPhysic(centerY));
		bodyDef.angle = 0;
		
		getBody() = world->CreateBody(&bodyDef);
		
		b2PolygonShape bodyShape;
		bodyShape.SetAsBox(	PhysicObject::mapPixelToPhysic(abs(frameWidth)) / 2.0f, 
							PhysicObject::mapPixelToPhysic(abs(frameHeight)) / 2.0f);

		b2FixtureDef bodyFixture;
		bodyFixture.shape = &bodyShape;
		bodyFixture.density = density;
		bodyFixture.friction = friction;
		bodyFixture.userData = &m_objectCollision;

		getBody()->CreateFixture(&bodyFixture);

		m_objectCollision.m_mainObjectType = PhysicObject::Collision::PhysicObjectType::POT_MOVE;
	}
	//////////////////////////////////////////////////////////////////////////
	void PlayerObject::onFootSensorCollision(PhysicObject::Collision* other)
	{
		assert(other);

		if (other->m_mainObjectType == PhysicObject::Collision::PhysicObjectType::POT_HARD || 
			other->m_mainObjectType == PhysicObject::Collision::PhysicObjectType::POT_MOVE ||
			other->m_mainObjectType == PhysicObject::Collision::PhysicObjectType::POT_PLATFORM ||
			other->m_mainObjectType == PhysicObject::Collision::PhysicObjectType::POT_JUMPER)
		{
			++m_numFootContact;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void PlayerObject::onFootSensorUnCollision(PhysicObject::Collision* other)
	{
		assert(other);

		if (other->m_mainObjectType == PhysicObject::Collision::PhysicObjectType::POT_HARD || 
			other->m_mainObjectType == PhysicObject::Collision::PhysicObjectType::POT_MOVE ||
			other->m_mainObjectType == PhysicObject::Collision::PhysicObjectType::POT_PLATFORM ||
			other->m_mainObjectType == PhysicObject::Collision::PhysicObjectType::POT_JUMPER)
		{
			--m_numFootContact;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	PlayerObject::PlayerObject(	const sf::Image& source, int centerX, int centerY,
								const Animation& animator, b2World* world,
								float density, float friction,
								float xvelocity, float yvelocity, const HUD& hud, 
								int playerLive, int maxCoin)
				: MoveObject(	source, centerX, centerY,
								animator.getFrame(true).m_x,
								animator.getFrame(true).m_y,
								animator.getFrame(true).m_width,
								animator.getFrame(true).m_height,
								world, density, friction),
				m_numFootContact(0),
				m_animator(animator), m_moveSpeed(xvelocity), m_jumpSpeed(yvelocity),
				m_hud(hud), m_currentLive(playerLive), m_totalLive(playerLive), 
				m_currentCoin(0), m_maxCoin(maxCoin)
	{
		getBody()->SetFixedRotation(true);	// предотвращаем поворот спрайта при соскальзывании

		m_objectCollision.m_mainObjectType = PhysicObject::Collision::PhysicObjectType::POT_PLAYER;

		// создание сенсора ног
		const int absFrameWidth(abs(m_animator.getFrame(true).m_width));
		const int absFrameHeight(abs(m_animator.getFrame(true).m_height));

		m_footSensorCollision.m_mainObject = this;
		m_footSensorCollision.m_mainObjectType = PhysicObject::Collision::PhysicObjectType::POT_SENSOR;
		m_footSensorCollision.m_onCollide = std::bind(&PlayerObject::onFootSensorCollision, this, std::placeholders::_1);
		m_footSensorCollision.m_onUnCollide = std::bind(&PlayerObject::onFootSensorUnCollision, this, std::placeholders::_1);

		const float sensorHalfWidth = (PhysicObject::mapPixelToPhysic(absFrameWidth) / 2.0f) - 0.1f;
		const float sensorCenterYOffset = PhysicObject::mapPixelToPhysic(absFrameHeight) / 2.0f;

		b2PolygonShape footSensorShape;
		footSensorShape.SetAsBox(sensorHalfWidth, 0.3f, b2Vec2(0, sensorCenterYOffset), 0);

		b2FixtureDef footSensorFixtureDef;
		footSensorFixtureDef.shape = &footSensorShape;
		footSensorFixtureDef.isSensor = true;
		footSensorFixtureDef.userData = &m_footSensorCollision;

		getBody()->CreateFixture(&footSensorFixtureDef);
	}
	//////////////////////////////////////////////////////////////////////////
	void PlayerObject::controlView(sf::RenderWindow& render)
	{
		auto currentPosition = getSprite().getPosition();
		auto currentView = render.getView();
		currentView.setCenter(currentPosition.x, currentPosition.y - 25.0f);
		render.setView(currentView);
	}
	//////////////////////////////////////////////////////////////////////////
	void PlayerObject::draw(sf::RenderWindow& render)
	{
		//getBody()->SetTransform(getBody()->GetPosition(), 0); // предотвращаем поворот спрайта при соскальзывании
		const auto currentFrame(m_animator.getFrame());
		getSprite().setTextureRect(sf::IntRect(currentFrame.m_x, currentFrame.m_y, currentFrame.m_width, currentFrame.m_height));
		MoveObject::draw(render);
	}
	//////////////////////////////////////////////////////////////////////////
	void PlayerObject::drawHUD(sf::RenderWindow& render)
	{
		m_hud.draw(render, m_currentLive, m_totalLive, m_currentCoin);
	}
	//////////////////////////////////////////////////////////////////////////
	bool PlayerObject::isWin()
	{
		return m_currentCoin >= m_maxCoin;
	}
	//////////////////////////////////////////////////////////////////////////
	bool PlayerObject::isFail()
	{
		return m_currentLive <= 0;
	}
	//////////////////////////////////////////////////////////////////////////
	void PlayerObject::animate(float delta)
	{
		const float reactVal = 0.1f;

		b2Vec2 bodyVelocity = getBody()->GetLinearVelocity();
		Animation::MoveState targetState = Animation::MoveState::MS_STAY_RIGHT;

		if (m_numFootContact > 0)
		{	// стоим на предмете
			if (m_desiredVelocityX > reactVal)
			{
				targetState = Animation::MoveState::MS_MOVE_RIGHT;
			}
			else if (m_desiredVelocityX < -reactVal)
			{
				targetState = Animation::MoveState::MS_MOVE_LEFT;
			}
			else
			{	// bodyVelocity.x == [-reactVal;reactVal]
				auto prevAnimState = m_animator.getState();
				if (prevAnimState == Animation::MoveState::MS_MOVE_LEFT ||
					prevAnimState == Animation::MoveState::MS_STAY_LEFT ||
					prevAnimState == Animation::MoveState::MS_FALL_LEFT ||
					prevAnimState == Animation::MoveState::MS_JUMP_LEFT)
				{
					targetState = Animation::MoveState::MS_STAY_LEFT;
				}
				else
				{
					targetState = Animation::MoveState::MS_STAY_RIGHT;
				}
			}
		}
		else
		{	// в воздухе
			if (bodyVelocity.y < -reactVal)
			{	//прыжок
				if (bodyVelocity.x > reactVal)
				{
					targetState = Animation::MoveState::MS_JUMP_RIGHT;
				}
				else if (bodyVelocity.x < -reactVal)
				{
					targetState = Animation::MoveState::MS_JUMP_LEFT;
				}
				else
				{// bodyVelocity.x == [-reactVal;reactVal]
					auto prevAnimState = m_animator.getState();
					if (prevAnimState == Animation::MoveState::MS_MOVE_LEFT ||
						prevAnimState == Animation::MoveState::MS_STAY_LEFT ||
						prevAnimState == Animation::MoveState::MS_FALL_LEFT ||
						prevAnimState == Animation::MoveState::MS_JUMP_LEFT)
					{
						targetState = Animation::MoveState::MS_JUMP_LEFT;
					}
					else
					{
						targetState = Animation::MoveState::MS_JUMP_RIGHT;
					}
				}
			}
			else
			{	// падение
				if (bodyVelocity.x > reactVal)
				{
					targetState = Animation::MoveState::MS_FALL_RIGHT;
				}
				else if (bodyVelocity.x < -reactVal)
				{
					targetState = Animation::MoveState::MS_FALL_LEFT;
				}
				else
				{// bodyVelocity.x == [-reactVal;reactVal]
					auto prevAnimState = m_animator.getState();
					if (prevAnimState == Animation::MoveState::MS_MOVE_LEFT ||
						prevAnimState == Animation::MoveState::MS_STAY_LEFT ||
						prevAnimState == Animation::MoveState::MS_FALL_LEFT ||
						prevAnimState == Animation::MoveState::MS_JUMP_LEFT)
					{
						targetState = Animation::MoveState::MS_FALL_LEFT;
					}
					else
					{
						targetState = Animation::MoveState::MS_FALL_RIGHT;
					}
				}
			}
		}

		m_animator.animate(targetState, delta);
	}
	//////////////////////////////////////////////////////////////////////////
	void PlayerObject::logic(float delta)
	{
		// обработка движения влево-вправо и прыжок
		b2Vec2 bodyVelocity = getBody()->GetLinearVelocity();
		m_desiredVelocityX = 0;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		{
			m_desiredVelocityX = -m_moveSpeed;
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		{
			m_desiredVelocityX = m_moveSpeed;
		}
		float impulsex = getBody()->GetMass() * (m_desiredVelocityX - bodyVelocity.x);

		static bool lastSignalIsJump = false;
		float impulsey = 0;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
		{
			if (!lastSignalIsJump && m_numFootContact > 0)
			{
				impulsey = -(getBody()->GetMass() * m_jumpSpeed);
				lastSignalIsJump = true;
			}
		}
		else
		{
			lastSignalIsJump = false;
		}
		// F = ma; a = v/t => F = mv/t
		getBody()->ApplyForceToCenter(b2Vec2(impulsex / delta, impulsey / delta), true);
	}
	//////////////////////////////////////////////////////////////////////////
	void PlayerObject::effect(int live, int coin)
	{
		m_currentLive += live;
		if (m_currentLive > m_totalLive)
		{
			m_currentLive = m_totalLive;
		}
		else if (m_currentLive < 0)
		{
			m_currentLive = 0;
		}
		m_currentCoin += coin;
	}
	//////////////////////////////////////////////////////////////////////////
}