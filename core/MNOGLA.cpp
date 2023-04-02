// #define MNOGLA_HAS_FREETYPE
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
#include <cstdio>  // FILE
#include <filesystem>
#include <iostream>
namespace MNOGLA {
static std::vector<int32_t> evtQueue;
static std::mutex m;
static size_t readPtr = 0;
logFun_t logI = nullptr;
logFun_t logE = nullptr;
#ifdef MNOGLA_HAS_FREETYPE
FT_Library freetypeLib;
FT_Face freetypeDefaultFace;
static char* freetypeDefaultFontdata;
#endif

fopenAsset_t fopenAsset = nullptr;
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

bool loadAsset(const char* fname, char** data, size_t* nBytes) {
    FILE* h;
    if (MNOGLA::mainArg0 == nullptr) {
        h = (FILE*)fopenAsset(fname, "rb");  // android
    } else {
        // resource files are expected to be copied into build folder, where the executable resides
        // e.g. use cmake "configure_file(../../mySourceFolder/mySource.ods myDest.ods COPYONLY)"
        ::std::filesystem::path p(MNOGLA::mainArg0);
        p.replace_filename(fname);
        h = (FILE*)fopenAsset(p.string().c_str(), "rb");
    }
    if (!h) return false;
    char buf[65536];
    *data = nullptr;
    *nBytes = 0;
    while (true) {
        size_t nChunk = fread(buf, 1, sizeof(buf), h);
        *data = (char*)realloc(*data, *nBytes + ::std::max(nChunk, (size_t)1));  // empty file returns 1 byte dummy buffer (realloc does not permit zero size)
        memcpy(*data + *nBytes, buf, nChunk);
        *nBytes += nChunk;
        if (!nChunk) return true;
    }
}

void coreInit(logFun_t _logI, logFun_t _logE, fopenAsset_t _fopenAsset) {
    lastTimestamp_nanosecs = 0;  // delta to appStartTime
    logI = _logI;
    logE = _logE;
    fopenAsset = _fopenAsset;
    logI("core init");

#ifdef MNOGLA_HAS_FREETYPE
    if (FT_Init_FreeType(&freetypeLib)) throw runtime_error("FT_Init_FreeType failed");
    size_t nFontBytes;
    if (!loadAsset("NotoSans-Regular.ttf", &freetypeDefaultFontdata, &nFontBytes))
        throw runtime_error("failed to load default font file");
    if (FT_New_Memory_Face(freetypeLib, (FT_Byte*)freetypeDefaultFontdata, nFontBytes, 0, &freetypeDefaultFace))
        throw runtime_error("failed to load default font face");
#endif

#ifdef MNOGLA_WINDOWS
    // Windows version uses GLEW to load openGl libraries but this requires initialization
    // for dynamic loading
    glewExperimental = 1;  // Needed for core profile
    if (glewInit() != GLEW_OK) throw runtime_error("Failed to initialize GLEW");
    glfwWindowHint(GLFW_SAMPLES, 4);
#endif

    // clock starts after initialization is done
    appStartTime = std::chrono::high_resolution_clock::now();
    util_init();
}

static ::std::vector<void (*)()> glInitFuns;
static ::std::vector<void (*)()> glDeinitFuns;

void coreInitGlContext() {
    // Note: In case of GL context loss, this will be called repeatedly. No need to glDelete() anything.
    for (const auto& f : glInitFuns)
        f();
}

void coreDeinit() {
    for (const auto& f : glDeinitFuns)
        f();
    util_deinit();
#ifdef MNOGLA_HAS_FREETYPE
    FT_Done_Face(freetypeDefaultFace);
    free(freetypeDefaultFontdata); // permitted only after FT_Done_Face
    FT_Done_FreeType(freetypeLib);
#endif
}

uint64_t lastTimestamp_nanosecs;

const char* mainArg0;

// registered functions will be called on coreInitGlContext (to load Gl resources)
void registerGlInit(void (*fun)()) {
    for (const auto f : glInitFuns)
        if (f == fun) throw runtime_error("registerGlInit: function already registered (duplicate)!");
    glInitFuns.push_back(fun);
}

// registered functions will be called on application exit (to unload Gl resources)
void registerGlDeinit(void (*fun)()) {
    for (const auto f : glDeinitFuns)
        if (f == fun) throw runtime_error("registerGlDeinit: function already registered (duplicate)!");
    glDeinitFuns.push_back(fun);
}

}  // namespace MNOGLA