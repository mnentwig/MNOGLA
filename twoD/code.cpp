// Purpose of this file:
// pull in various source files per include ===
//  - keeps CMAKE input file simple
//  - enables additional optimizations over independent object files
#include "internal/twoDView.cpp"
#include "internal/twoDShape.cpp"
#include "internal/filledRect.cpp"
#include "internal/outlinedRect.cpp"
#include "internal/vectorText.cpp"
#ifdef MNOGLA_HAS_FREETYPE
#include "internal/textureText.cpp"
#endif
#include "internal/twoDMatrix.cpp"
