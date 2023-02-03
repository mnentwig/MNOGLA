// MNOGLA: mn's operating-system independent openGl ES application
// this file: Windows host, to be used with minGW
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <cstdarg>
#include <stdexcept>

#include "../MNOGLA.h"
using std::runtime_error;

static void window_size_callback(GLFWwindow* /*window*/, int width, int height) {
    MNOGLA_evtSubmitHostToApp(MNOGLA_eKeyToHost::WINSIZE, /*nArgs*/2, (int32_t)width, (int32_t)height);
}

static void cursor_position_callback(GLFWwindow* /*window*/, double xpos, double ypos) {
    MNOGLA_evtSubmitHostToApp(MNOGLA_eKeyToHost::PTR_MOVE, /*nArgs*/3, /*ix*/ 0, (int32_t)xpos, (int32_t)ypos);
}

void scroll_callback(GLFWwindow* /*window*/, double xoffset, double yoffset) {
    int32_t dx = xoffset < 0   ? -1
             : xoffset > 0 ? 1
                           : 0;
    int32_t dy = yoffset < 0   ? -1
             : yoffset > 0 ? 1
                           : 0;
    MNOGLA_evtSubmitHostToApp(MNOGLA_eKeyToHost::SCROLL, /*nArgs*/2, dx, dy);
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    int32_t key = (action == GLFW_PRESS)     ? MNOGLA_eKeyToHost::BTNDOWN
                   : (action == GLFW_RELEASE) ? MNOGLA_eKeyToHost::BTNUP
                                              : MNOGLA_eKeyToHost::INV_NULL;
    if (!key) return;
    MNOGLA_evtSubmitHostToApp(key, /*nArgs*/1, button);
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
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    MNOGLA_init(winWidth, winHeight, logI_impl, logE_impl);

    while (true) {
        glfwPollEvents();
        if (glfwWindowShouldClose(window))
            break;

        MNOGLA_videoCbT0();
        // === show new image ===
#ifdef SINGLE_BUFFER
        glFlush();
#else
        glfwSwapBuffers(window);
#endif
    }
    glfwTerminate();

    return 0;
}
