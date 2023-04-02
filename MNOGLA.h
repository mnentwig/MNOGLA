#pragma once
#include <cstddef>  // std::size_t on Android
#include <cstdint>
#include <functional>
#ifdef MNOGLA_HAS_FREETYPE
#include <ft2build.h>
#include FT_FREETYPE_H
#endif

using std::size_t;

// === user code must implement those: ===

//* host calls userApp at startup. Gl context is NOT available. */
void MNOGLA_userInit();
//* host calls userApp once GL context is available and whenever it is re-obtained e.g. app switch in Android */
void MNOGLA_initGlContext();

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
#ifdef MNOGLA_HAS_FREETYPE
extern FT_Library freetypeLib;
extern FT_Face freetypeDefaultFace;
#endif

typedef void* (*fopenAsset_t)(const char* fname, const char* mode);  // cast return value to FILE* (not including cstdio here)
extern fopenAsset_t fopenAsset;
// argv[0] from int main(int argc, const char** argv);
extern const char* mainArg0;

// host calls core to initialize
void coreInit(logFun_t logI, logFun_t logE, fopenAsset_t fopenAsset);

// host calls when a GL context has been (re)obtained
// Note: for repeated callbacks, it means the glContext was lost and re-obtained.
// If so, resources in a lost context do not need to be glDelete()d.
void coreInitGlContext();

// host calls on application exit
void coreDeinit();

// registered functions will be called by coreInitGlContext (to load/reload Gl resources)
void registerGlInit(void (*fun)());

// registered functions will be called on application exit (to unload Gl resources)
void registerGlDeinit(void (*fun)());

// host signals an event
void evtSubmitHostToApp(int32_t key, size_t nArgs, ...);

// host signals an event to be sent with a timestamp
void evtTimestampedSubmitHostToApp(int32_t key, size_t nArgs, ...);

// host sends timestamp before MNOGLA_videoCbT0()
void timestampSubmitHostToApp();

// userApp gets event
size_t evtGetHostToApp(int32_t* dest);

// timestamp of last event retrieved by getEvtHostToApp()
// - for a timestamped event, it is the exact event time
// - render callback sends an own timestamp, which becomes visible after running the event loop empty
extern uint64_t lastTimestamp_nanosecs;

// returns contents of asset file (true = success). Caller free()s *data.
bool loadAsset(const char* fname, char** data, size_t* nBytes);

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
        MOUSE_WHEEL = 400,
        WINSIZE = 1000,
        TIMESTAMP = 2000,
        AUDIO_START = 10000,
        AUDIO_RESTART = 10001
    };
};
}  // namespace MNOGLA
#include <cstdio>
