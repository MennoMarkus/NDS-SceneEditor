#pragma once
#include <sstream>
#include <string>
#include <functional>
#include "ServiceLocator.h"
#include "Service.h"

#define LOGGER nds_se::ServiceLocator::get().getService<nds_se::Logger>()

#define LOG(level, message) \
nds_se::ServiceLocator::get().getService<nds_se::Logger>()->logBegin((level)) \
<< message; \
nds_se::ServiceLocator::get().getService<nds_se::Logger>()->logEnd()

namespace nds_se
{
	enum LogLevel
	{
		LOG_ALL = 0,	// Display all logging
		LOG_DEBUG,
		LOG_INFO,
		LOG_WARNING,
		LOG_ERROR,
		LOG_FATAL,
		LOG_NONE		// Diable all logging
	};

	using LogCallbackID  = unsigned int;
	using LogCallbackFunction = std::function<void(const std::string&)>;

	class Logger : public Service
	{
	public:
		LogLevel m_minLogLevel = LOG_INFO; // Report all message on log levels heigher or equal to minLogLevel and lower or equal to maxLogLevel.
		LogLevel m_maxLogLevel = LOG_FATAL;
		LogLevel m_exitThreshold = LOG_ERROR; // Exit the application on log levels higher than or equal.

	private:
		std::ostringstream os;
		LogLevel m_currentMessageLevel = LOG_NONE;

		LogCallbackID m_currentCallbackID = 0;
		std::vector<std::pair<LogCallbackID, LogCallbackFunction>> m_callbacks;

	public:
		Logger();
		~Logger();
		void log(LogLevel level, const std::string& message);

		std::ostringstream& logBegin(LogLevel level);
		void logEnd();

		LogCallbackID registerCallback(LogCallbackFunction function);
		void unregisterCallback(LogCallbackID ID);
	};
}