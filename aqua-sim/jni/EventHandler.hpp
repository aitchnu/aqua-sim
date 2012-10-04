#ifndef _EVENTHANDLER_HPP_
#define _EVENTHANDLER_HPP_

#include <android_native_app_glue.h>
#include"ActivityHandler.hpp"
#include"log.hpp"

class EventHandler
{
	public:
		EventHandler(android_app* pApplication);
		~EventHandler();
		void run(ActivityHandler* pActivityHandler);
		void processAppEvent(int32_t pCommand);
		void processInputEvent();
		static void callback_event(android_app* pApplication,int32_t pCommand);

	public:
		bool mEnabled;
		bool mQuit;
//	InputHandler* mInputHandler;
		android_app* mApplication;
		ActivityHandler* mActivityHandler;
};

#endif
