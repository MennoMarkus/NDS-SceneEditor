#include "Main.h"

#include "services/Logger.h"
#include "Application.h"

int main(int argc, char** argv) 
{

	// Setup logging so we can track errors
	nds_se::Logger logger;
#ifdef _DEBUG
	logger.m_minLogLevel = nds_se::LOG_ALL;
	logger.m_exitThreshold = nds_se::LOG_FATAL;
#endif
	nds_se::ServiceLocator::get().provideService<nds_se::Logger>(&logger);

	nds_se::Application app;
	app.run();

	return 0;
}