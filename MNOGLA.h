#pragma once
#include <cstddef>  // std::size_t on Android
#include <cstdint>
using std::size_t;

// === user code must implement those: ===
// "extern C" linkage and global namespace to improve compatibility (e.g. userApp may use plain C and build on a different compiler)

//* host calls userApp at startup */
extern "C" void MNOGLA_userInit(int w, int h);
//* host calls userApp to render new video frame (thread 0) */
extern "C" void MNOGLA_videoCbT0();
//* host calls userApp to request generation of an audio buffer */
extern "C" void MNOGLA_audioCbT1(float* audioBuf, int32_t numFrames);
//* host calls userApp to report incoming MIDI event */
extern "C" void MNOGLA_midiCbT2(int32_t v0, int32_t v1, int32_t v2);

namespace MNOGLA {

// logging function signature (function to be provided by the host)
typedef void (*logFun_t)(const char* format, ...);
//* host-provided logging functions are accessible in those global variables */
extern logFun_t logI;
extern logFun_t logE;

// host calls core to initialize
void coreInit(logFun_t logI, logFun_t logE);

// host signals an event
void evtSubmitHostToApp(int32_t key, size_t nArgs, ...);

// userApp gets event
size_t evtGetHostToApp(int32_t* dest);

// ############################################################
// host-to-app events
// ############################################################
// WINSIZE: window resize
// - width in pixels
// - height in pixels
//
// PTR: touchscreen pointer
// - pointer index  (multitouch)
// - x coordinate in pixels
// - y coordinate in pixels
// A mouse reports as PTR_MOVE with index==0 (no DOWN/UP events)
//
// BTNDOWN, BTNUP Mouse button down, up
// - button number (left, right, mid, ...)
//
// SCROLL: Mouse wheel
// - dx (-1, 0, 1)
// - dy (-1, 0, 1)
//
// AUDIO_START:
// - nChannels
// - samplerate, Hz
//
// AUDIO_RESTART: (e.g. on bluetooth headset disconnect, no arguments)

class eKeyToHost {
   public:
    enum {
        INV_NULL = 0,
        WINSIZE = 1000,
        PTR_DOWN = 200,
        PTR_UP = 201,
        PTR_MOVE = 202,
        BTNDOWN = 300,
        BTNUP = 301,
        SCROLL = 400,
        AUDIO_START = 10000,
        AUDIO_RESTART = 10001
    };
};
}  // namespace MNOGLA