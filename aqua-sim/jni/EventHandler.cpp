#include "EventHandler.hpp"

using namespace std;

EventHandler::EventHandler(android_app* pApplication)
{
	mEnabled = false;
	mApplication = pApplication;
	mApplication->onAppCmd = callback_event;
	mApplication->userData = this;
}

void EventHandler::run(ActivityHandler* pActivityHandler)
{
	int32_t lResult;
	int32_t lEvents;
	android_poll_source* lSource;

	app_dummy();
	mActivityHandler=pActivityHandler;

	while (true)
	{
		while ((lResult = ALooper_pollAll(mEnabled ? 0 : -1, NULL, &lEvents,
				(void**) &lSource)) >= 0)
		{
			if (lSource != NULL)
			{
				logDebug("Event Occured");
				lSource->process(mApplication, lSource);
			}

			if (mApplication->destroyRequested)
			{
				return;
			}
		}

		if ((mEnabled) && (!mQuit))
		{
//			if (mActivityHandler->onStep() != STATUS_OK) {
				mQuit = true;
				ANativeActivity_finish(mApplication->activity);
//			}
		}
	}
}

void EventHandler::processAppEvent(int32_t pCommand)
{
	switch (pCommand)
	{
		case APP_CMD_CONFIG_CHANGED:
			logDebug("App configuration Changed");
			mActivityHandler->onConfigurationChanged();
			break;
		case APP_CMD_INIT_WINDOW:
			logDebug("App window Initialized");
			mActivityHandler->onCreateWindow();
			logDebug("Window Created");
			break;
		case APP_CMD_DESTROY:
			logDebug("App destroyed");
			mActivityHandler->onDestroy();
			break;
		case APP_CMD_GAINED_FOCUS:
			logDebug("App gained focus");
//           activate();
			mActivityHandler->onGainFocus();
			break;
		case APP_CMD_LOST_FOCUS:
			logDebug("App lost focus");
			mActivityHandler->onLostFocus();
			//          deactivate();
			break;
		case APP_CMD_LOW_MEMORY:
			logDebug("Low memory");
			mActivityHandler->onLowMemory();
			break;
		case APP_CMD_PAUSE:
			logDebug("App paused");
			mActivityHandler->onPause();
//           deactivate();
			break;
		case APP_CMD_RESUME:
			logDebug("App resumed");
			mActivityHandler->onResume();
			break;
		case APP_CMD_SAVE_STATE:
			logDebug("App save state");
			mActivityHandler->onSaveState(&mApplication->savedState,
					&mApplication->savedStateSize);
			break;
		case APP_CMD_START:
			logDebug("Start");
			mActivityHandler->onStart();
			break;
		case APP_CMD_STOP:
			logDebug("Stop");
			mActivityHandler->onStop();
			break;
		case APP_CMD_TERM_WINDOW:
			logDebug("Terminate Window");
			mActivityHandler->onDestroyWindow();
//           deactivate();
			break;
		default:
			break;
	}
}

void EventHandler::callback_event(android_app* pApplication,int32_t pCommand)
{
			EventHandler& lEventManager = *(EventHandler*) pApplication->userData;
	        lEventManager.processAppEvent(pCommand);
}

