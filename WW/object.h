#ifndef OBJECT_H
#define OBJECT_H

#include <stdexcept>
#include <cassert>
#include <functional>
#include <iostream>
#include <set>

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <Box2D/Box2D.h>

#include "animation.h"
#include "hud.h"
#include "music.h"

namespace GameSpace
{
	//////////////////////////////////////////////////////////////////////////
	class Object
	{
		sf::Texture m_texture;
		sf::Sprite m_sprite;
	protected:
		sf::Texture& getTexture();
		sf::Sprite& getSprite();
		/*
			@brief создает игровой объект
			@param source изображение источник
			@param centerX положение центра объекта по Х
			@param centerY положение центра объекта по У
			@param frameX смещение в изображении по Х
			@param frameY смещение в изображении по У
			@param frameWidth ширина кадра в изображении
			@param frameHeight высота кадра в изображении
		*/
		Object(	const sf::Image& source, int centerX, int centerY, 
				int frameX, int frameY, int frameWidth, int frameHeight);
		/*
			@brief копирует объект
		*/
		Object(const Object& other);
		/*
			@brief присваивает объект
		*/
		Object& operator=(const Object& other);
	public:
		/*
			@brief класс будет родительским
		*/
		virtual ~Object() = default;
		/*
			@brief отрисовывает объект
			@param render объект в котором будет отрисовка
		*/
		virtual void draw(sf::RenderWindow& render);
		/*
			@brief вызывает обработку анимации (со скоростью игры)
			@param delta время такта в секундах
		*/
		virtual void animate(float delta);
		/*
			@brief вызывает обработку логики тех объектов, у которых она есть
			@param delta время такта в секундах
		*/
		virtual void logic(float delta);
		/*
			@brief вызывается объектом создающим эффект у объектов принимающих эффект
			@param live эффект на жизнь
			@param coin эффект на монеты
		*/
		virtual void effect(int live, int coin);
		/*
			@brief	используется для уничтожения объектов, если они уничтожены в логике игры
			@ret	следует ли удалить объект из игровых объектов
		*/
		virtual bool destroy();
	};
	//////////////////////////////////////////////////////////////////////////
	class PhysicObject : public Object
	{
		b2Body* m_body;
	protected:
		b2Body*& getBody();
		static const double DEGTORAD;
		static const double RADTODEG;
		/*
			@brief преобразует пиксели в физ.единицы
		*/
		static float mapPixelToPhysic(float v);
		static float mapPixelToPhysic(int v);
		/*
			@brief преобразует физ.единицы в пиксели
		*/
		static float mapPhysicToPixel(float v);
		/*
			@brief проксирует создание объекта к базовому Object
			@param все параметры из базового класса Object
		*/
		PhysicObject(	const sf::Image& source, int centerX, int centerY,
						int frameX, int frameY, int frameWidth, int frameHeight);
		/*
			@brief почленного копирования достаточно
		*/
		PhysicObject(const PhysicObject& other) = default;
		PhysicObject& operator=(const PhysicObject& other) = default;
	public:
		/*
			@brief	структура, которая является дескриптором объекта при столкновении
					нужна только для объектов, которые имеют физику
		*/
		struct Collision
		{
			// главный объект
			PhysicObject* m_mainObject;
			// описывает тип объекта
			enum class PhysicObjectType{	POT_UNDEF, 
											POT_HARD, 
											POT_MOVE, 
											POT_PLAYER, 
											POT_SENSOR, 
											POT_PLATFORM,
											POT_JUMPER,
											POT_EFFECT} m_mainObjectType;
			// функция, вызываемая при столкновении
			// параметр - другой Collision
			std::function<void(Collision*)> m_onCollide;
			// функция, вызываемая при уходе от столкновения
			// параметр - другой Collision
			std::function<void(Collision*)> m_onUnCollide;
		};
		Collision m_objectCollision;	// Используется при столкновении с объектом
		/*
			@brief виртуальный деструктор
		*/
		virtual ~PhysicObject() = default;
		/*
			@brief отрисовка объекта в соответствии с координатами в мире
			@param render место для отрисовки
		*/
		virtual void draw(sf::RenderWindow& render) override;
	};
	//////////////////////////////////////////////////////////////////////////
	class SimpleObject : public Object
	{
	public:
		/*
			@brief создает декорацию
			@param все параметры из базового класса Object
		*/
		SimpleObject(	const sf::Image& source, int centerX, int centerY,
						int frameX, int frameY, int frameWidth, int frameHeight);
		/*
			@brief создает декорацию из целого изображения
			@param source источник изображения
			@param centerX положение центра по Х
			@param centerY положение центра по Y
		*/
		SimpleObject(const sf::Image& source, int centerX, int centerY);
		/*
			@brief этого достаточно
		*/
		SimpleObject(const SimpleObject& other) = default;
		SimpleObject& operator=(const SimpleObject& other) = default;
		virtual ~SimpleObject() = default;
	};
	//////////////////////////////////////////////////////////////////////////
	class HardObject : public PhysicObject
	{
	public:
		/*
			@brief создает "жёсткий" объект мира - не двигается и не проходим
			@param все как у родителя +
			@param world мир физики
			@param friction коэффицент трения
		*/
		HardObject(	const sf::Image& source, int centerX, int centerY,
					int frameX, int frameY, int frameWidth, int frameHeight,
					b2World* world, float friction);
		/*
			@brief создает "жёсткий" объект мира размером в спрайт
		*/
		HardObject(const sf::Image& source, int centerX, int centerY, b2World* world, float friction);

		HardObject(const HardObject& other) = default;
		HardObject& operator=(const HardObject& other) = default;
		virtual ~HardObject() = default;
	};
	//////////////////////////////////////////////////////////////////////////
	// используется для создания движущихся платформ
	class PlatformObject : public PhysicObject
	{
		b2Vec2 m_srcPoint;	// точка движения старта
		b2Vec2 m_dstPoint;	// точка движения назначения
		float m_speed;
	public:
		/*
			@brief создает платформу, перемещающуюся
					из (centerX, centerY) в (dstCenterX, dstCenterY)
			@param все как у родителя +
			@param dstCenterX координата назначения Х
			@param dstCenterY координата назначения У
			@param friction коэффицент трения верхней части панели
			@param speed скорость движения платформы
		*/
		PlatformObject(	const sf::Image& source, int centerX, int centerY,
						int frameX, int frameY, int frameWidth, int frameHeight,
						b2World* world, int dstCenterX, int dstCenterY,
						float friction, float speed);

		PlatformObject(const PlatformObject&) = default;
		PlatformObject& operator=(const PlatformObject&) = default;
		virtual ~PlatformObject() = default;

		/*
			@brief логика платформы
		*/
		void logic(float delta) override;
	};
	//////////////////////////////////////////////////////////////////////////
	// используется для создания прыжковых ускорителей
	class JumpingObject : public PhysicObject
	{
	public:
		/*
			@brief создает объект, на котором можно прыгать
			@param все как у родителя +
			@param friction коэффицент трения верхней кромки
			@param restitution коэффицент упругости
		*/
		JumpingObject(const sf::Image& source, int centerX, int centerY,
			int frameX, int frameY, int frameWidth, int frameHeight,
			b2World* world, float friction, float restitution);

		JumpingObject(const JumpingObject&) = default;
		JumpingObject& operator=(const JumpingObject&) = default;
		virtual ~JumpingObject() = default;
	};
	//////////////////////////////////////////////////////////////////////////
	// используется как объект, касание к которому приводит к эффекту
	class EffectObject : public PhysicObject
	{
		int m_liveEffect;	// эффект, причиняемый жизни
		int m_coinEffect;	// эффект, причиняемый монетам
		bool m_isDestroyable;	// уничтожить после эффекта
		bool m_selfDestroy;	// объект должен уничтожится
		std::set<PhysicObject*> m_contacts;	// отслеживает контакты, чтобы причинять эффект только новым
		/*
			@brief вызывается после контакта с объектом
			@param other объект, с которым произошел контакт
		*/
		void onCollision(PhysicObject::Collision* other);
		/*
			@brief вызывается после прекращения контакта с объектом
			@param other объект, с которым отменен контакт
		*/
		void onUnCollision(PhysicObject::Collision* other);
	public:
		/*
			@brief создает объект который убивает
			@param все как у родителя +
			@param friction коэффицент трения
			@param restitution коэффицент упругости (отталкивать после атаки)
			@param liveEffect эффект, причиняемый жизни при касании
			@param coinEffect эффект, причиняемый монетам при касании
			@param isDestroyable уничтожать объект после касания с ним
		*/
		EffectObject(	const sf::Image& source, int centerX, int centerY,
						int frameX, int frameY, int frameWidth, int frameHeight,
						b2World* world, float friction, float restitution, 
						int liveEffect, int coinEffect, bool isDestroyable	);

		EffectObject(const EffectObject&) = default;
		EffectObject& operator=(const EffectObject&) = default;
		virtual ~EffectObject() = default;

		virtual bool destroy() override;
	};
	//////////////////////////////////////////////////////////////////////////
	// используется для игрока и других сущностей, которые умеют двигаться
	class MoveObject : public PhysicObject
	{
	public:
		/*
			@brief создает объект мира, способный к перемещению
			@param все как у родителя +
			@param density плотность (от этого зависит масса)
			@param friction коэффицент трения
		*/
		MoveObject(	const sf::Image& source, int centerX, int centerY,
					int frameX, int frameY, int frameWidth, int frameHeight,
					b2World* world, float density, float friction);

		MoveObject(const MoveObject& other) = default;
		MoveObject& operator=(const MoveObject& other) = default;
		virtual ~MoveObject() = default;
	};
	//////////////////////////////////////////////////////////////////////////
	class PlayerObject : public MoveObject
	{
		int m_numFootContact; // количество контактов с почвой
		/*
			@brief вызывается при контакте с объектом нижней гранью
			@param other объект, с которым осуществлен контакт
		*/
		void onFootSensorCollision(PhysicObject::Collision* other);
		/*
			@brief вызывается при отмене контакта с объектом нижней гранью
			@param other объект, с которым прекращен контакт
		*/
		void onFootSensorUnCollision(PhysicObject::Collision* other);

		Animation m_animator;	// объект для анимации
		float m_moveSpeed, m_jumpSpeed;	// скорость движения и прыжка
		float m_desiredVelocityX;	// скорость, которую хочется достигнуть игроку
		HUD m_hud;	// инфо-панель
		PlayerSound m_sound;	// звуковые эффекты
		int m_currentLive, m_totalLive;	// текущее количество жизней и всего жизней
		int m_currentCoin, m_maxCoin;	// текущее количество монет и количество монет для выйгрыша
	public:
		/*
			@brief создает игровой объект
			@param source исходное изображение
			@param centerX координата Х центра
			@param centerY координата У центра
			@param animator объект-аниматор
			@param world физический мир
			@param density плотность
			@param friction трение
			@param xvelocity скорость по Х
			@param yvelocity скорость по У
			@param hud инфо-панель
			@param playerLive количество жизней у игрока
			@param maxcoin количество монет, приводящих к выйгрышу
		*/
		PlayerObject(	const sf::Image& source, int centerX, int centerY,
						const Animation& animator, b2World* world,
						float density, float friction,
						float xvelocity, float yvelocity,
						const HUD& hud, const PlayerSound& sound, 
						int playerLive, int maxCoin);

		PlayerObject(const PlayerObject& other) = default;
		PlayerObject& operator=(const PlayerObject& other) = default;
		virtual ~PlayerObject() = default;

		/*
			@brief хелпер для сенсора ног
		*/
		PhysicObject::Collision m_footSensorCollision;

		/*
			@brief выполняет перемещение камеры
			@param render объект для рендера
		*/
		void controlView(sf::RenderWindow& render);
		/*
			@brief выполняет прорисовку
		*/
		virtual void draw(sf::RenderWindow& render) override;
		/*
			@brief выполняет анимацию в зависимости от скорости
			@param delta время такта в секундах
		*/
		virtual void animate(float delta) override;
		/*
			@brief инициирует управление игровым объектом
			@param delta время такта в секундах
		*/
		virtual void logic(float delta) override;
		/*
			@brief вызывается при причинении эффекта
			@param live эффект к жизни
			@param coin эффект к монетам
		*/
		virtual void effect(int live, int coin) override;
		/*
			@brief выполняет прорисовку HUD информации
			@param render объект для отрисовки
		*/
		void drawHUD(sf::RenderWindow& render);
		/*
			@brief проверяет условие выйгрыша
			@ret   выйгран ли уровень
		*/
		bool isWin();
		/*
			@brief проверяет условие проигрыша
			@ret   проигран ли уровень
		*/
		bool isFail();
	};
	//////////////////////////////////////////////////////////////////////////
}

#endif // !OBJECT_H
