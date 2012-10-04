#ifndef _GAMESCREEN_HPP_
#define _GAMESCREEN_HPP_

#include"../GraphicsHandler.hpp"
#include"Screen.hpp"

using namespace std;

class GameScreen : public Screen
{
	public:
		GameScreen(GraphicsHandler* pGraphicsManager);
		~GameScreen();
		void load();
		void pause();
		void update();
		void render();

	private:
		GraphicsHandler* mGraphicsManager;
		//	packt::InputService* mInputService;
		//	packt::PhysicsService* mPhysicsService;
		//	packt::SoundService* mSoundService;
		//	packt::TimeService* mTimeService;
		android_app* mApplication;
};

#endif
