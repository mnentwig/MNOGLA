#pragma once
#include <cassert>
#include <iostream>
#include <memory>
#include <vector>

#include "../../MNOGLA.h"  // freetype
namespace MNOGLA {
class fontAtlas {
   public:
    fontAtlas(FT_Face& face);

   protected:
    // one glyph in the font atlas
    class glyph {
       public:
        glyph(FT_Face& face, uint8_t ASCII_charNum);
        // void getX1X2(size_t& x1, size_t& x2) const;
        size_t getAtlasX1() const { return atlasPosX; }
        size_t getAtlasX2() const { return atlasPosX + atlasWidth; }
        size_t getAtlasWidth() const { return atlasWidth; }
        size_t getAtlasY1() const { return atlasPosY; }
        size_t getAtlasY2() const { return atlasPosY + atlasHeight; }
        size_t getAtlasHeight() const { return atlasHeight; }
        void setAtlasPosX(size_t atlasPosX) { this->atlasPosX = atlasPosX; }
        //        void setAtlasWidth(size_t atlasWidth) { this->atlasWidth = atlasWidth; }
        void setAtlasPosY(size_t atlasPosY) { this->atlasPosY = atlasPosY; }
        uint8_t* getBitmapTmp() { return bitmapTmp; }
        void freeBitmapTmp();

       protected:
        size_t ASCII_charNum;
        size_t atlasPosX = 0, atlasPosY = 0, atlasWidth = 0, atlasHeight = 0;  // position in atlas
        size_t offsetX = 0, offsetY = 0;                                       // location of bitmap relative to origin
        float xAdvance = 0;
        uint8_t* bitmapTmp = nullptr;  // bitmap data, to be free()d once placed in atlas
    };

    // used during construction of font atlas
    class glyphRow {
       public:
        // rightToLeft reverts packing order.
        // Non-zero nGlyphs sets max. number of glyphs.
        // Non-zero bitmapWidth limits/defines combined width.
        glyphRow(::std::shared_ptr<glyphRow> prevRow, bool rightToLeft, size_t nGlyphs, size_t minBitmapWidth);
        bool addGlyph(::std::shared_ptr<glyph> g);
        size_t getMinY(size_t ixBegin, size_t ixEnd) const;
        size_t getMaxY() const;
        size_t getNGlyphs() const;
        size_t getBitmapWidth() const;
        bool getRightToLeft() const { return rightToLeft; }

       protected:
        ::std::shared_ptr<glyphRow> prevRow;
        bool rightToLeft;
        size_t nGlyphs;
        size_t bitmapWidth;
        ::std::vector<::std::shared_ptr<glyph>> glyphs;
    };

    // places glyphs sorted by height in an atlas with nHorGlyphs in the first row. Returns area in pixels.
    static size_t tryPlaceGlyphs(const ::std::vector<::std::shared_ptr<glyph>>& glyphs, size_t nHorGlyphs, size_t* pWidth, size_t* pHeight) {
        std::cout << "tryPlaceGlyphs " << nHorGlyphs << "\n";
        assert(nHorGlyphs > 0);
        auto itGlyph = glyphs.begin();
        bool getNextGlyph = true;
        size_t minBitmapWidth = 0;

        // === determine minimum bitmap width ===
        // (bitmap can't be smaller than the largest glyph)
        size_t bitmapWidth = 0;
        size_t bitmapHeight = 0;
        for (const auto& g : glyphs) {
            minBitmapWidth = std::max(minBitmapWidth, g->getAtlasHeight());
            bitmapHeight = std::max(bitmapHeight, (*itGlyph)->getAtlasHeight());
        }
        bitmapWidth = minBitmapWidth;
        std::shared_ptr<glyphRow> row = std::make_shared<glyphRow>(/*prevRow*/ nullptr, /*rightToLeft*/ false, nHorGlyphs, minBitmapWidth);
        while (true) {
            if (getNextGlyph) {
                ++itGlyph;
                if (itGlyph == glyphs.end())
                    break;
            }

            bool success = row->addGlyph(*itGlyph);
            getNextGlyph = success;  // added successfully => continue with next glyph
            if (success) {
                bitmapWidth = std::max(bitmapWidth, (*itGlyph)->getAtlasX2());
                bitmapHeight = std::max(bitmapHeight, (*itGlyph)->getAtlasY2());
                std::cout << bitmapWidth << " " << bitmapHeight << std::endl;
            } else {
                // failed to add => create new row
                row = std::make_shared<glyphRow>(row, /*reverse direction*/ !row->getRightToLeft(), /*nGlyphs limit (none)*/ 0, /*fixed width*/ bitmapWidth);
            }
        }

        if (pWidth) *pWidth = bitmapWidth;
        if (pHeight) *pHeight = bitmapHeight;
        return bitmapWidth * bitmapHeight;
    }

    // copies one glyph to its location in the fontAtlas
    void copyGlyphToAtlas(const ::std::shared_ptr<glyph> g) {
        std::cout << "copying " << g->getAtlasWidth() << " x " << g->getAtlasHeight() << " to " << g->getAtlasX1() << " / " << g->getAtlasY1() << " of atlas " << width << " x " << height << std::endl;
        uint8_t* pAtlasTopLeft = bitmap + g->getAtlasY1() * width + g->getAtlasX1();
        uint8_t* pGlyph = g->getBitmapTmp();
        const size_t cAtlasWidth = width;
        const size_t glyphWidth = g->getAtlasWidth();
        const size_t glyphHeight = g->getAtlasHeight();
        for (size_t row = 0; row < glyphHeight; ++row)
            for (size_t col = 0; col < glyphWidth; ++col)
                *(pAtlasTopLeft + row * cAtlasWidth + col) = *(pGlyph++);
    }

    // === data ===
    uint8_t* bitmap = nullptr;  // atlas bitmap data
    size_t width;
    size_t height;
};
}  // namespace MNOGLA