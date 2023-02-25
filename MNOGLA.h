#pragma once
#include <cstddef>  // std::size_t on Android
#include <cstdint>
using std::size_t;

// === user code must implement those: ===

//* host calls userApp at startup */
void MNOGLA_userInit(int w, int h);
//* host calls userApp to render new video frame (thread 0) */
void MNOGLA_videoCbT0();
//* host calls userApp to request generation of an audio buffer */
void MNOGLA_audioCbT1(float* audioBuf, int32_t numFrames);
//* host calls userApp to report incoming MIDI event */
void MNOGLA_midiCbT2(int32_t v0, int32_t v1, int32_t v2);

namespace MNOGLA {

// logging function signature (function to be provided by the host)
typedef void (*logFun_t)(const char* format, ...);
//* host-provided logging functions are accessible in those global variables */
extern logFun_t logI;
extern logFun_t logE;

// host calls core to initialize
void coreInit(logFun_t logI, logFun_t logE);
void coreDeinit();

// host signals an event
void evtSubmitHostToApp(int32_t key, size_t nArgs, ...);

// host signals an event to be sent with a timestamp
void evtTimestampedSubmitHostToApp(int32_t key, size_t nArgs, ...);

// host sends timestamp before MNOGLA_videoCbT0()
void timestampSubmitHostToApp();

// userApp gets event
size_t evtGetHostToApp(int32_t* dest);

// last timestamp received in the event queue
extern uint64_t lastTimestamp_nanosecs;

// ############################################################
// host-to-app events
// ############################################################
// WINSIZE: window resize
// - width in pixels
// - height in pixels
//
// TOUCH_DOWN: touchscreen pointer
// - pointer index  (multitouch)
// - x coordinate in pixels
// - y coordinate in pixels
// 
// TOUCH_MOVE: touchscreen pointer
// - pointer index  (multitouch)
// - x coordinate in pixels
// - y coordinate in pixels
//
// TOUCH_UP: touchscreen pointer
// - pointer index  (multitouch)
// - nRemaining pointers (e.g. 0 when last finger is removed from the screen)
//   use as guardrail against state inconsistency between OS and app
//   (if 0, app clears all known pointers).
//   negative number: not known (mouse uses this variant)
//   Note: Android host doesn't implement all possible OS event types
//
// MOUSE_DOWN, MOUSE_UP: Mouse button down, up
// - button number (left, right, mid, ...)
//
// MOUSE_SCROLL: Mouse wheel
// - dx (-1, 0, 1)
// - dy (-1, 0, 1)
//
// AUDIO_START:
// - nChannels
// - samplerate, Hz
//
// AUDIO_RESTART: (e.g. on bluetooth headset disconnect, no arguments)
//
// TIMESTAMP:
// the number of nanoseconds since application start
// - first half of the uint64_t value
// - second half of the uint64_t value (note: endianness is left to implementation)
class eKeyToHost {
   public:
    enum {
        INV_NULL = 0,
        TOUCH_DOWN = 200,
        TOUCH_UP = 201,
        TOUCH_MOVE = 202,
        MOUSE_DOWN = 300,
        MOUSE_UP = 301,
        MOUSE_MOVE = 302,
        MOUSE_SCROLL = 400,
        WINSIZE = 1000,
        TIMESTAMP = 2000,
        AUDIO_START = 10000,
        AUDIO_RESTART = 10001
    };
};
}  // namespace MNOGLA