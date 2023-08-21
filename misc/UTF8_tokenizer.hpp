#pragma once
#include <string>
static bool UTF8_tokenizer(std::string::const_iterator& iBegin, const std::string::const_iterator iEnd, uint32_t& outUTF32) {
    if (iBegin == iEnd) return false;
    uint32_t c0 = *(iBegin++);
    if ((c0 & uint32_t(0b10000000)) == 0) {
        outUTF32 = c0;
        return true;
    }

    if (iBegin == iEnd) return false;
    uint32_t c1 = *(iBegin++);
    if ((c1 & 0b11100000) == 0b11000000) {
        // 2 byte code point
        outUTF32 = (c0 & 0b00011111) << 6 | (c1 & 0b00111111);
        return true;
    }

    if (iBegin == iEnd) return false;
    uint32_t c2 = *(iBegin++);
    if ((c2 & 0b11110000) == 0b11100000) {
        // 3 byte code point
        outUTF32 = (c0 & 0b00001111) << 12 | (c1 & 0b00111111) << 6 | (c2 & 0b00111111);
        return true;
    }

    if (iBegin == iEnd) return false;
    uint32_t c3 = *(iBegin++);
    // 4 byte code point
    outUTF32 = (c0 & 0b00000111) << 18 | (c1 & 0b00111111) << 12 | (c2 & 0b00111111) << 6 | (c3 & 0b00111111);
    return true;
}
