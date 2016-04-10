#include "config.h"

namespace GameSpace
{
	//////////////////////////////////////////////////////////////////////////
	std::string Config::readValFromXml(TiXmlElement* element, const std::string& name) const
	{
		const char* value = element->Attribute(name.c_str());
		if (!value)
		{
			throw std::runtime_error("GameSpace::Config::readValFromXml if (!value)");
		}
		return std::string(value);
	}
	//////////////////////////////////////////////////////////////////////////
	Config::Config(const char* config) : m_configName(config)
	{
		TiXmlDocument document(config);
		if (!document.LoadFile())
		{
			throw std::runtime_error("GameSpace::Config::Config if (!document.LoadFile())");
		}
		TiXmlElement* gameRoot = document.FirstChildElement("game");
		if (!gameRoot)
		{
			throw std::runtime_error("GameSpace::Config::Config if (!gameRoot)");
		}
		readScreenConfig(gameRoot);
		readMenuConfig(gameRoot);
		readLevelsConfig(gameRoot);
		readInfoConfig(gameRoot, "win", m_winConfig);
		readInfoConfig(gameRoot, "fail", m_failConfig);
	}
	//////////////////////////////////////////////////////////////////////////
	void Config::readScreenConfig(TiXmlElement* rootElement)
	{
		using namespace std;
		m_screenConfig = ScreenConfig(
								stoi(readValFromXml(rootElement, "width")),
								stoi(readValFromXml(rootElement, "height")),
								readValFromXml(rootElement, "caption"),
								(readValFromXml(rootElement, "fullscreen") == "1")
							);
	}
	//////////////////////////////////////////////////////////////////////////
	void Config::readMenuConfig(TiXmlElement* rootElement)
	{
		TiXmlElement* menuRoot = rootElement->FirstChildElement("menu");
		if (!menuRoot)
		{
			throw std::runtime_error("GameSpace::Config::getMenuConfig if (!menuRoot)");
		}
		TiXmlElement* startButton = menuRoot->FirstChildElement("start");
		if (!startButton)
		{
			throw std::runtime_error("GameSpace::Config::getMenuConfig if (!startButton)");
		}
		TiXmlElement* exitButton = menuRoot->FirstChildElement("exit");
		if (!exitButton)
		{
			throw std::runtime_error("GameSpace::Config::getMenuConfig if (!exitButton)");
		}
		m_menuConfig = MenuConfig(	readValFromXml(menuRoot, "bgimg"), 
									readValFromXml(startButton, "img"), 
									readValFromXml(exitButton, "img")	);
	}
	//////////////////////////////////////////////////////////////////////////
	void Config::readLevelsConfig(TiXmlElement* rootElement)
	{
		using namespace std;
		TiXmlElement* levelRoot = rootElement->FirstChildElement("levels");
		if (!levelRoot)
		{
			throw std::runtime_error("GameSpace::Config::getLevelsConfig if (!levelRoot)");
		}
		m_levelsConfig = LevelsConfig(	readValFromXml(levelRoot, "bgimg"),
										stoi(readValFromXml(levelRoot, "blockPerLine")),
										stoi(readValFromXml(levelRoot, "linePerScreen")),
										readValFromXml(levelRoot, "loadimg")	);
		for (TiXmlElement* lvl = levelRoot->FirstChildElement();
			lvl;
			lvl = lvl->NextSiblingElement())
		{
			const std::string image(readValFromXml(lvl, "img"));
			const std::string file(readValFromXml(lvl, "file"));
			const bool compl = (readValFromXml(lvl, "completed") == "1");

			m_levelsConfig.m_levels.emplace_back(image, file, compl);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Config::markLevelAsCompleted(const std::string& levelFile)
	{
		TiXmlDocument document(m_configName.c_str());
		if (!document.LoadFile())
		{
			throw std::runtime_error("GameSpace::Config::markLevelAsCompleted if (!document.LoadFile())");
		}
		TiXmlElement* gameRoot = document.FirstChildElement("game");
		if (!gameRoot)
		{
			throw std::runtime_error("GameSpace::Config::markLevelAsCompleted if (!gameRoot)");
		}
		TiXmlElement* levelRoot = gameRoot->FirstChildElement("levels");
		if (!levelRoot)
		{
			throw std::runtime_error("GameSpace::Config::markLevelAsCompleted if (!levelRoot)");
		}
		for (TiXmlElement* lvl = levelRoot->FirstChildElement();
			lvl;
			lvl = lvl->NextSiblingElement())
		{
			if (readValFromXml(lvl, "file") == levelFile)
			{
				lvl->SetAttribute("completed", "1");
				break;
			}
		}
		document.SaveFile();

		readLevelsConfig(gameRoot);
	}
	//////////////////////////////////////////////////////////////////////////
	void Config::readInfoConfig(TiXmlElement* rootElement, const std::string& info, InfoConfig& result)
	{
		TiXmlElement* infoItem = rootElement->FirstChildElement(info.c_str());
		if (!infoItem)
		{
			throw std::runtime_error("GameSpace::Config::getWinConfig if (!infoItem)");
		}
		result = InfoConfig(readValFromXml(infoItem, "bgimg"),
							readValFromXml(infoItem, "img"));
	}
	//////////////////////////////////////////////////////////////////////////
}