#include <android_native_app_glue.h>
#include"GraphicsHandler.hpp"
//#include "Log.hpp"
#include"Game.hpp"
#include"EventHandler.hpp"
#include"ActivityHandler.hpp"

void android_main(android_app* pApplication)
{
	app_dummy();
	// Creates services.
	EventHandler* lEventManager = new EventHandler::EventHandler(pApplication);
	GraphicsHandler* lGraphicsManager =  new GraphicsHandler::GraphicsHandler(pApplication);
	Game* waterSim= new Game(pApplication,lGraphicsManager);
	lEventManager->run(waterSim);
}

