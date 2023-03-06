#include <cstdio>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

#include "../odsLoader/odsLoader.hpp"
using std::string, std::to_string, std::runtime_error, std::vector, std::map;
class pattern {
   public:
    pattern(const MNOGLA::odsDoc::block& data) : name(), denom(4), notes() {
        map<string, int> notename2midi;
        const vector<string> notenames{"c", "c#", "d", "d#", "e", "f", "f#", "g", "g#", "a", "a#", "b"};
        for (int oct = 1; oct < 7; ++oct)
            for (size_t note = 0; note < notenames.size(); ++note)
                notename2midi.insert({notenames[note] + to_string(oct), note + 12 * oct + 12});

        // === parse parameters ===
        // starting 2nd row, first column
        for (size_t ixRow = data.getIxRow() + 1; ixRow < data.getIxRow() + data.getNRows(); ixRow += 2) {
            string key;
            if (!data.getDoc().getCell(data.getIxSheet(), ixRow, data.getIxCol(), key)) throw runtime_error("?? empty cell in first block col (key)??");
            string val;
            if (!data.getDoc().getCell(data.getIxSheet(), ixRow + 1, data.getIxCol(), val)) throw runtime_error("?? empty cell in first block col (val)??");
            if (key == "name") {
                name = val;
            } else if (key == "denom") {
                if (sscanf(val.c_str(), "%llu", &this->denom) != 1) throw runtime_error("pattern: failed to convert denom");
            } else
                throw runtime_error("pattern: invalid key '" + key + "'");
        }

        // === parse sequence ===
        // starting 2nd row, 2nd column
        for (size_t ixCol = data.getIxCol() + 1; ixCol < data.getIxCol() + data.getNCols(); ++ixCol) {
            string val;
            if (!data.getDoc().getCell(data.getIxSheet(), data.getIxRow() + 1, ixCol, val))
                val = "";
            if (val == "")
                notes.push_back(0);  // pause
            else if (val == "-")
                notes.push_back(-1);  // sustain
            else {
                auto it = notename2midi.find(val);
                if (it == notename2midi.end()) throw runtime_error("failed to parse " + val);
                notes.push_back(it->second);
            }
        }
    }
    const string& getName() const { return name; }
    const size_t getDenom() const { return denom; }

   protected:
    string name;
    size_t denom;
    vector<int> notes;
};