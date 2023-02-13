#include "../MNOGLA.h"

#include <cassert>
#include <chrono>
#include <cstdarg>
#include <mutex>
#include <stdexcept>
#include <vector>

#include "../core/MNOGLA_includeGl.h"
#include "MNOGLA_util.h"
using std::runtime_error;
#include <iostream>
namespace MNOGLA {
static std::vector<int32_t> evtQueue;
static std::mutex m;
static size_t readPtr = 0;
logFun_t logI = nullptr;
logFun_t logE = nullptr;
std::chrono::time_point<std::chrono::high_resolution_clock> appStartTime;
typedef union {
    uint64_t timestamp;
    struct {
        int32_t firstHalf;
        int32_t secondHalf;
    } split;
} timestamp_t;

void evtSubmitHostToApp(int32_t key, size_t nArgs, ...) {
    va_list args;
    va_start(args, /*after*/ nArgs);
    std::lock_guard<std::mutex> lk(m);
    evtQueue.push_back((int32_t)nArgs + 2);  // +1 for size, key each
    evtQueue.push_back(key);
    for (size_t ixArg = 0; ixArg < nArgs; ++ixArg)
        evtQueue.push_back(va_arg(args, int32_t));
    va_end(args);
}

void evtTimestampedSubmitHostToApp(int32_t key, size_t nArgs, ...) {
    ::std::chrono::high_resolution_clock::time_point now = ::std::chrono::high_resolution_clock::now();

    timestamp_t ts;
    ts.timestamp = (uint64_t)::std::chrono::duration_cast<std::chrono::nanoseconds>(now - appStartTime).count();

    va_list args;
    va_start(args, /*after*/ nArgs);
    std::lock_guard<std::mutex> lk(m);

    // === time stamp event ===
    evtQueue.push_back((int32_t)4);  // size, key, LSB, MSB
    evtQueue.push_back(eKeyToHost::TIMESTAMP);
    evtQueue.push_back(ts.split.firstHalf);
    evtQueue.push_back(ts.split.secondHalf);

    // === payload event ===
    evtQueue.push_back((int32_t)nArgs + 2);  // +1 for size, key each
    evtQueue.push_back(key);
    for (size_t ixArg = 0; ixArg < nArgs; ++ixArg)
        evtQueue.push_back(va_arg(args, int32_t));
    va_end(args);
}

void timestampSubmitHostToApp() {
    ::std::chrono::high_resolution_clock::time_point now = ::std::chrono::high_resolution_clock::now();

    timestamp_t ts;
    ts.timestamp = (uint64_t)::std::chrono::duration_cast<std::chrono::nanoseconds>(now - appStartTime).count();

    std::lock_guard<std::mutex> lk(m);
    evtQueue.push_back((int32_t)4);  // size, key, LSB, MSB
    evtQueue.push_back(eKeyToHost::TIMESTAMP);
    evtQueue.push_back(ts.split.firstHalf);
    evtQueue.push_back(ts.split.secondHalf);
}

static size_t evtGetHostToApp_inner(int32_t* dest) {
    std::lock_guard<std::mutex> lk(m);
    if (readPtr == evtQueue.size()) {
        evtQueue.clear();
        readPtr = 0;
        return 0;
    }
    size_t n = evtQueue[readPtr++];
    assert(n > 0);

    for (size_t ix = 1; ix < n; ++ix)
        *(dest++) = evtQueue[readPtr++];

    return n - 1;
}

size_t evtGetHostToApp(int32_t* dest) {
    while (true) {
        size_t n = evtGetHostToApp_inner(dest);

        // === peak timestamp event ===
        if (n != 3) return n;
        if (*dest != eKeyToHost::TIMESTAMP) return n;

        // === handle timestamp event centrally ===
        timestamp_t ts;
        ts.split.firstHalf = *(dest + 1);
        ts.split.secondHalf = *(dest + 2);
        lastTimestamp_nanosecs = ts.timestamp;
    }
}

void coreInit(logFun_t _logI, logFun_t _logE) {
    lastTimestamp_nanosecs = 0;  // delta to appStartTime
    logI = _logI;
    logE = _logE;

#ifdef MNOGLA_WINDOWS
    // Windows version uses GLEW to load openGl libraries but this requires initialization
    // for dynamic loading
    glewExperimental = 1;  // Needed for core profile
    if (glewInit() != GLEW_OK) throw runtime_error("Failed to initialize GLEW");
    glfwWindowHint(GLFW_SAMPLES, 4);
#endif
    initUtil();

    // clock starts after initialization is done
    appStartTime = std::chrono::high_resolution_clock::now();
}
void coreDeinit() {
    deinitUtil();
}

uint64_t lastTimestamp_nanosecs;
}  // namespace MNOGLA