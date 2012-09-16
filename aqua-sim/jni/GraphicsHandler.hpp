#ifndef _GRAPHICSHANDLER_HPP_
#define _GRAPHICSHANDLER_HPP_

#include <android_native_app_glue.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include"log.hpp"

using namespace std;

class GraphicsHandler
{
	public:
		GraphicsHandler(android_app* pApplication);
		~GraphicsHandler();

		const int32_t& getHeight();
		const int32_t& getWidth();

		void start();
		void stop();
		void update();

//		GraphicsTexture* registerTexture(const char* pPath);
//		GraphicsSprite* registerSprite(GraphicsTexture* pTexture,
//				int32_t pHeight, int32_t pWidth, Location* pLocation);
//		GraphicsTileMap* registerTileMap(const char* pPath,
//				GraphicsTexture* pTexture, Location* pLocation);

	protected:
//		status loadResources();
//		status unloadResources();
		void setup();

	private:
		android_app* mApplication;
//		TimeService* mTimeService;

		// Display properties.
		int32_t mWidth, mHeight;
		EGLDisplay mDisplay;
		EGLSurface mSurface;
		EGLContext mContext;

		// Graphics resources.
//		GraphicsTexture* mTextures[32];
//		int32_t mTextureCount;
//		GraphicsSprite* mSprites[256];
//		int32_t mSpriteCount;
//		GraphicsTileMap* mTileMaps[8];
//		int32_t mTileMapCount;
};

#endif
