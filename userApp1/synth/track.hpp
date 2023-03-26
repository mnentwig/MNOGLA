#pragma once
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

#include "pattern.hpp"
using std::string, std::to_string, std::runtime_error, std::vector, std::map;
#pragma once
namespace MNOGLA {
class track {
   public:
    track(const MNOGLA::odsDoc::block& data, const map<string, pattern>& patterns) : name(), num(1), denom(1), notesByPoly() {
        // === parse parameters ===
        // starting 2nd row, first column
        for (size_t ixRow = data.getIxRow() + 1; ixRow < data.getIxRow() + data.getNRows(); ixRow += 2) {
            string key;
            if (!data.getDoc().getCell(data.getIxSheet(), ixRow, data.getIxCol(), key)) throw runtime_error("?? empty cell in first block col (key)??");
            string val;
            if (!data.getDoc().getCell(data.getIxSheet(), ixRow + 1, data.getIxCol(), val)) throw runtime_error("?? empty cell in first block col (val)??");
            if (key == "name") {
                name = val;
            } else if (key == "num") {
                if (sscanf(val.c_str(), "%llu", &this->num) != 1) throw runtime_error("pattern: failed to convert num");
            } else if (key == "denom") {
                if (sscanf(val.c_str(), "%llu", &this->denom) != 1) throw runtime_error("pattern: failed to convert denom");
            } else
                throw runtime_error("pattern: invalid key '" + key + "'");
        }

        // === parse content ===
        // starting 2nd row, 2nd column
        bool isFirstPat = true;
        for (size_t ixRow = 1; ixRow < data.getNRows(); ++ixRow) {
            notesByPoly.push_back(vector<int>());
            const string patchname = data.getCellRel(ixRow, /*ixCol*/ 1, /*default*/ "(empty patchname)");

            for (size_t ixCol = 2; ixCol < data.getNCols(); ++ixCol) {
                const string patternName = data.getCellRel(ixRow, ixCol, /*default*/ "(empty patternname)");
                auto it = patterns.find(patternName);
                if (it == patterns.end()) throw runtime_error("unknown pattern: " + patternName);
                const pattern& p = it->second;
                // === configure num/denom ===
                if (isFirstPat) {
                    isFirstPat = false;
                    num = p.getNum();
                    denom = p.getDenom();
                } else {
                    if (num != p.getNum() || denom != p.getDenom()) throw runtime_error("num/denom mismatch");
                }

                // === append notes ===
                size_t origLength = getNNotes();
                for (size_t ixPoly = 0; ixPoly < p.getNotesByPoly().size(); ++ixPoly) {
                    // === create poly vector ===
                    if (notesByPoly.size() < ixPoly + 1)
                        notesByPoly.push_back(vector<int>());

                    // == pad poly vector ===
                    auto& nbpDest = notesByPoly[ixPoly];
                    while (nbpDest.size() < origLength)
                        nbpDest.push_back(/*pause*/ 0);

                    // === append to poly vector ===
                    const auto& nbpSrc = p.getNotesByPoly()[ixPoly];
                    nbpDest.insert(nbpDest.begin(), nbpSrc.begin(), nbpSrc.end());
                }  // for ixPoly
            }      // for ixCol (patterns in table)
        }          // for ixRow (synths in table)
    }

    size_t getNNotes() const {
        size_t n = 0;
        for (const auto& it : notesByPoly)
            n = std::max(n, it.size());
        return n;
    }

    const string& getName() const { return name; }
    const size_t getNum() const { return num; }
    const size_t getDenom() const { return denom; }
    const vector<vector<int>>& getNotesByPoly() const { return notesByPoly; }

   protected:
    string name;
    size_t num;
    size_t denom;
    vector<vector<int>> notesByPoly;
};
}  // namespace MNOGLA