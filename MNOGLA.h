#pragma once
#include <cstdint>
#include <cstddef> // std::size_t on Android
using std::size_t;

// file has two main roles:
//  - platform-independent interface to openGL host
//      - MNOGLA_xyz functions
//      - message protocol
//  - means to load openGL headers

// ################################################################
// interface: function prototypes of MNOGLA_xyz()
// ################################################################

// logging function signature (function to be provided by the host)
typedef void (*logFun_t)(const char* format, ...);

// host calls to initialize
extern void MNOGLA_init(int w, int h, logFun_t logI, logFun_t logE);

// host signals an event
extern void MNOGLA_evtSubmitHostToApp(int32_t key, size_t nArgs, ...);

// userApp gets event
extern size_t MNOGLA_evtGetHostToApp(int32_t* dest);

// host calls to render new video frame (thread 0)
extern void MNOGLA_videoCbT0();

// host calls to generate audio buffer
extern void MNOGLA_audioCbT1(float* audioBuf, int32_t numFrames);

// host calls to report incoming MIDI event
extern void MNOGLA_midiCbT2(int32_t v0, int32_t v1, int32_t v2);

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

class MNOGLA_eKeyToHost {
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
