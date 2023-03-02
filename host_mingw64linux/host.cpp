// MNOGLA: mn's operating-system independent openGl ES application
// this file: Windows host, to be used with minGW
#include <cstdarg>
#include <stdexcept>
#include <string>
#include <iostream>
#ifdef MNOGLA_HASAUDIO
#include <portaudio.h>

#endif

#include "../MNOGLA.h"
#include "../core/MNOGLA_includeGl.h"
using std::runtime_error, std::string;

#ifdef MNOGLA_HASAUDIO
static int portaudioCallback(const void* /*inputBuffer*/, void* outputBuffer,
                             unsigned long framesPerBuffer,
                             const PaStreamCallbackTimeInfo* /*timeInfo*/,
                             PaStreamCallbackFlags /*statusFlags*/,
                             void* /*userData*/) {
    MNOGLA_audioCbT1((float*)outputBuffer, framesPerBuffer);
    return 0;
}
#endif

static void window_size_callback(GLFWwindow* /*window*/, int width, int height) {
    MNOGLA::evtSubmitHostToApp(MNOGLA::eKeyToHost::WINSIZE, /*nArgs*/ 2, (int32_t)width, (int32_t)height);
}

static void cursor_position_callback(GLFWwindow* /*window*/, double xpos, double ypos) {
    MNOGLA::evtTimestampedSubmitHostToApp(MNOGLA::eKeyToHost::MOUSE_MOVE, /*nArgs*/ 2, (int32_t)xpos, (int32_t)ypos);
}

void scroll_callback(GLFWwindow* /*window*/, double xoffset, double yoffset) {
    int32_t dx = xoffset < 0   ? -1
                 : xoffset > 0 ? 1
                               : 0;
    int32_t dy = yoffset < 0   ? -1
                 : yoffset > 0 ? 1
                               : 0;
    MNOGLA::evtTimestampedSubmitHostToApp(MNOGLA::eKeyToHost::MOUSE_SCROLL, /*nArgs*/ 2, dx, dy);
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    int32_t key = (action == GLFW_PRESS)     ? MNOGLA::eKeyToHost::MOUSE_DOWN
                  : (action == GLFW_RELEASE) ? MNOGLA::eKeyToHost::MOUSE_UP
                                             : MNOGLA::eKeyToHost::INV_NULL;
    if (!key) return;
    MNOGLA::evtTimestampedSubmitHostToApp(key, /*nArgs*/ 1, button);
}

void logI_impl(const char* format, ...) {
    va_list args;
    va_start(args, /*start after*/ format);
    printf("I:");
    vprintf(format, args);
    printf("\n");
    fflush(stdout);
    va_end(args);
}

void logE_impl(const char* format, ...) {
    va_list args;
    va_start(args, /*start after*/ format);
    printf("E:");
    vprintf(format, args);
    printf("\n");
    fflush(stdout);
    va_end(args);
}

int main(void) {
    if (!glfwInit()) throw runtime_error("Failed to initialize GLFW");

    // Android: <uses-feature android:glEsVersion="0x00030001" android:required="true" />
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
#ifdef SINGLE_BUFFER
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_FALSE);
#else
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
#endif
    glfwWindowHint(GLFW_SAMPLES, 4);
    int winWidth;
    int winHeight;
#if 1
    winWidth = 640;
    winHeight = 480;
    GLFWwindow* window = glfwCreateWindow(winWidth, winHeight, __FILE__, NULL, NULL);
#else
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
    GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, __FILE__, monitor, NULL);
    winWidth = mode->width;
    winHeight = mode->height;
#endif
    glfwMakeContextCurrent(window);
    if (1 || glfwExtensionSupported("WGL_EXT_swap_control_tear") || glfwExtensionSupported("GLX_EXT_swap_control_tear")) {
        glfwSwapInterval(0);
    }
    glfwSetWindowSizeCallback(window, window_size_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);
    //    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    MNOGLA::coreInit(logI_impl, logE_impl);
    MNOGLA_userInit(winWidth, winHeight);
#ifdef MNOGLA_HASAUDIO
    // =============================================
    // Audio
    // =============================================
    PaError paErr = Pa_Initialize();
    if (paErr != paNoError) throw runtime_error(string("failed to initialize portAudio: ") + Pa_GetErrorText(paErr));

    PaStream* stream;
    paErr = Pa_OpenDefaultStream(&stream,
                                 /*nChanIn*/ 0,
                                 /*nChanOut*/ 2,
                                 /*format*/ paFloat32,
                                 /*rate*/ 48000,
                                 /*frames per buf*/ 256,
                                 portaudioCallback,
                                 /*userData*/ NULL);
    if (paErr != paNoError) throw runtime_error(string("failed to open portAudio stream:") + Pa_GetErrorText(paErr));
    paErr = Pa_StartStream(stream);
    if (paErr != paNoError) throw runtime_error(string("failed to start portAudio stream:") + Pa_GetErrorText(paErr));
#endif

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        if (glfwWindowShouldClose(window))
            break;

        MNOGLA::timestampSubmitHostToApp();
        MNOGLA_videoCbT0();
        // === show new image ===
#ifdef SINGLE_BUFFER
        glFlush();
#else
        glfwSwapBuffers(window);
#endif
    }
#ifdef MNOGLA_HASAUDIO
    paErr = Pa_StopStream(stream);
    if (paErr != paNoError) throw runtime_error(string("failed to stop portAudio stream:") + Pa_GetErrorText(paErr));
    paErr = Pa_Terminate();
    if (paErr != paNoError) throw runtime_error(string("failed to terminate portAudio:") + Pa_GetErrorText(paErr));
#endif

    MNOGLA::coreDeinit();

    glfwTerminate();

    return 0;
}
