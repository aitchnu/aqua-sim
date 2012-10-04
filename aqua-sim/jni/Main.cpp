#include <android_native_app_glue.h>
#include"GraphicsHandler.hpp"
#include"Game.hpp"
#include"EventHandler.hpp"
#include"ActivityHandler.hpp"

void android_main(android_app* pApplication)
{
	app_dummy();
	EventHandler* lEventManager = new EventHandler::EventHandler(pApplication);
	ResourceHandler* lResourceManager = new ResourceHandler::ResourceHandler(pApplication);
	GraphicsHandler* lGraphicsManager =  new GraphicsHandler::GraphicsHandler(pApplication,lResourceManager);
	Game* waterSim= new Game(pApplication,lGraphicsManager);
	lEventManager->run(waterSim);
}
