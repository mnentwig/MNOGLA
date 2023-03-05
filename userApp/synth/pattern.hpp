#include <cstdio>
#include <stdexcept>
#include <string>

#include "../odsLoader/odsLoader.hpp"
using std::string, std::runtime_error;
class pattern {
   public:
    pattern(const MNOGLA::odsDoc::block& data) : name(), num(0), denom(0) {
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
                if (sscanf(val.c_str(), "%llu", &this->num) != 1) throw runtime_error("pattern: failed to convert denom");
            } else
                throw runtime_error("pattern: invalid key '" + key + "'");
        }
    }

    const string& getName() const { return name; }

   protected:
    string name;
    size_t num;
    size_t denom;
};