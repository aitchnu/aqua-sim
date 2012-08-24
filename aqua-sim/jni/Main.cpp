#include <jni.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <android_native_app_glue.h>
#include <android/log.h>
#include<stdlib.h>
#include <Box2D/Box2D.h>
#include<png.h>
#include<string>
#include<cstdio>

static GLuint programObject;
static EGLDisplay lDisplay;
static EGLSurface mSurface;
static int32_t lWidth, lHeight;
static android_app* lApplication;
static float timeStep = 1.0f / 60.0f;
static int velocityIterations = 6;
static int positionIterations = 2;
static int enabled;
static b2Vec2 gravity(0.0f, -10.0f);
static b2World newworld(gravity);
static AAssetManager* mAssetManager;
static AAsset* mAsset;
static png_byte* lImageBuffer = NULL;
static GLuint mTextureId;
static b2Body* body;

GLuint LoadShader(const char *shaderSrc, GLenum type) {
	GLuint shader;
	GLint compiled;
// Create the shader object
	shader = glCreateShader(type);
	if (shader == 0)
		return 0;
// Load the shader source
	glShaderSource(shader, 1, &shaderSrc, NULL);
// Compile the shader
	glCompileShader(shader);
// Check the compile status
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		__android_log_print(ANDROID_LOG_DEBUG, "test", "NOT COMPILED !!!");
		GLint infoLen = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
		if (infoLen > 1) {
//			char *infoLog2 = malloc(sizeof(char) * infoLen);
//			glGetShaderInfoLog(shader, infoLen, NULL, infoLog2);
//			free(infoLog);
		}
		glDeleteShader(shader);
		return 0;
	}
	return shader;
}

void callback_read(png_structp pStruct, png_bytep pData, png_size_t pSize) {
	AAsset_read(mAsset, pData, pSize);
}

void load_image() {
	png_byte lHeader[8];
	png_structp lPngPtr = NULL;
	png_infop lInfoPtr = NULL;

	png_bytep* lRowPtrs = NULL;
	png_int_32 lRowSize;
	bool lTransparency;
	// Opens and checks image signature (first 8 bytes).
	mAssetManager = lApplication->activity->assetManager;
	mAsset = AAssetManager_open(mAssetManager, "box.png", AASSET_MODE_UNKNOWN);
	// Creates required structures.
	lPngPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	lInfoPtr = png_create_info_struct(lPngPtr);

	// Prepares reading operation by setting-up a read callback.
	png_set_read_fn(lPngPtr, NULL, callback_read);
	// Set-up error management. If an error occurs while reading,
	// code will come back here and jump
	setjmp(png_jmpbuf(lPngPtr));

// Ignores first 8 bytes already read and processes header.
	png_set_sig_bytes(lPngPtr, 8);
	png_read_info(lPngPtr, lInfoPtr);
// Retrieves PNG info and updates PNG struct accordingly.
	png_int_32 lDepth, lColorType;
	png_uint_32 lWidth, lHeight;
	png_get_IHDR(lPngPtr, lInfoPtr, &lWidth, &lHeight, &lDepth, &lColorType,
			NULL, NULL, NULL);
	int32_t mWidth = lWidth;
	int32_t mHeight = lHeight;
	GLint mFormat;
// Creates a full alpha channel if transparency is encoded as
// an array of palette entries or a single transparent color.
	lTransparency = false;
	if (png_get_valid(lPngPtr, lInfoPtr, PNG_INFO_tRNS)) {
		png_set_tRNS_to_alpha(lPngPtr);
		lTransparency = true;
	}
// Expands PNG with less than 8bits per channel to 8bits.
	if (lDepth < 8) {
		png_set_packing(lPngPtr);
		// Shrinks PNG with 16bits per color channel down to 8bits.
	} else if (lDepth == 16) {
		png_set_strip_16(lPngPtr);
	}
// Indicates that image needs conversion to RGBA if needed.
	switch (lColorType) {
	case PNG_COLOR_TYPE_PALETTE:
		png_set_palette_to_rgb(lPngPtr);
		mFormat = lTransparency ? GL_RGBA : GL_RGB;
		break;
	case PNG_COLOR_TYPE_RGB:
		mFormat = lTransparency ? GL_RGBA : GL_RGB;
		break;
	case PNG_COLOR_TYPE_RGBA:
		mFormat = GL_RGBA;
		break;
	case PNG_COLOR_TYPE_GRAY:
		png_set_expand_gray_1_2_4_to_8(lPngPtr);
		mFormat = lTransparency ? GL_LUMINANCE_ALPHA : GL_LUMINANCE;
		break;
	case PNG_COLOR_TYPE_GA:
		png_set_expand_gray_1_2_4_to_8(lPngPtr);
		mFormat = GL_LUMINANCE_ALPHA;
		break;
	}
// Validates all tranformations.
	png_read_update_info(lPngPtr, lInfoPtr);

// Get row size in bytes.
	lRowSize = png_get_rowbytes(lPngPtr, lInfoPtr);
// Ceates the image buffer that will be sent to OpenGL.
	lImageBuffer = new png_byte[lRowSize * lHeight];
// Pointers to each row of the image buffer. Row order is
// inverted because different coordinate systems are used by
// OpenGL (1st pixel is at bottom left) and PNGs (top-left).
	lRowPtrs = new png_bytep[lHeight];
	for (int32_t i = 0; i < lHeight; ++i) {
		lRowPtrs[lHeight - (i + 1)] = lImageBuffer + i * lRowSize;
	}
// Reads image content.
	png_read_image(lPngPtr, lRowPtrs);

// Frees memory and resources.
	AAsset_close(mAsset);
	png_destroy_read_struct(&lPngPtr, &lInfoPtr, NULL);
	delete[] lRowPtrs;

	glGenTextures(1, &mTextureId);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mTextureId);

	// Set-up texture properties.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	// Loads image data into OpenGL.
	glTexImage2D(GL_TEXTURE_2D, 0, mFormat, mWidth, mHeight, 0, mFormat,
			GL_UNSIGNED_BYTE, lImageBuffer);
	delete[] lImageBuffer;
}

void initialize() {

	__android_log_print(ANDROID_LOG_DEBUG, "test", "initializing");
	EGLConfig lConfig;
	EGLint lFormat, lNumConfigs, lErrorResult;
//	const EGLint lAttributes[] = { EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
//			EGL_BLUE_SIZE, 5, EGL_GREEN_SIZE, 6, EGL_RED_SIZE, 5,
//			EGL_SURFACE_TYPE, EGL_WINDOW_BIT,EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};
	const EGLint lAttributes[] = {
//	            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
//	            EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
			EGL_BLUE_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_RED_SIZE, 8,
//	            EGL_DEPTH_SIZE,64,
//	            EGL_CONTEXT_CLIENT_VERSION, 2,
			EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT, EGL_NONE };
	lDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	if (eglInitialize(lDisplay, NULL, NULL) == EGL_TRUE)
		__android_log_print(ANDROID_LOG_DEBUG, "PACKT", "display is good");
	if (eglChooseConfig(lDisplay, lAttributes, &lConfig, 1,
			&lNumConfigs)==EGL_TRUE)
		__android_log_print(ANDROID_LOG_DEBUG, "PACKT",
				"choose config is good");
//EGLSurface mSurface;
	EGLContext mContext;
	if (eglGetConfigAttrib(lDisplay, lConfig, EGL_NATIVE_VISUAL_ID,
			&lFormat)==EGL_TRUE)
		__android_log_print(ANDROID_LOG_DEBUG, "PACKT", "get config is good");
	ANativeWindow_setBuffersGeometry(lApplication->window, 0, 0, lFormat);
	mSurface = eglCreateWindowSurface(lDisplay, lConfig, lApplication->window,
			NULL);
	if (mSurface == EGL_NO_SURFACE) {
		__android_log_print(ANDROID_LOG_DEBUG, "PACKT", "error in surface");
	}
	EGLint contextAttrs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
	mContext = eglCreateContext(lDisplay, lConfig, EGL_NO_CONTEXT,
			contextAttrs);
	eglMakeCurrent(lDisplay, mSurface, mSurface, mContext);
	eglQuerySurface(lDisplay, mSurface, EGL_WIDTH, &lWidth);
	eglQuerySurface(lDisplay, mSurface, EGL_HEIGHT, &lHeight);
	char vShaderStr[] ="uniform mat4 uMvp;\n"
			"attribute vec4 vPosition; \n"
			"attribute vec2 texCoord; \n"
			"varying vec2 v_texCoord; \n"
			"void main() \n"
			"{ \n"
			" gl_Position = uMvp * vPosition; \n"
			" v_texCoord = texCoord; \n"
			"} \n";
	char fShaderStr[] = "precision mediump float; \n"
			"varying vec2 v_texCoord; \n"
			"uniform sampler2D s_texture; \n"
			"void main() \n"
			"{ \n"
			" gl_FragColor = texture2D(s_texture, v_texCoord); \n"
			" //gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0); \n"
			"} \n";

	GLuint vertexShader;
	GLuint fragmentShader;
	GLint linked;
	vertexShader = LoadShader(vShaderStr, GL_VERTEX_SHADER);
	fragmentShader = LoadShader(fShaderStr, GL_FRAGMENT_SHADER);
// Create the program object
	programObject = glCreateProgram();
	glAttachShader(programObject, vertexShader);
	glAttachShader(programObject, fragmentShader);
// Bind vPosition to attribute 0
// Link the program
	glLinkProgram(programObject);
// Check the link status
	glGetProgramiv(programObject, GL_LINK_STATUS, &linked);
	if (!linked) {
		GLint infoLen = 0;
		glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &infoLen);
		if (infoLen > 1) {
//			char* infoLog1 = malloc(sizeof(char) * infoLen);
//			glGetProgramInfoLog(programObject, infoLen, NULL, infoLog1);
//			free(infoLog);
		}
		glDeleteProgram(programObject);
	}
// Store the program object
	char arr[100];
	sprintf(arr, "%d %d", lWidth, lHeight);
	__android_log_print(ANDROID_LOG_DEBUG, "test", arr);
	glViewport(0, 0, lWidth, lHeight);
	enabled = 1;
	load_image();

}

void processAppEvent(int32_t pCommand) {
	switch (pCommand) {
	case APP_CMD_CONFIG_CHANGED:
//		mActivityHandler->onConfigurationChanged();
		__android_log_print(ANDROID_LOG_DEBUG, "test",
				"APP_CMD_CONFIG_CHANGED");
		break;
	case APP_CMD_INIT_WINDOW:
		__android_log_print(ANDROID_LOG_DEBUG, "test", "Window created");
		break;
	case APP_CMD_DESTROY:
		__android_log_print(ANDROID_LOG_DEBUG, "test", "APP_CMD_DESTROY");
		ANativeActivity_finish(lApplication->activity);
//		mActivityHandler->onDestroy();
		break;
	case APP_CMD_GAINED_FOCUS:
		initialize();
		__android_log_print(ANDROID_LOG_DEBUG, "test", "focus baby !!");
//		mActivityHandler->onGainFocus();
		break;
	case APP_CMD_LOST_FOCUS:
		__android_log_print(ANDROID_LOG_DEBUG, "test", "focus lost");
//		mActivityHandler->onLostFocus();
//		deactivate();
		break;
	case APP_CMD_LOW_MEMORY:
		__android_log_print(ANDROID_LOG_DEBUG, "test", "low memory");
//		mActivityHandler->onLowMemory();
		break;
	case APP_CMD_PAUSE:
		__android_log_print(ANDROID_LOG_DEBUG, "test", "pause");
		enabled = 0;
		eglMakeCurrent(lDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE,
				EGL_NO_CONTEXT);
		eglDestroySurface(lDisplay, mSurface);
		eglTerminate(lDisplay);
//		mActivityHandler->onPause();
//		deactivate();
		break;
	case APP_CMD_RESUME:
		__android_log_print(ANDROID_LOG_DEBUG, "test", "resume");
//		mActivityHandler->onResume();
		break;
	case APP_CMD_SAVE_STATE:
//		mActivityHandler->onSaveState(&mApplication->savedState,
//				&mApplication->savedStateSize);
		break;
	case APP_CMD_START:
		__android_log_print(ANDROID_LOG_DEBUG, "test", "start");
//		mActivityHandler->onStart();
		break;
	case APP_CMD_STOP:
		__android_log_print(ANDROID_LOG_DEBUG, "test", "stop");
//		mActivityHandler->onStop();
		break;
	case APP_CMD_TERM_WINDOW:
		__android_log_print(ANDROID_LOG_DEBUG, "test", "destroy window");
//		mActivityHandler->onDestroyWindow();
//		deactivate();
		break;
	default:
		break;
	}
}

void callback_event(android_app* pApplication, int32_t pCommand) {
	processAppEvent(pCommand);
}

void create_world() {
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(0.0f, 4.0f);
	body = newworld.CreateBody(&bodyDef);
	b2PolygonShape dynamicBox;
	dynamicBox.SetAsBox(1.0f, 1.0f);
	b2FixtureDef fixtureDef;
	fixtureDef.shape = &dynamicBox;
	fixtureDef.density = 1.0f;
	fixtureDef.friction = 0.3f;
	body->CreateFixture(&fixtureDef);
	char temp[100]="blank \0";
//	if (body!=NULL)
//	{
//			sprintf(temp,"%f",(float)body->GetPosition().x);
//	__android_log_print(ANDROID_LOG_DEBUG, "test error",temp);
//	}
//		else
//			__android_log_print(ANDROID_LOG_DEBUG, "test error","NULL here also");
}

void update() {
	newworld.Step(timeStep, velocityIterations, positionIterations);
	// for ortho2D far=1 and near=-1
	GLfloat mvp[][4]={{ 2.0/320.0 , 0 , 0 ,0},
					  { 0 , 2.0/240.0 , 0 , 0},
					  { 0 , 0 , 1 , 0},
					  { -1 , -1 , 0 , 1}};
	GLfloat vVertices[] =
			{ 100.0f, 100.0f, 0.0f, 1.0f, 1.0f,
			  0.0f, 100.0f, 0.0f, 0.0f, 1.0f,
			  0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
			 100.0f, 0.0f,0.0f, 1.0f, 0.0f };
	GLshort indices[] = {0,1,2, 0,2,3};
	static float lClearColor = 0.0f;
//lClearColor += 10.0f * 0.01f;
	glClearColor(lClearColor, lClearColor, lClearColor, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(programObject);

	GLuint vertexPos = glGetAttribLocation(programObject, "vPosition");
	glBindAttribLocation(programObject, vertexPos, "vPosition");
	GLuint texPos = glGetAttribLocation(programObject, "texCoord");
	glBindAttribLocation(programObject, texPos, "texCoord");
	GLuint mMvpLoc = glGetUniformLocation(programObject,"uMvp");
	glUniformMatrix4fv(mMvpLoc, 1, false,mvp[0]);

	glEnableVertexAttribArray(vertexPos);
	glEnableVertexAttribArray(texPos);

	glVertexAttribPointer(vertexPos, 3, GL_FLOAT, GL_FALSE, 20, vVertices);
	glVertexAttribPointer(texPos, 2, GL_FLOAT, GL_FALSE, 20, vVertices + 3);

	char temp[100]="blank \0";
//	b2Vec2 mypos = body->GetPosition();
//	if (body!=NULL)
//		sprintf(temp,"x=%f y=%f vx=%f vy=%f",(float)body->GetPosition().x,(float)body->GetPosition().y,(float)body->GetLinearVelocity().x,(float)body->GetLinearVelocity().y);
//	__android_log_print(ANDROID_LOG_DEBUG, "test error", temp);
	glUniform1i(glGetUniformLocation(programObject, "s_texture"), 0);
//	if(glGetError()==GL_INVALID_OPERATION)
//			__android_log_print(ANDROID_LOG_DEBUG, "test error", "yup before !!");
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT,indices);
//	if(glGetError()==GL_INVALID_OPERATION)
//		__android_log_print(ANDROID_LOG_DEBUG, "test error", "yup");
	eglSwapBuffers(lDisplay, mSurface);
//	__android_log_print(ANDROID_LOG_DEBUG, "PACKT", "oh ho... update");

}

void check_event() {
	int32_t lResult;
	int32_t lEvents;
	android_poll_source* lSource;
	lApplication->onAppCmd = callback_event;
// Makes sure native glue is not stripped by the linker.
	app_dummy();
//	mActivityHandler = pActivityHandler;

// Global step loop.
	while (true) {
// Event processing loop.
		while ((lResult = ALooper_pollAll(0, NULL, &lEvents, (void**) &lSource))
				>= 0) {
			// An event has to be processed.
			if (lSource != NULL) {
				lSource->process(lApplication, lSource);
				__android_log_print(ANDROID_LOG_DEBUG, "test",
						"event occured !!");
			}
			// Application is getting destroyed.
//			if (mApplication->destroyRequested) {
//				return;
//			}
		}
		if (enabled == 1)
			update();

// Steps the application.
//		if ((mEnabled) && (!mQuit)) {
//			if (mActivityHandler->onStep() != STATUS_OK) {
//				mQuit = true;
//				ANativeActivity_finish(mApplication->activity);
//			}
//		}
	}
}

void android_main(android_app* pApplication) {
	enabled = 0;
	app_dummy();
	lApplication = pApplication;
	create_world();
	check_event();
}
