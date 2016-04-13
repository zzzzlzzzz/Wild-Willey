#include "world.h"

namespace GameSpace
{
	//////////////////////////////////////////////////////////////////////////
	void ContactDispatcher::BeginContact(b2Contact* contact)
	{
		assert(contact);

		auto bodyAUserData = static_cast<PhysicObject::Collision*>(contact->GetFixtureA()->GetUserData());
		auto bodyBUserData = static_cast<PhysicObject::Collision*>(contact->GetFixtureB()->GetUserData());
		if (bodyAUserData && bodyBUserData)
		{
			if (bodyAUserData->m_onCollide)
			{
				bodyAUserData->m_onCollide(bodyBUserData);
			}
			if (bodyBUserData->m_onCollide)
			{
				bodyBUserData->m_onCollide(bodyAUserData);
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ContactDispatcher::EndContact(b2Contact* contact)
	{
		assert(contact);

		auto bodyAUserData = static_cast<PhysicObject::Collision*>(contact->GetFixtureA()->GetUserData());
		auto bodyBUserData = static_cast<PhysicObject::Collision*>(contact->GetFixtureB()->GetUserData());

		if (bodyAUserData && bodyBUserData)
		{
			if (bodyAUserData->m_onUnCollide)
			{
				bodyAUserData->m_onUnCollide(bodyBUserData);
			}
			if (bodyBUserData->m_onUnCollide)
			{
				bodyBUserData->m_onUnCollide(bodyAUserData);
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	std::string World::readValFromXML(TiXmlElement* element, const std::string& name)
	{
		const char* value = element->Attribute(name.c_str());
		if (!value)
		{
			throw std::runtime_error("GameSpace::World::readValFromXML if (!value)");
		}
		return std::string(value);
	}
	//////////////////////////////////////////////////////////////////////////
	Animation World::readAnimationFromXML(TiXmlElement* rootElement)
	{
		using namespace std;

		auto getStateFromStr = [](const string& src) -> Animation::MoveState
		{
			if (src == "stay_right")
			{
				return Animation::MoveState::MS_STAY_RIGHT;
			}
			else if (src == "stay_left")
			{
				return Animation::MoveState::MS_STAY_LEFT;
			}
			else if (src == "move_right")
			{
				return Animation::MoveState::MS_MOVE_RIGHT;
			}
			else if (src == "move_left")
			{
				return Animation::MoveState::MS_MOVE_LEFT;
			}
			else if (src == "jump_right")
			{
				return Animation::MoveState::MS_JUMP_RIGHT;
			}
			else if (src == "jump_left")
			{
				return Animation::MoveState::MS_JUMP_LEFT;
			}
			else if (src == "fall_right")
			{
				return Animation::MoveState::MS_FALL_RIGHT;
			}
			else if (src == "fall_left")
			{
				return Animation::MoveState::MS_FALL_LEFT;
			}
			throw std::runtime_error("GameSpace::World::readAnimationFromXML getStateFromStr");
		};

		const int defxpos = stoi(readValFromXML(rootElement, "defimgx"));
		const int defypos = stoi(readValFromXML(rootElement, "defimgy"));
		const int defwidth = stoi(readValFromXML(rootElement, "defimgwidth"));
		const int defheight = stoi(readValFromXML(rootElement, "defimgheight"));
		const Animation::MoveState defstate(getStateFromStr(readValFromXML(rootElement, "initstate")));

		Animation animator(sf::IntRect(defxpos, defxpos, defwidth, defheight), defstate);

		for (TiXmlElement* state = rootElement->FirstChildElement(); state; state = state->NextSiblingElement())
		{
			const Animation::MoveState mstate(getStateFromStr(state->Value()));
			animator.setStateSpeed(mstate, stof(readValFromXML(state, "speed")));
			for (TiXmlElement* image = state->FirstChildElement(); image; image = image->NextSiblingElement())
			{
				const int x = stoi(readValFromXML(image, "x"));
				const int y = stoi(readValFromXML(image, "y"));
				const int width = stoi(readValFromXML(image, "width"));
				const int height = stoi(readValFromXML(image, "height"));
				animator.addFrame(mstate, sf::IntRect(x, y, width, height));
			}
		}

		return animator;
	}
	//////////////////////////////////////////////////////////////////////////
	HUD World::readHUDFromXML(TiXmlElement* rootElement)
	{
		using namespace std;
		sf::Image hudImage;
		if (!hudImage.loadFromFile(readValFromXML(rootElement, "file")))
		{
			throw runtime_error("GameSpace::World::readHUDFromXML if (!hudImage.loadFromFile(...))");
		}

		const std::string fontFile = readValFromXML(rootElement, "font");
		const int fontSize = stoi(readValFromXML(rootElement, "fontsize"));

		auto readImgRect = [this](TiXmlElement* el, sf::IntRect& result)
		{
			result.left = stoi(readValFromXML(el, "srcX"));
			result.top = stoi(readValFromXML(el, "srcY"));
			result.width = stoi(readValFromXML(el, "srcWidth"));
			result.height = stoi(readValFromXML(el, "srcHeight"));
		};

		TiXmlElement* live = rootElement->FirstChildElement("live");
		if (!live)
		{
			throw runtime_error("GameSpace::World::readHUDFromXML if (!live)");
		}
		sf::IntRect liveRect;
		readImgRect(live, liveRect);

		TiXmlElement* coin = rootElement->FirstChildElement("coin");
		if (!coin)
		{
			throw std::runtime_error("GameSpace::World::readHUDFromXML if (!coin)");
		}
		sf::IntRect coinRect;
		readImgRect(coin, coinRect);

		return HUD(fontFile, fontSize, hudImage, liveRect, coinRect);
	}
	//////////////////////////////////////////////////////////////////////////
	void World::loadPlayerFromFile(const std::string& playerCfg, int leftX, int bottomY)
	{
		using namespace std;
		assert(m_physWorld);

		TiXmlDocument playerFile(playerCfg.c_str());
		if (!playerFile.LoadFile())
		{
			throw runtime_error("GameSpace::World::loadPlayerFromFile if (!playerFile.LoadFile())");
		}
		TiXmlElement* rootElement = playerFile.FirstChildElement("player");
		if (!rootElement)
		{
			throw runtime_error("GameSpace::World::loadPlayerFromFile if (!rootElement)");
		}

		const float density(stof(readValFromXML(rootElement, "density")));
		const float friction(stof(readValFromXML(rootElement, "friction")));
		const float xvelocity(stof(readValFromXML(rootElement, "xvelocity")));
		const float yvelocity(stof(readValFromXML(rootElement, "yvelocity")));
		const int lives(stoi(readValFromXML(rootElement, "lives")));
		const int winCoins(stoi(readValFromXML(rootElement, "wincoin")));
		const string file(readValFromXML(rootElement, "file"));
		
		TiXmlElement* animation = rootElement->FirstChildElement("animation");
		if (!animation)
		{
			throw runtime_error("GameSpace::World::loadPlayerFromFile if (!animation)");
		}
		const Animation animator(readAnimationFromXML(animation));

		TiXmlElement* hud = rootElement->FirstChildElement("hud");
		if (!hud)
		{
			throw runtime_error("GameSpace::World::loadPlayerFromFile if (!hud)");
		}
		const HUD playerHud(readHUDFromXML(hud));

		TiXmlElement* sound = rootElement->FirstChildElement("sound");;
		if (!sound)
		{
			throw runtime_error("GameSpace::World::loadPlayerFromFile if (!sound)");
		}
		const PlayerSound playerSound(	readValFromXML(sound, "step"), 
										readValFromXML(sound, "jump"),
										readValFromXML(sound, "getLive"),
										readValFromXML(sound, "getCoin"));

		sf::Image playerImage;
		if (!playerImage.loadFromFile(file))
		{
			throw std::runtime_error("GameSpace::World::loadPlayerFromFile if (!playerImage.loadFromFile(file))");
		}
		const int playerCenterX(static_cast<int>(leftX + animator.getDefaultFrame().width / 2.0));
		const int playerCenterY(static_cast<int>(bottomY - animator.getDefaultFrame().height / 2.0));
		auto player = make_shared<PlayerObject>(playerImage, playerCenterX, playerCenterY,
												animator, m_physWorld.get(), density, friction, 
												xvelocity, yvelocity, playerHud, playerSound, 
												lives, winCoins);
		m_gameObjects.push_back(player);

		m_controlView = std::bind(&PlayerObject::controlView, player, std::placeholders::_1);
		m_drawHUD = std::bind(&PlayerObject::drawHUD, player, std::placeholders::_1);
		m_playerWin = std::bind(&PlayerObject::isWin, player);
		m_playerFail = std::bind(&PlayerObject::isFail, player);
	}
	//////////////////////////////////////////////////////////////////////////
	sf::Color World::readColorFromXML(TiXmlElement* element, const std::string& name)
	{
		using namespace std;
		if (const char* colorPtr = element->Attribute(name.c_str()))
		{
			const string color(colorPtr);
			int a = 255, r = 0, g = 0, b = 0;
			string::size_type colorOffset = 1;
			if (color.length() == 9)
			{
				a = stoi(color.substr(colorOffset, 2), 0, 16); colorOffset += 2;
			}
			r = stoi(color.substr(colorOffset, 2), 0, 16); colorOffset += 2;
			g = stoi(color.substr(colorOffset, 2), 0, 16); colorOffset += 2;
			b = stoi(color.substr(colorOffset, 2), 0, 16);
			return sf::Color(r, g, b, a);
		}
		return sf::Color(255, 255, 255, 255);
	}
	//////////////////////////////////////////////////////////////////////////
	void World::readPropFromXML(TiXmlElement* element, std::map<std::string, std::string>& data)
	{
		while (element)
		{
			data[readValFromXML(element, "name")] = readValFromXML(element, "value");
			element = element->NextSiblingElement();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void World::loadMapFromFile(const std::string& mapCfg)
	{
		/*
			описание формата
			карта в формате tmx
			у карты есть параметры width, height, tilewidth, tileheight
			backgroundcolor задает цвет фона (поддерживается прозрачность)
				+ в свойствах карты
				gravityx гравитация по X (float)
				gravityy гравитация по Y (float)
				timewarp варп (модификация времени) (float)
				friction коэффицент трения для земли (float)

			тайлсеты задаются отдельными изображениями

				+ в свойствах слоя
				visible = [transparent, opaque]
				transparent - прозрачный слой
				opaque - жёсткий слой

			объекты на карте поверх карты (все объекты из тайлов)
			у каждого объекта на карте есть тип
			в зависимости от типа отличаются дополнительные свойства
				тип player
					+ config - задает расположение файла конфигурации игрока
				тип movable (перемещаемые объекты)
					+ density - плотность
					+ friction - трение
				тип platform (самодвигающиеся платформы)
					+ dstx - значение назначения Х (левая граница)
					+ dsty - значение назначения У (нижняя граница)
					+ friction - трение верхней кромки
					+ speed - скорость
				тип jumping (батут для прыжков)
					+ friction - трение верхней кромки платформы
					+ restitution - упругость верхней кромки платформы
				тип effect (объект-эффект)
					+ friction - трение объекта
					+ restitution - упругость объекта
					+ live - бонус к жизни при касании
					+ coin - бонус к монетам при касании
					+ destroy - если "1", то уничтожать после касания, если "0", то нет
		*/
		using namespace std;

		TiXmlDocument mapFile(mapCfg.c_str());
		if (!mapFile.LoadFile())
		{
			throw runtime_error("GameSpace::World::loadMapFromFile if (!mapFile.LoadFile())");
		}
		TiXmlElement* rootElement = mapFile.FirstChildElement("map");

		const int xTileDim(stoi(readValFromXML(rootElement, "width")));
		const int yTileDim(stoi(readValFromXML(rootElement, "height")));
		const int tileWidth(stoi(readValFromXML(rootElement, "tilewidth")));
		const int tileHeight(stoi(readValFromXML(rootElement, "tileheight")));
		m_mapBgColor = readColorFromXML(rootElement, "backgroundcolor");
		
		map<string, string> mapProperties;
		readPropFromXML(rootElement->FirstChildElement("properties")->FirstChildElement(), mapProperties);
		m_timeWarp = stof(mapProperties["timewarp"]);
		m_physWorld.reset(new b2World(
							b2Vec2(	stof(mapProperties["gravityx"]), 
									stof(mapProperties["gravityy"])	)
										));
		m_physWorld->SetContactListener(&m_contactDispatcher);

		map<int, sf::Image> tiles;
		vector<TiXmlElement*> transparents, opaques, objects;

		for (	TiXmlElement* mapItem = rootElement->FirstChildElement();
				mapItem;
				mapItem = mapItem->NextSiblingElement())
		{
			if (strcmp(mapItem->Value(), "tileset") == 0)
			{	// секция описывает набор тайлов
				const int firstGid(stoi(readValFromXML(mapItem, "firstgid")));
				for (	TiXmlElement* tileItem = mapItem->FirstChildElement();
						tileItem;
						tileItem = tileItem->NextSiblingElement())
				{
					const int tileId(firstGid + stoi(readValFromXML(tileItem, "id")));
					TiXmlElement* imageItem = tileItem->FirstChildElement("image");
					if (!imageItem)
					{
						throw runtime_error("GameSpace::World::loadMapFromFile if (!imageItem)");
					}
					if (!tiles[tileId].loadFromFile(readValFromXML(imageItem, "source")))
					{
						throw runtime_error("GameSpace::World::loadMapFromFile if (!tiles[tileId].loadFromFile(...))");
					}
				}
			}
			else if (strcmp(mapItem->Value(), "layer") == 0)
			{	// секция описывает слой
				map<string, string> layerProperties;
				readPropFromXML(mapItem->FirstChildElement("properties")->FirstChildElement(), layerProperties);
				if (layerProperties["visible"] == "transparent")
				{
					transparents.push_back(mapItem);
				}
				else if (layerProperties["visible"] == "opaque")
				{
					opaques.push_back(mapItem);
				}
			}
			else if (strcmp(mapItem->Value(), "objectgroup") == 0)
			{	// секция описывает объекты
				objects.push_back(mapItem);
			}
		}

		auto processLayer = [&](vector<TiXmlElement*>& layer,
								function<shared_ptr<Object>(const sf::Image&, int, int)> make)
		{
			for (const auto& item : layer)
			{
				int currentTileNum = 0;
				for (TiXmlElement* dataItem = item->FirstChildElement("data")->FirstChildElement();
					dataItem;
					dataItem = dataItem->NextSiblingElement())
				{
					auto curTile = tiles.find(stoi(readValFromXML(dataItem, "gid")));
					if (curTile != tiles.end())
					{
						const auto curTileSize = curTile->second.getSize();
						const int xpos = static_cast<int>((currentTileNum % xTileDim) * tileWidth + curTileSize.x / 2.0);
						const int ypos = static_cast<int>((currentTileNum / xTileDim) * tileHeight + curTileSize.y / 2.0);
						m_gameObjects.push_back(make(curTile->second, xpos, ypos));
					}
					++currentTileNum;
				}
			}
		};

		processLayer(transparents, [](const sf::Image& source, int x, int y)
		{
			return make_shared<SimpleObject>(source, x, y);
		});

		processLayer(opaques, [&](const sf::Image& source, int x, int y)
		{
			return make_shared<HardObject>(source, x, y, m_physWorld.get(), stof(mapProperties["friction"]));
		});

		for (const auto& item : objects)
		{
			for (	TiXmlElement* objectItem = item->FirstChildElement();
					objectItem;
					objectItem = objectItem->NextSiblingElement())
			{
				const string objectType(readValFromXML(objectItem, "type"));
				const int gid(stoi(readValFromXML(objectItem, "gid")));
				const int leftX(stoi(readValFromXML(objectItem, "x")));
				const int bottomY(stoi(readValFromXML(objectItem, "y")));

				map<string, string> properties;
				readPropFromXML(objectItem->FirstChildElement("properties")->FirstChildElement(), properties);

				if (objectType == "player")
				{
					loadPlayerFromFile(properties["config"], leftX, bottomY);
				}
				else if (objectType == "movable")
				{
					const auto tileSize = tiles[gid].getSize();
					const int centerX = static_cast<int>(leftX + tileSize.x / 2.0);
					const int centerY = static_cast<int>(bottomY - tileSize.y / 2.0);
					m_gameObjects.push_back(make_shared<MoveObject>(tiles[gid],
												centerX, centerY,
												0, 0, tileSize.x, tileSize.y,
												m_physWorld.get(),
												stof(properties["density"]),
												stof(properties["friction"]))
											);
				}
				else if (objectType == "platform")
				{
					const auto tileSize = tiles[gid].getSize();
					const int centerX = static_cast<int>(leftX + tileSize.x / 2.0);
					const int centerY = static_cast<int>(bottomY - tileSize.y / 2.0);
					const int dstCenterX = static_cast<int>(stof(properties["dstx"]) + tileSize.x / 2.0);
					const int dstCenterY = static_cast<int>(stof(properties["dsty"]) - tileSize.y / 2.0);
					m_gameObjects.push_back(make_shared<PlatformObject>(tiles[gid],
																		centerX, centerY,
																		0, 0, tileSize.x, tileSize.y,
																		m_physWorld.get(),	
																		dstCenterX, dstCenterY, 
																		stof(properties["friction"]), 
																		stof(properties["speed"]))
											);
				}
				else if (objectType == "jumping")
				{
					const auto tileSize = tiles[gid].getSize();
					const int centerX = static_cast<int>(leftX + tileSize.x / 2.0);
					const int centerY = static_cast<int>(bottomY - tileSize.y / 2.0);
					m_gameObjects.push_back(make_shared<JumpingObject>(	tiles[gid],
																		centerX, centerY,
																		0, 0, tileSize.x, tileSize.y,
																		m_physWorld.get(),
																		stof(properties["friction"]),
																		stof(properties["restitution"]))
											);
				}
				else if (objectType == "effect")
				{
					const auto tileSize = tiles[gid].getSize();
					const int centerX = static_cast<int>(leftX + tileSize.x / 2.0);
					const int centerY = static_cast<int>(bottomY - tileSize.y / 2.0);
					m_gameObjects.push_back(make_shared<EffectObject>(	tiles[gid], 
																		centerX, centerY, 
																		0, 0, tileSize.x, tileSize.y,
																		m_physWorld.get(), 
																		stof(properties["friction"]), 
																		stof(properties["restitution"]),
																		stoi(properties["live"]), 
																		stoi(properties["coin"]), 
																		(properties["destroy"] == "1"))
											);
				}
				// TO DO добавить другие типы
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void World::loadFromFile(const std::string& level)
	{
		m_gameObjects.clear();
		m_physWorld.reset(nullptr);
		loadMapFromFile(level);
		m_lastLevel = level;
	}
	//////////////////////////////////////////////////////////////////////////
	void World::runGameTiming(float delta)
	{
		delta *= m_timeWarp;

		for (auto& itGameObject : m_gameObjects)
		{
			itGameObject->logic(delta);
			itGameObject->animate(delta);
		}

		const int32 velocityIterations = 6;
		const int32 positionIterations = 2;

		m_physWorld->Step(delta, velocityIterations, positionIterations);

		m_gameObjects.remove_if([](std::shared_ptr<Object>& obj){	return obj->destroy(); });
	}
	//////////////////////////////////////////////////////////////////////////
	void World::runDrawing(sf::RenderWindow& render)
	{
		assert(m_controlView);
		m_controlView(render);

		sf::RectangleShape bgShape(render.getView().getSize());
		bgShape.setPosition(render.mapPixelToCoords(sf::Vector2i(0, 0)));
		bgShape.setFillColor(m_mapBgColor);
		render.draw(bgShape);

		for (auto& itGameObject : m_gameObjects)
		{
			itGameObject->draw(render);
		}

		assert(m_drawHUD);
		m_drawHUD(render);
	}
	//////////////////////////////////////////////////////////////////////////
	bool World::isWin()
	{
		assert(m_playerWin);
		return m_playerWin();
	}
	//////////////////////////////////////////////////////////////////////////
	bool World::isFail()
	{
		assert(m_playerFail);
		return m_playerFail();
	}
	//////////////////////////////////////////////////////////////////////////
	const std::string& World::getLastLevel() const
	{
		return m_lastLevel;
	}
	//////////////////////////////////////////////////////////////////////////
}