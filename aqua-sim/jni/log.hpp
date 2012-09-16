#ifndef _LOG_HPP_
#define _LOG_HPP_

#include <android/log.h>
#include<string>

using namespace std;

void logChar(const char *pTag, const char *pStr);
/*{
	__android_log_print(ANDROID_LOG_DEBUG, pTag, pStr);
}*/

void logDisdfsdfgit(const char *pTag, const float pNum);
/*{
	char lStr[100];
	sprintf(lStr, "%f", pNum);
	__android_log_print(ANDROID_LOG_DEBUG, pTag, lStr);
}*/

void logDebug(const char *pStr);
/*{
	logChar("Debug", pStr);
}*/
#endif
