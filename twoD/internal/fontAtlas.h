#pragma once
#include <cassert>
#include <iostream>
#include <memory>
#include <vector>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../../3rdPartyLicense/stb_image_write.h"
#include "../../MNOGLA.h"  // freetype
namespace MNOGLA {
using ::std::vector, ::std::shared_ptr, ::std::make_shared, ::std::runtime_error;
class fontAtlas {
   public:
    fontAtlas(FT_Face& face, size_t fontHeightRes) : width(0), height(0) {
        const ::std::string charsToLoad_UTF8(" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~ää§§©«»®°±¹²³µ€äöüÄÖÜßमतलब हिंदी में");
        FT_Set_Pixel_Sizes(face, 0, fontHeightRes);
        vector<shared_ptr<glyph>> glyphs;
        vector<uint32_t> charsToLoad_UTF32 = utf8_to_utf32(charsToLoad_UTF8);
        for (auto c : charsToLoad_UTF32)
            glyphs.push_back(make_shared<glyph>(face, c));
        std::sort(glyphs.begin(), glyphs.end(), [](const auto& a, const auto& b) { return a->getAtlasHeight() > b->getAtlasHeight(); });

        size_t nHorGlyphsOpt = 0;
        size_t nHorGlyphsOptVal = 0;

        // === try all bitmap widths for nHorGlyphs contiguous horizontal glyphs in first row ===
        for (size_t nHorGlyphs = 1; nHorGlyphs < glyphs.size(); ++nHorGlyphs) {
            size_t optVal = tryPlaceGlyphs(glyphs, nHorGlyphs, nullptr, nullptr);
            if (!nHorGlyphsOpt || (optVal < nHorGlyphsOptVal)) {
                nHorGlyphsOpt = nHorGlyphs;
                nHorGlyphsOptVal = optVal;
            }
        }

        // === apply the best result ===
        // fills in atlas location for each glyph
        // size_t area =
        tryPlaceGlyphs(glyphs, nHorGlyphsOpt, &width, &height);
        // std::cout << "final area: " << area << " with " << nHorGlyphsOpt << " glyphs in first row" << std::endl;
        // std::cout << "Atlas size " << width << " x " << height << std::endl;

        // === allocate atlas bitmap ===
        bitmap = new uint8_t[width * height];
        memset((void*)bitmap, /*val*/ 0, width * height * sizeof(uint8_t));
        for (const auto& g : glyphs) {
            copyGlyphToAtlas(g);
            g->freeBitmapTmp();
        }
        //stbi_write_png("fontatlas.png", width, height, /*channels*/ 1, (void*)bitmap, /*stride_bytes*/ 0);
    }
    ~fontAtlas() {
        delete[] bitmap;
        bitmap = nullptr;
    }
    void getBitmap(uint8_t** pBitmap, size_t* pWidth, size_t* pHeight) {
        *pBitmap = bitmap;
        *pWidth = width;
        *pHeight = height;
    }

   protected:
    fontAtlas(const fontAtlas&) = delete;
    int operator=(const fontAtlas&) = delete;

    // one glyph in the font atlas
    class glyph {
       public:
        glyph(FT_Face& face, uint32_t charcode) : charcode(charcode) {
            FT_UInt glyph_index = FT_Get_Char_Index(face, charcode);
            if (!glyph_index) throw runtime_error("missing glyph number " + std::to_string(charcode));
            if (FT_Load_Char(face, charcode, FT_LOAD_RENDER | FT_LOAD_FORCE_AUTOHINT | FT_LOAD_TARGET_LIGHT)) throw runtime_error("failed to load glyph");
            atlasWidth = face->glyph->bitmap.width;
            atlasHeight = face->glyph->bitmap.rows;
            xAdvance = ((float)face->glyph->advance.x) / 64.0f;
            bitmapTmp = new uint8_t[atlasWidth * atlasHeight];
            for (size_t c = 0; c < atlasWidth * atlasHeight; ++c)
                *(bitmapTmp + c) = *(face->glyph->bitmap.buffer + c);
        }

        size_t getAtlasX1() const { return atlasPosX; }
        size_t getAtlasX2() const { return atlasPosX + atlasWidth; }
        size_t getAtlasWidth() const { return atlasWidth; }
        size_t getAtlasY1() const { return atlasPosY; }
        size_t getAtlasY2() const { return atlasPosY + atlasHeight; }
        size_t getAtlasHeight() const { return atlasHeight; }
        void setAtlasPosX(size_t atlasPosX) { this->atlasPosX = atlasPosX; }
        void setAtlasPosY(size_t atlasPosY) { this->atlasPosY = atlasPosY; }
        const uint8_t* getBitmapTmp() { return bitmapTmp; }
        void freeBitmapTmp() {
            assert(bitmapTmp);
            delete[] bitmapTmp;
            bitmapTmp = nullptr;
        }

        bool done = false;

       protected:
        uint32_t charcode;
        size_t atlasPosX = 0, atlasPosY = 0, atlasWidth = 0, atlasHeight = 0;  // position in atlas
        size_t offsetX = 0, offsetY = 0;                                       // location of bitmap relative to origin
        float xAdvance = 0;
        uint8_t* bitmapTmp = nullptr;  // bitmap data, to be free()d once placed in atlas
    };

    // used during construction of font atlas
    class glyphRow {
       public:
        glyphRow(::std::shared_ptr<glyphRow> prevRow)
            : prevRow(prevRow), glyphs() {}

        bool addGlyph(::std::shared_ptr<glyph> g, bool rightToLeft, size_t fixedBitmapWidth) {
            size_t xStart;
            size_t xEnd;
            if (!rightToLeft) {
                xStart = glyphs.size() ? glyphs.back()->getAtlasX2() : 0;
                xEnd = xStart + g->getAtlasWidth();
                if ((fixedBitmapWidth > 0) && (xEnd > fixedBitmapWidth)) return false;  // no space
            } else {
                assert(fixedBitmapWidth && "fixedBitmapWidth is required for right-to-left glyph packing order");
                xEnd = glyphs.size() ? glyphs.back()->getAtlasX1() : fixedBitmapWidth;
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

        // atlas is free between xBegin and xEnd starting at returned value
        size_t getMinY(size_t xBegin, size_t xEnd) const {
            size_t minY = 0;  // start of texture
            for (const auto& g : glyphs)
                if ((g->getAtlasX1() < xEnd) && (g->getAtlasX2() > xBegin))
                    minY = std::max(minY, g->getAtlasY2());
            if ((minY == 0) && (prevRow != nullptr))
                minY = prevRow->getMinY(xBegin, xEnd);
            return minY;
        }
        size_t getNGlyphs() const { return glyphs.size(); }

       protected:
        ::std::shared_ptr<glyphRow> prevRow;
        ::std::vector<::std::shared_ptr<glyph>> glyphs;
    };

    // places glyphs sorted by height in an atlas with nHorGlyphs in the first row. Returns area in pixels.
    static size_t tryPlaceGlyphs(const ::std::vector<::std::shared_ptr<glyph>>& glyphs, size_t nHorGlyphs, size_t* pWidth, size_t* pHeight) {
        assert(nHorGlyphs > 0);
        size_t minBitmapWidth = 0;

        // Algorithm (heuristic in many ways, as the bin-packing problem is NP-hard)
        // [1] In the first row, place a given number of glyphs in order of descending height.
        // [2] The first row determines the width of the bitmap
        // [3] Following rows are filled alternating right-to-left and left-to-right to somewhat compensate the diagonal-ish shape of rows from sorting
        // [4] Glyphs are filled in order of descending height as far as possible
        // [5] If the next-highest glyph does not fit, all smaller glyphs are tried
        // [6] If no glyph fits into the gap, a new row is created.
        // [7] For an new row, placement continues with the next-largest unplaced glyph.
        // [8] For simplicity, bitmap width and height are constantly tracked with every placed glyph. This implies [2].
        // [9] The algorithm finishes, once the known number of glyphs has been placed.

        size_t bitmapWidth = 0;
        size_t bitmapHeight = 0;
        for (auto& g : glyphs) {
            minBitmapWidth = std::max(minBitmapWidth, g->getAtlasHeight());  // [2]
            bitmapHeight = std::max(bitmapHeight, g->getAtlasHeight());
            g->done = false;  // reset all glyphs, as the algorithm may be run repeatedly
        }
        bitmapWidth = minBitmapWidth;
        bool rightToLeft = false;
        std::shared_ptr<glyphRow> row = std::make_shared<glyphRow>(/*prevRow*/ nullptr);
        size_t nGlyphsRemaining = glyphs.size();
        bool newRow = false;
        auto itGlyph = glyphs.begin();
        bool restartAtLargestGlyph = false;
        while (nGlyphsRemaining) {  // [9]
            if (/* [7] */ restartAtLargestGlyph || /* [5] */ (itGlyph == glyphs.end()))
                itGlyph = glyphs.begin();
            while (true) {
                if (!(*itGlyph)->done)
                    break;
                ++itGlyph;
                if (itGlyph == glyphs.end()) {  // [6]
                    itGlyph = glyphs.begin();
                    // note: The following condition is guaranteed because of [7]
                    if (!row->getNGlyphs()) throw runtime_error("font atlas generator internal error: failed to place glyph into empty row. " + std::to_string(nGlyphsRemaining) + " glyphs remaining.");
                    newRow = true;
                }
            }  // while searching for new glyph

            if (nHorGlyphs && (nHorGlyphs == row->getNGlyphs())) {  // [1]
                newRow = true;
                nHorGlyphs = 0;
            }

            if (newRow) {
                rightToLeft = !rightToLeft;  // [2]
                row = std::make_shared<glyphRow>(row);
                newRow = false;
            }

            bool success = row->addGlyph(*itGlyph, rightToLeft, nHorGlyphs ? 0 : bitmapWidth);
            if (success) {
                bitmapWidth = std::max(bitmapWidth, (*itGlyph)->getAtlasX2());    // [8]
                bitmapHeight = std::max(bitmapHeight, (*itGlyph)->getAtlasY2());  // [8]
                --nGlyphsRemaining;
                (*itGlyph)->done = true;
            } else {
                newRow = true;
                restartAtLargestGlyph = true;  // [7]
            }
        }

        // std::cout << "Atlas size " << bitmapWidth << " x " << bitmapHeight << std::endl;

        // === return result ===
        if (pWidth) *pWidth = bitmapWidth;
        if (pHeight) *pHeight = bitmapHeight;
        // return bitmapWidth * bitmapHeight; // optimize for area
        return std::max(bitmapWidth, bitmapHeight); // optimize largest dimension (for GL_MAX_TEXTURE_SIZE limit)
    }

    // copies one glyph to its location in the fontAtlas
    void copyGlyphToAtlas(const ::std::shared_ptr<glyph> g) {
        uint8_t* const pAtlasTopLeft = bitmap + g->getAtlasY1() * width + g->getAtlasX1();
        const uint8_t* pGlyph = g->getBitmapTmp();
        const size_t cAtlasWidth = width;
        const size_t glyphWidth = g->getAtlasWidth();
        const size_t glyphHeight = g->getAtlasHeight();
        assert(g->getAtlasX2() <= width);
        assert(g->getAtlasY2() <= height);
        for (size_t row = 0; row < glyphHeight; ++row)
            for (size_t col = 0; col < glyphWidth; ++col)
                *(pAtlasTopLeft + row * cAtlasWidth + col) = *(pGlyph++);
    }

    static vector<uint32_t> utf8_to_utf32(const string& text) {
        vector<uint32_t> r;
        const size_t s = text.size();
        for (size_t ix = 0; ix < s;) {
            if ((text[ix] & 0b10000000) == 0) {
                // ASCII
                r.push_back(text[ix]);
                ix += 1;
            } else if ((ix + 1 < s) && (text[ix] & 0b11100000) == 0b11000000) {
                // 2 byte code point
                r.push_back((uint32_t)(text[ix] & 0b00011111) << 6 | (text[ix + 1] & 0b00111111));
                ix += 2;
            } else if ((ix + 2 < s) && (text[ix] & 0b11110000) == 0b11100000) {
                // 3 byte code point
                r.push_back((uint32_t)(text[ix] & 0b00001111) << 12 | (text[ix + 1] & 0b00111111) << 6 | (text[ix + 2] & 0b00111111));
                ix += 3;
            } else if (ix + 3 < s) {
                // 4 byte code point
                r.push_back((uint32_t)(text[ix] & 0b00000111) << 18 | (text[ix + 1] & 0b00111111) << 12 | (text[ix + 2] & 0b00111111) << 6 | (text[ix + 3] & 0b00111111));
                ix += 4;
            } else {
                break;  // incomplete code, ignore and quit
            }
        }

        return r;
    }

    // === data ===
    // atlas bitmap data
    uint8_t* bitmap = nullptr;
    // atlas bitmap width
    size_t width;
    // atlas bitmap height
    size_t height;
};
}  // namespace MNOGLA