#include"Game.hpp"

using namespace std;

Game::Game(android_app* pApplication,GraphicsHandler* pGraphicsManager)
{
	mApplication = pApplication;
	mGraphicsManager =  pGraphicsManager;
	mMainScreen = new GameScreen(pGraphicsManager);
}

void Game::onStart()
{
	mCurrentScreen=mMainScreen;
}

Game::~Game()
{

}

void Game::onResume()
{

}
void Game::onPause()
{

}
void Game::onStop()
{

}
void Game::onDestroy()
{

}
void Game::onDeactivate()
{

}
void Game::onSaveState(void** pData, size_t* pSize)
{

}
void Game::onConfigurationChanged()
{

}
void Game::onLowMemory()
{

}

void Game::onCreateWindow()
{

}
void Game::onDestroyWindow()
{

}
void Game::onGainFocus()
{
	mGraphicsManager->start();
}
void Game::onLostFocus()
{

}
