#include "services/Logger.h"

#include <iostream>

namespace nds_se
{
	Logger::Logger() :
		os(std::string(128, '\0')) // Reserve 128 characters
	{}

	Logger::~Logger()
	{
		logEnd();
		fflush(stdout);
	}

	void Logger::log(LogLevel level, const std::string& message)
	{
		logBegin(level) << message;
		logEnd();
	}

	std::ostringstream& Logger::logBegin(LogLevel level)
	{
		// Force end current log
		if (m_currentMessageLevel != LOG_NONE)
			logEnd();

		m_currentMessageLevel = level;

		switch (level)
		{
			case LOG_DEBUG:	os << "[DEBUG]\t"; break;
			case LOG_INFO: os << "[INFO]\t"; break;
			case LOG_WARNING: os << "[WARNING]\t"; break;
			case LOG_ERROR:	os << "[ERROR]\t"; break;
			case LOG_FATAL: os << "[FATAL]\t"; break;
			default:
				break;
		}

		return os;
	}

	void Logger::logEnd()
	{
		if (m_currentMessageLevel >= m_minLogLevel && m_currentMessageLevel <= m_maxLogLevel)
		{
			// Print to the console
			os << "\n";
			std::string message = os.str();
			fprintf(stdout, "%s", message.c_str());
			fflush(stdout);

			// Print to callbacks
			for (auto& callback : m_callbacks)
			{
				std::invoke(callback.second, message);
			}
		}

		// Clear buffer
		os.str("");

		if (m_currentMessageLevel >= m_exitThreshold && m_currentMessageLevel != LOG_NONE)
		{
			exit(1);
		}

		m_currentMessageLevel = LOG_NONE;
	}

	LogCallbackID Logger::registerCallback(LogCallbackFunction function)
	{
		m_currentCallbackID++;
		m_callbacks.push_back(std::make_pair(m_currentCallbackID, function));
		return m_currentCallbackID;
	}

	void Logger::unregisterCallback(LogCallbackID ID)
	{
		for (auto it = m_callbacks.begin(); it != m_callbacks.end(); ++it)
		{
			if (it->first == ID)
			{
				m_callbacks.erase(it);
				return;
			}
		}
	}
}