#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <stdexcept>
#include <vector>

#include <tinyxml.h>

namespace GameSpace
{
	/*
		@brief	описывает конфигурацию игры
				читает конфигурацию из конфигурационного файла игры
				также записывает конфигурацию для сохранения прогресса
				все настройки кэшируются
	*/
	class Config final
	{
	public:
		// @brief описывает настройки экрана
		struct ScreenConfig
		{
			unsigned int m_width, m_height;
			std::string m_caption;
			bool m_isFullScreen;
			ScreenConfig(unsigned int width, unsigned int height, const std::string& caption, bool isFullScreen)
				: m_width(width), m_height(height), m_caption(caption), m_isFullScreen(isFullScreen)
			{}
			ScreenConfig() = default;
		};
		// @brief описывает настройки меню
		struct MenuConfig
		{
			std::string m_backgroundImage;
			std::string m_startImage;
			std::string m_exitImage;
			MenuConfig(const std::string& backgroundImage,
				const std::string& startImage,
				const std::string& exitImage)
				: m_backgroundImage(backgroundImage),
				m_startImage(startImage),
				m_exitImage(exitImage)
			{}
			MenuConfig() = default;
		};
		// описывает настройки уровней
		struct LevelsConfig
		{
			std::string m_backgroundImage;
			int m_blockPerLine;
			int m_linePerScreen;
			std::string m_loadImage;
			struct Level
			{
				std::string m_image;
				std::string m_levelFile;
				bool m_isCompleted;
				Level(const std::string image, const std::string levelFile, bool isCompleted)
					: m_image(image), m_levelFile(levelFile), m_isCompleted(isCompleted)
				{}
			};
			std::vector<Level> m_levels;
			LevelsConfig(const std::string backgroundImage, int blockPerLine, int linePerScreen, const std::string& loadImage)
				:	m_backgroundImage(backgroundImage), 
					m_blockPerLine(blockPerLine), 
					m_linePerScreen(linePerScreen),
					m_loadImage(loadImage)
			{}
			LevelsConfig() = default;
		};
		// описывает настройки информационных экранов
		struct InfoConfig
		{
			std::string m_backgroundImage;
			std::string m_image;
			InfoConfig(const std::string& backgroundImage, const std::string& image)
				: m_backgroundImage(backgroundImage), m_image(image)
			{}
			InfoConfig() = default;
		};
		/*
			@brief открывает файл конфигурации
			@param config путь к файлу конфигурации
		*/
		Config(const char* config);
		/*
			@brief читает настройки экрана
			@ret   настройки экрана
		*/
		const ScreenConfig& getScreenConfig() const
		{ return m_screenConfig; }
		/*
			@brief читает настройки меню
			@ret   настройки меню
		*/
		const MenuConfig& getMenuConfig() const
		{ return m_menuConfig; }
		/*
			@brief считывает настройки уровней
			@ret   настройки уровней
		*/
		const LevelsConfig& getLevelsConfig() const
		{ return m_levelsConfig; }
		/*
			@brief настройки окна выйгрыша
		*/
		const InfoConfig& getWinConfig() const
		{ return m_winConfig; }
		/*
			@brief настройки окна проигрыша
		*/
		const InfoConfig& getFailConfig() const
		{ return m_failConfig; }
		/*
		@brief отметить уровень как пройденный
		*/
		void markLevelAsCompleted(const std::string& levelFile);
	private:
		std::string m_configName;

		ScreenConfig m_screenConfig; // настройки окна
		MenuConfig m_menuConfig; // настройки экрана меню
		LevelsConfig m_levelsConfig; // настройки экрана выбора уровня
		InfoConfig m_winConfig; // настройки экрана выйгрыша
		InfoConfig m_failConfig; // настройка экрана проигрыша
		
		void readScreenConfig(TiXmlElement* rootElement);
		void readMenuConfig(TiXmlElement* rootElement);
		void readLevelsConfig(TiXmlElement* rootElement);
		void readInfoConfig(TiXmlElement* rootElement, const std::string& info, InfoConfig& result);
		/*
			@brief читает параметр из XML
			@param element элемент из которого следует читать
			@param name имя параметра для чтения
			@ret   считанное значение
		*/
		std::string readValFromXml(TiXmlElement* element, const std::string& name) const;
	};
}

#endif // !CONFIG_H
