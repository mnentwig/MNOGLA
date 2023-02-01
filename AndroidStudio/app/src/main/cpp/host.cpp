#include <android/log.h>
#include <jni.h>
#include "../../../../../MNOGLA.h"
#define LOG_TAG "MNOGLA_host"

void host_logI(const char* format, ...){
  va_list args;
  va_start(args, /*start after*/format);
  __android_log_vprint(ANDROID_LOG_INFO, LOG_TAG, format, args);
  va_end(args);
}

void host_logE(const char* format, ...){
  va_list args;
  va_start(args, /*start after*/format);
  __android_log_vprint(ANDROID_LOG_ERROR, LOG_TAG, format, args);
  va_end(args);
}

extern "C" JNIEXPORT void JNICALL Java_com_android_MNOGLAJNI_MNOGLALIB_init(JNIEnv* /*env*/,
                                                              jclass,
                                                              jint width,
                                                              jint height) {
  MNOGLA_init(width, height, host_logI, host_logE);
}

extern "C" JNIEXPORT void JNICALL Java_com_android_MNOGLAJNI_MNOGLALIB_render(JNIEnv*, jclass) {
  MNOGLA_render();
}

extern "C" JNIEXPORT void JNICALL Java_com_android_MNOGLAJNI_MNOGLALIB_evt2(JNIEnv*, jclass, int32_t key, int32_t v1, int32_t v2) {
  MNOGLA_evtSubmitHostToApp(key, /*nArgs*/2, v1, v2);
}

extern "C" JNIEXPORT void JNICALL Java_com_android_MNOGLAJNI_MNOGLALIB_evt3(JNIEnv*, jclass, int32_t key, int32_t v1, int32_t v2, int32_t v3) {
  MNOGLA_evtSubmitHostToApp(key, /*nArgs*/3, v1, v2, v3);
}
