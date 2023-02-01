#include "MNOGLA.h"
#include <vector>
#include <mutex>
#include <cassert>
#include <cstdarg>

static std::vector<int32_t> evtQueue;
static std::mutex m;
static size_t readPtr = 0;

extern void MNOGLA_evtSubmitHostToApp(int32_t key, size_t nArgs, ...){
  va_list args;
  va_start(args, /*after*/nArgs);
  std::lock_guard<std::mutex> lk(m);
  evtQueue.push_back((int32_t)nArgs+2); // +1 for size, key each
  evtQueue.push_back(key);
  for (size_t ixArg = 0; ixArg < nArgs; ++ixArg)
    evtQueue.push_back(va_arg(args, int32_t));
  va_end(args);
}

extern size_t MNOGLA_evtGetHostToApp(int32_t* dest){
  std::lock_guard<std::mutex> lk(m);
  if (readPtr == evtQueue.size()){
    evtQueue.clear();
    readPtr = 0;
    return 0;
  }
  size_t n = evtQueue[readPtr++];
  assert(n > 0);
  for (size_t ix = 1; ix < n; ++ix)
    *(dest++) = evtQueue[readPtr++];
  return n-1;
}
