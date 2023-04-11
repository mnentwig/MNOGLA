#include "fontAtlas.h"

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <vector>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../../3rdPartyLicense/stb_image_write.h"

namespace MNOGLA {
using std::runtime_error, std::shared_ptr, std::make_shared, std::vector;

// ===========================================================
// fontAtlas
// ===========================================================
fontAtlas::fontAtlas(FT_Face& face) : width(0), height(0) {
    vector<shared_ptr<glyph>> glyphs;
    for (uint8_t c = 32; c < 128; ++c)
        glyphs.push_back(make_shared<glyph>(face, c));
    std::sort(glyphs.begin(), glyphs.end(), [](const auto& a, const auto& b) { return a->getAtlasHeight() > b->getAtlasHeight(); });

    for (const auto& g : glyphs)
        std::cout << g->getAtlasHeight() << std::endl;
    size_t nHorGlyphsOpt = 0;
    size_t nHorGlyphsOptArea = 0;

    // === try all bitmap widths for nHorGlyphs contiguous horizontal glyphs in first row ===
    for (size_t nHorGlyphs = 1; nHorGlyphs < glyphs.size(); ++nHorGlyphs) {
        size_t area = tryPlaceGlyphs(glyphs, nHorGlyphs, nullptr, nullptr);
        if (!nHorGlyphsOpt || (area < nHorGlyphsOptArea)) {
            nHorGlyphsOpt = nHorGlyphs;
            nHorGlyphsOptArea = area;
        }
    }

    // === apply the best result ===
    // fills in atlas location for each glyph
    size_t area = tryPlaceGlyphs(glyphs, nHorGlyphsOpt, &width, &height);
    std::cout << "final area: " << area << " with " << nHorGlyphsOpt << " glyphs in first row" << std::endl;
    std::cout << "Atlas size " << width << " x " << height << std::endl;
    bitmap = new uint8_t[width * height];
    memset((void*)bitmap, /*val*/ 0, width * height * sizeof(uint8_t));
    for (const auto& g : glyphs) {
        copyGlyphToAtlas(g);
        g->freeBitmapTmp();
//        break;
    }

    stbi_write_png("fontatlas.png", width, height, /*channels*/ 1, (void*)bitmap, /*stride_bytes*/ 0);
    exit(0);
}

// ===========================================================
// fontAtlas::glyph
// ===========================================================
fontAtlas::glyph::glyph(FT_Face& face, uint8_t ASCII_charNum) : ASCII_charNum(ASCII_charNum) {
    if (FT_Load_Char(face, ASCII_charNum, FT_LOAD_RENDER | FT_LOAD_FORCE_AUTOHINT | FT_LOAD_TARGET_LIGHT)) throw runtime_error("missing glyph");
    atlasWidth = face->glyph->bitmap.width;
    atlasHeight = face->glyph->bitmap.rows;
    xAdvance = ((float)face->glyph->advance.x) / 64.0f;
    bitmapTmp = new uint8_t[atlasWidth * atlasHeight];
    for (size_t c = 0; c < atlasWidth * atlasHeight; ++c)
        *(bitmapTmp + c) = *(face->glyph->bitmap.buffer + c);
}

void fontAtlas::glyph::freeBitmapTmp() {
    assert(bitmapTmp);
    delete[] bitmapTmp;
    bitmapTmp = nullptr;
}

// ===========================================================
// fontAtlas::glyphRow
// ===========================================================
fontAtlas::glyphRow::glyphRow(shared_ptr<glyphRow> prevRow, bool rightToLeft, size_t nGlyphs, size_t minBitmapWidth)
    : prevRow(prevRow), rightToLeft(rightToLeft), nGlyphs(nGlyphs), bitmapWidth(minBitmapWidth), glyphs() {}

bool fontAtlas::glyphRow::addGlyph(shared_ptr<glyph> g) {
    if ((nGlyphs > 0) && (nGlyphs == glyphs.size())) {
        return false;  // target glyph count reached (first row)
    }

    size_t xStart;
    size_t xEnd;
    if (!rightToLeft) {
        xStart = glyphs.size() ? glyphs.back()->getAtlasX2() : 0;
        xEnd = xStart + g->getAtlasWidth();
        if (/*glyph count test disabled*/ !nGlyphs && (xEnd > bitmapWidth)) return false;  // no space
    } else {
        assert(bitmapWidth && "bitmapWidth needs to be set for right-to-left glyph packing");
        xEnd = glyphs.size() ? glyphs.back()->getAtlasX1() : bitmapWidth;
        if (xEnd < g->getAtlasWidth()) return false;  // no space
        xStart = xEnd - g->getAtlasWidth();
    }
    // === insert glyph in row ===
    glyphs.push_back(g);
    g->setAtlasPosX(xStart);

    // === move up as far as possible ===
    size_t posY = !prevRow ? 0 : prevRow->getMinY(xStart, xEnd);
    g->setAtlasPosY(posY);
    return true;
}

size_t fontAtlas::glyphRow::getMinY(size_t ixBegin, size_t ixEnd) const {
    size_t minY = 0;  // start of texture
    for (const auto& g : glyphs)
        if ((g->getAtlasX1() < ixEnd) && (g->getAtlasX2() > ixBegin))
            minY = std::max(minY, g->getAtlasY2());
    if ((minY == 0) && (prevRow != nullptr))
        minY = prevRow->getMinY(ixBegin, ixEnd);
    return minY;
}

size_t fontAtlas::glyphRow::getMaxY() const {
    size_t bitmapHeight = 0;
    for (const auto& g : glyphs)
        bitmapHeight = std::max(bitmapHeight, g->getAtlasY2());
    std::cout << "getMaxY\t" << bitmapHeight << "\n";
    return bitmapHeight;
}

size_t fontAtlas::glyphRow::getBitmapWidth() const {
    size_t r = bitmapWidth;
    // update bitmapWidth:
    for (const auto& g : glyphs)
        r = std::max(r, g->getAtlasX2());
    std::cout << "getBitmapWidth\t" << r << "\n";

    return r;
}

}  // namespace MNOGLA