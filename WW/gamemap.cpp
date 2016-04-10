#include "gamemap.h"

namespace GameSpace
{
	//////////////////////////////////////////////////////////////////////////
	std::string GameMap::getStrAttribute(TiXmlElement* element, const char* attribName)
	{
		const char* attrVal = element->Attribute(attribName);
		if (!attrVal)
		{
			throw std::runtime_error("GameSpace::GameMap::getStrAttribute if (!attrVal)");
		}
		return std::string(attrVal);
	}
	//////////////////////////////////////////////////////////////////////////
	sf::Color GameMap::getColorAttribute(TiXmlElement* element, const char* attribName)
	{
		if (const char* bgColorPtr = element->Attribute(attribName))
		{
			const std::string bgColor(bgColorPtr + 1);
			int componentOffset = 0;

			int alphaVal = 255, redVal, greenVal, blueVal;
			
			if (bgColor.length() == 8)
			{
				alphaVal = std::stoi(bgColor.substr(componentOffset, 2), nullptr, 16);
				componentOffset += 2;
			}
			
			redVal = std::stoi(bgColor.substr(componentOffset, 2), nullptr, 16);
			componentOffset += 2;
			greenVal = std::stoi(bgColor.substr(componentOffset, 2), nullptr, 16);
			componentOffset += 2;
			blueVal = std::stoi(bgColor.substr(componentOffset, 2), nullptr, 16);

			return sf::Color(redVal, greenVal, blueVal, alphaVal);
		}
		return sf::Color();
	}
	//////////////////////////////////////////////////////////////////////////
	void GameMap::loadFromFile(const std::string& mapFile)
	{
		using namespace std;

		TiXmlDocument mapInXml(mapFile.c_str());
		if (!mapInXml.LoadFile())
		{
			throw runtime_error("GameSpace::GameMap::loadFromFile if (!mapInXml.LoadFile())");
		}
		TiXmlElement* rootMapItem = mapInXml.FirstChildElement("map");
		if (!rootMapItem)
		{
			throw runtime_error("GameSpace::GameMap::loadFromFile if (!rootMapItem)");
		}
		
		const int mapWidth = stoi(getStrAttribute(rootMapItem, "width"));
		const int mapHeight = stoi(getStrAttribute(rootMapItem, "height"));
		const int tileWidth = stoi(getStrAttribute(rootMapItem, "tilewidth"));
		const int tileHeight = stoi(getStrAttribute(rootMapItem, "tileheight"));
		m_bgColor = getColorAttribute(rootMapItem, "backgroundcolor");

		map<int, string> tiles;
		for (TiXmlElement* rootSectionItem = rootMapItem->FirstChildElement(); rootSectionItem; rootSectionItem = rootSectionItem->NextSiblingElement())
		{
			if (strcmp(rootSectionItem->Value(), "tileset") == 0)
			{
				const int firstGid = stoi(getStrAttribute(rootSectionItem, "firstgid"));
				for (TiXmlElement* tileItem = rootSectionItem->FirstChildElement(); tileItem; tileItem = tileItem->NextSiblingElement())
				{
					TiXmlElement* imgItem = tileItem->FirstChildElement("image");
					if (!imgItem)
					{
						throw runtime_error("GameSpace::GameMap::loadFromFile if (!img)");
					}
					tiles[firstGid + stoi(getStrAttribute(tileItem, "id"))] = getStrAttribute(imgItem, "source");
				}
			}
			else if (strcmp(rootSectionItem->Value(), "layer") == 0)
			{
				const string layerName(getStrAttribute(rootSectionItem, "name"));
				int currentTileNumber = 0;
				for (TiXmlElement* tile = rootSectionItem->FirstChildElement("data")->FirstChildElement(); tile; tile = tile->NextSiblingElement())
				{
					auto itTile = tiles.find(stoi(getStrAttribute(tile, "gid")));
					if (itTile != tiles.end())
					{
						sf::Image tileImage;
						if (!tileImage.loadFromFile(itTile->second))
						{
							throw runtime_error("GameSpace::GameMap::loadFromFile if (!tileImage.loadFromFile(tiles[atoi(tileGidPtr)]))");
						}
						float tilePosX = floor(static_cast<float>(tileWidth * (currentTileNumber % mapWidth)));
						float tilePosY = floor(static_cast<float>(tileHeight * (currentTileNumber / mapWidth)));

						if (layerName.compare(0, strlen("opaque"), "opaque") == 0)
						{
							m_opaques.emplace_back(tileImage, tilePosX, tilePosY);
						}
						else if (layerName.compare(0, strlen("transparent"), "transparent") == 0)
						{
							m_transparents.emplace_back(tileImage, tilePosX, tilePosY);
						}
					}
					++currentTileNumber;
				}
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void GameMap::draw(sf::RenderWindow& render) const
	{
		using namespace std;
		for_each(m_opaques.cbegin(), m_opaques.cend(), bind(&Object::draw, placeholders::_1, std::ref(render)));
		for_each(m_transparents.cbegin(), m_transparents.cend(), bind(&Object::draw, placeholders::_1, std::ref(render)));
	}
	//////////////////////////////////////////////////////////////////////////
	const sf::Color& GameMap::getBackground() const
	{
		return m_bgColor;
	}
	//////////////////////////////////////////////////////////////////////////
}