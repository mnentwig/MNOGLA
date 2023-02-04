#include "../MNOGLA.h"

#include <cassert>
#include <cstdarg>
#include <mutex>
#include <stdexcept>
#include <vector>

#include "../core/MNOGLA_includeGl.h"
#include "MNOGLA_util.h"
using std::runtime_error;

namespace MNOGLA {
static std::vector<int32_t> evtQueue;
static std::mutex m;
static size_t readPtr = 0;
logFun_t logI = nullptr;
logFun_t logE = nullptr;

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

size_t evtGetHostToApp(int32_t* dest) {
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

void coreInit(logFun_t _logI, logFun_t _logE) {
    logI = _logI;
    logE = _logE;

#ifdef MNOGLA_WINDOWS
    // Windows version uses GLEW to load openGl libraries but this requires initialization
    // for dynamic loading
    glewExperimental = 1;  // Needed for core profile
    if (glewInit() != GLEW_OK) throw runtime_error("Failed to initialize GLEW");
#endif
    initUtil();
     
}
} // namespace MNOGLA