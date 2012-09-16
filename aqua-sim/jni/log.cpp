#include "log.hpp"

void logChar(const char *pTag, const char *pStr)
{
	__android_log_print(ANDROID_LOG_DEBUG, pTag, pStr);
}

void logDisdfsdfgit(const char *pTag, const float pNum)
{
	char lStr[100];
	sprintf(lStr, "%f", pNum);
	__android_log_print(ANDROID_LOG_DEBUG, pTag, lStr);
}

void logDebug(const char *pStr)
{
	logChar("Debug", pStr);
}
