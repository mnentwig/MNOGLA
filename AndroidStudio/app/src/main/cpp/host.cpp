#include <android/log.h>
#include <jni.h>
#include "../../../../../MNOGLA.h"
#include <utility> // std::move

#define LOG_TAG "MNOGLA_host"

// =========================================================================================
// RAII execution of (cleanup) code, from GSL
// =========================================================================================
template<class F>
class final_act {
public:
    explicit final_act(F f) noexcept: f_(std::move(f)), invoke_(true) {}

    final_act(final_act &&other) noexcept: f_(std::move(other.f_)), invoke_(other.invoke_) {
        other.invoke_ = false;
    }

    final_act(const final_act &) = delete;

    final_act &operator=(const final_act &) = delete;

    ~final_act() noexcept {
        if (invoke_) f_();
    }

private:
    F f_;
    bool invoke_;
};

// =========================================================================================
// Logging
// =========================================================================================
void host_logI(const char *format, ...) {
    va_list args;
    va_start(args, /*start after*/format);
    __android_log_vprint(ANDROID_LOG_INFO, LOG_TAG, format, args);
    va_end(args);
}

void host_logE(const char *format, ...) {
    va_list args;
    va_start(args, /*start after*/format);
    __android_log_vprint(ANDROID_LOG_ERROR, LOG_TAG, format, args);
    va_end(args);
}

//#define MNOGLA_HASAUDIO
#ifdef MNOGLA_HASAUDIO
// =========================================================================================
// Audio
// =========================================================================================
#include <aaudio/AAudio.h>
#include <thread>
#include <mutex>

class audio {
    static AAudioStream *audioStream;

    static aaudio_data_callback_result_t dataCallback(
            AAudioStream */*stream*/,
            void */*userData*/,
            void *audioData,
            int32_t numFrames) {
        auto *audioBuf = static_cast<float *>(audioData);
        static float phi = 0;
        for (size_t ix = 0; ix < numFrames; ++ix) {
            *(audioBuf++) = 0.1f * cos(phi);
            phi += 1000.0f / 48000.0f * 2.0f * M_PI;
        }
        int n = phi / (2 * M_PI);
        phi -= (float) (n * (2 * M_PI));
        return AAUDIO_CALLBACK_RESULT_CONTINUE;
    }

    static void errorCallback(AAudioStream */*stream*/,
                              void */*userData*/,
                              aaudio_result_t error) {
        if (error == AAUDIO_ERROR_DISCONNECTED) {
            new std::thread(restart);
        }
    }

public:
    static void start() {
        const size_t nChan = 1;
        AAudioStreamBuilder *streamBuilder;
        AAudio_createStreamBuilder(&streamBuilder);
//        auto f = [streamBuilder] { AAudioStreamBuilder_delete(streamBuilder); };
        final_act shutdown([&] { AAudioStreamBuilder_delete(streamBuilder); });
        AAudioStreamBuilder_setFormat(streamBuilder, AAUDIO_FORMAT_PCM_FLOAT);
        AAudioStreamBuilder_setChannelCount(streamBuilder, nChan);
        AAudioStreamBuilder_setPerformanceMode(streamBuilder, AAUDIO_PERFORMANCE_MODE_LOW_LATENCY);
        AAudioStreamBuilder_setDataCallback(streamBuilder, dataCallback, nullptr);
        AAudioStreamBuilder_setErrorCallback(streamBuilder, errorCallback, nullptr);

        // Opens the stream.
        aaudio_result_t result = AAudioStreamBuilder_openStream(streamBuilder, &audioStream);
        if (result != AAUDIO_OK) {
            host_logE("Error opening audio stream %s:", AAudio_convertResultToText(result));
            return;
        }

        int32_t sampleRate = AAudioStream_getSampleRate(audioStream);
        AAudioStream_setBufferSizeInFrames(
                audioStream, AAudioStream_getFramesPerBurst(audioStream) * /*nBursts*/2);
        result = AAudioStream_requestStart(audioStream);
        if (result != AAUDIO_OK) {
            __android_log_print(ANDROID_LOG_ERROR, "AudioEngine", "Error starting stream %s",
                                AAudio_convertResultToText(result));
            return;
        }

        MNOGLA_evtSubmitHostToApp(MNOGLA_eKeyToHost::AUDIO_START, /*nArgs*/2, (int32_t)nChan, (int32_t)sampleRate);
        host_logI("Audio started (%i samples per second)", sampleRate);
    }

    static void stop() {
        if (audioStream != nullptr) {
            AAudioStream_requestStop(audioStream);
            AAudioStream_close(audioStream);
        }
    }

    static void restart() {
        host_logI("restarting audio");
        static std::mutex restartingLock;
        if (restartingLock.try_lock()) {
            stop();
            start();
            MNOGLA_evtSubmitHostToApp(MNOGLA_eKeyToHost::AUDIO_RESTART, /*nArgs*/0);
            restartingLock.unlock();
        }
    }
};

AAudioStream *audio::audioStream;

#else // if (no) audio

class audio {
public:
    static void start() {}
};

#endif
// =========================================================================================
// called from Java
// =========================================================================================
extern "C" JNIEXPORT void JNICALL Java_com_android_MNOGLAJNI_MNOGLALIB_init(JNIEnv * /*env*/,
                                                                            jclass,
                                                                            jint width,
                                                                            jint height) {
    audio::start();
    MNOGLA_init(width, height, host_logI, host_logE);
}

extern "C" JNIEXPORT void JNICALL Java_com_android_MNOGLAJNI_MNOGLALIB_render(JNIEnv *, jclass) {
    MNOGLA_render();
}

extern "C" JNIEXPORT void JNICALL
Java_com_android_MNOGLAJNI_MNOGLALIB_evt2(JNIEnv *, jclass, int32_t key, int32_t v1, int32_t v2) {
    MNOGLA_evtSubmitHostToApp(key, /*nArgs*/2, v1, v2);
}

extern "C" JNIEXPORT void JNICALL
Java_com_android_MNOGLAJNI_MNOGLALIB_evt3(JNIEnv *, jclass, int32_t key, int32_t v1, int32_t v2,
                                          int32_t v3) {
    MNOGLA_evtSubmitHostToApp(key, /*nArgs*/3, v1, v2, v3);
}

