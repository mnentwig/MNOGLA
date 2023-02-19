// === pull in various source files per include ===
//  - keeps CMAKE input file simple
//  - enables additional optimizations over independent object files
//  - as we don't have headers:
//      - everything must be included at the same time
//      - dependencies must form a tree
#include "internal/twoDView.cpp"
#include "internal/filledRect.cpp"
#include "internal/outlinedRect.cpp"
#include "internal/twoDShape.cpp"
#include "internal/vectorText.cpp"
