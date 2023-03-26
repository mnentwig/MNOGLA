#pragma once
#include <cstdio>
#include <map>
#include <regex>
#include <stdexcept>
#include <string>
#include <vector>

#include "../odsLoader/odsLoader.hpp"
using std::string, std::to_string, std::runtime_error, std::vector, std::map, std::regex;
class pattern {
   public:
    pattern(const MNOGLA::odsDoc::block& data) : name(), num(1), denom(1), notes() {
        map<string, int> notename2midi;
        const vector<string> notenames{"c", "c#", "d", "d#", "e", "f", "f#", "g", "g#", "a", "a#", "b"};
        for (int oct = 0; oct < 7; ++oct)
            for (size_t note = 0; note < notenames.size(); ++note)
                notename2midi.insert({notenames[note] + to_string(oct), note + 12 * oct});

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

        // === parse sequence ===
        // starting 2nd row, 2nd column
        for (size_t ixRow = 1; ixRow < data.getNRows(); ++ixRow) {
            notes.push_back(vector<int>());
            for (size_t ixCol = 1; ixCol < data.getNCols(); ++ixCol) {
                const string cellnote = data.getCellRel(ixRow, ixCol, /*default*/ "");  // e.g. "e3"
                if (cellnote == "")
                    notes.back().push_back(0);  // pause
                else if (cellnote == "-")
                    notes.back().push_back(-1);  // sustain
                else {
                    auto it = notename2midi.find(cellnote);
                    if (it == notename2midi.end()) throw runtime_error("failed to parse '" + cellnote + "'");
                    notes.back().push_back(it->second);
                }
            }
        }
    }
    const string& getName() const { return name; }
    const size_t getNum() const { return num; }
    const size_t getDenom() const { return denom; }
    const vector<vector<int>>& getNotesByPoly() const { return notes; }

   protected:
    string name;
    size_t num;
    size_t denom;
    vector<vector<int>> notes;
    static vector<string> strsplit(const string& s, const regex& e) {
        vector<string> r;
        std::sregex_token_iterator it(s.begin(), s.end(), e, -1);
        std::sregex_token_iterator end;
        while (it != end)
            r.push_back(*it++);
        return r;
    }
};