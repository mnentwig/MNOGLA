#pragma once
#include <cstdlib>  // realloc
#include <cstring>  // memcpy
#include <filesystem>
#include <functional>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>

#include "../../MNOGLA.h"
#include "minizip/ioapi.c"
#include "minizip/unzip.c"
// #include "minizip/unzip.h"
#include "tinyxml2/tinyxml2.cpp"

namespace MNOGLA {
using namespace tinyxml2;
using std::runtime_error, std::string, std::cout, std::endl, std::map, std::to_string, std::vector;

class odsDoc {
   public:
    odsDoc(const string& filename) : odsDoc() {
        if (MNOGLA::mainArg0 == nullptr)
            throw runtime_error("need executable location for resource file path");

        // resource files are expected to be copied into build folder, where the executable resides
        // e.g. use cmake "configure_file(../../mySourceFolder/mySource.ods myDest.ods COPYONLY)"
        ::std::filesystem::path p(MNOGLA::mainArg0);
        p.replace_filename(filename);
        // === load XML from .ods (which is a zip file internally) ===
        int lengthOfXmlData;
        char* buf = unzipToBuf(p.string().c_str(), "content.xml", &lengthOfXmlData);
        cout << "create" << endl;
        RAII_cleanup c([&buf] { free(buf); buf = nullptr; });
        if (!buf) throw runtime_error("unzip failed for " + filename + "(looking at " + p.string() + ")");

        startup(buf, lengthOfXmlData);
    }

    odsDoc(const char* buf, size_t nBytes) : odsDoc() {
        startup(buf, nBytes);
    }

    size_t getNSheets() { return sheetNames.size(); }

    typedef map<size_t, string> odsRow_t;
    typedef map<size_t, odsRow_t> odsSheet_t;
    typedef map<size_t, odsSheet_t> odsBook_t;

   protected:
    odsDoc() : sheetNames(), content() {}
    vector<string> sheetNames;
    odsBook_t content;

    /* Loads "fileToExtract" from "zipfile". Returns buffer with contents or NULL, if failed.
       Use free() on buffer to deallocate.
       length returns the number of bytes. Contents are null-terminated.
    */
   protected:
    static char* unzipToBuf(const char* zipfile, const char* fileToExtract, int* length) {
        unzFile uf = unzOpen64(zipfile);
        if (!uf) return NULL;
        if (unzLocateFile(uf, fileToExtract, /*case sensitive*/ 0) != UNZ_OK) return NULL;
        if (unzOpenCurrentFilePassword(uf, /*password*/ NULL)) return NULL;

        char chunk[65536];
        int nRead = 0;
        char* retBuf = NULL;
        while (1) {
            int nBytes = unzReadCurrentFile(uf, chunk, sizeof(chunk));
            if (nBytes < 0) {                        // error
                retBuf = (char*)realloc(retBuf, 0);  // free
                return NULL;
            }

            if (nBytes == 0)
                break;
            retBuf = (char*)realloc(retBuf, nRead + nBytes + /*null termination*/ 1);
            memcpy(/*dest*/ retBuf + nRead, /*src*/ chunk, nBytes);
            *(retBuf + nRead + nBytes) = 0;  // null termination
            nRead += nBytes;
        }

        *length = nRead;
        return retBuf;
    }

    //* traverse to next element of same type (name) e.g. table, row, cell in a spreadsheet */
   protected:
    static XMLElement* xmlNext(XMLElement* e) {
        return e->NextSiblingElement(e->Value());
    }

   protected:
    void startup(const char* buf, size_t nBytes) {
        // === load XML ===
        XMLDocument doc;
        if (XML_SUCCESS != doc.Parse(buf, nBytes)) throw runtime_error("loadfile failed");

        // === locate first spreadsheet in XML hierarchy ===
        XMLElement* e = doc.FirstChildElement("office:document-content")->FirstChildElement("office:body")->FirstChildElement("office:spreadsheet");
        if (!e) throw runtime_error("failed to parse xml hierarchy to office:spreadsheet level");
        XMLElement* table = e->FirstChildElement("table:table");
        if (!table) throw runtime_error("no table");  // need at least one
        size_t ixTable = 0;
        while (table) {
            const char* tname = table->Attribute("table:name");
            if (!tname) throw runtime_error("no table name");
            sheetNames.push_back(tname);

            // === create new sheet (via default constructor) ===
            odsSheet_t& currentSheet = content[ixTable];

            cout << "$NEW_SHEET," << tname << endl;

            size_t ixRow = 0;
            // === locate first row in XML hierarchy ===
            XMLElement* row = table->FirstChildElement("table:table-row");
            while (row) {
                size_t nRowRep = 1;
                const char* tnRowRep = row->Attribute("table:number-rows-repeated");
                if (tnRowRep != NULL)
                    nRowRep = std::atol(tnRowRep);
                for (size_t ixRowRep = 0; ixRowRep < nRowRep; ++ixRowRep) {
                    // === create new row ===
                    currentSheet.insert({ixRow, odsRow_t()});
                    odsRow_t& currentRow = currentSheet[ixRow];
                    size_t ixCol = 0;

                    // === locate first cell in XML hierarchy ===
                    XMLElement* cell = row->FirstChildElement("table:table-cell");
                    while (cell) {
                        size_t nColRep = 1;
                        const char* tnColRep = cell->Attribute("table:number-columns-repeated");
                        if (tnColRep != NULL)
                            nColRep = std::atol(tnColRep);

                        // === extract value ===
                        XMLElement* text = cell->FirstChildElement("text:p");
                        const char* tOut = text ? text->GetText() : nullptr;
                        for (size_t ix = 0; ix < nColRep; ++ix) {
                            if (tOut != nullptr) {
                                currentRow[ixCol] = tOut;
                                cout << ixTable << "\t" << ixRow << "\t" << ixCol << "\t" << tOut << endl;
                            }
                            ++ixCol;
                        }  // for nColRep
                        cell = xmlNext(cell);
                    }  // while cell
                    cout << endl;
                    ++ixRow;
                }  // for ixRowRep
                row = xmlNext(row);
            }  // while row
            table = xmlNext(table);
            ++ixTable;
        }  // while table
    }

   public:
    void getSize(size_t ixTable, size_t* nRows, size_t* nCols) {
        assert(nRows != nullptr);
        assert(nCols != nullptr);
        *nRows = 0;
        *nCols = 0;
        auto it1 = content.find(ixTable);
        if (it1 == content.end()) return;  // table does not exist
        odsSheet_t& currentSheet = it1->second;
        for (auto& itRow : currentSheet) {
            size_t ixCurrentRow = itRow.first;
            odsRow_t& currentRow = itRow.second;
            *nRows = std::max(*nRows, ixCurrentRow + 1u);  // size = ixMax+1
            for (auto& itCol : currentRow) {
                size_t ixCurrentCol = itCol.first;
                *nCols = std::max(*nCols, ixCurrentCol + 1u);  // size = ixMax+1
            }                                                  // for itCol
        }                                                      // for itRow
    }

   public:
    bool getCell(size_t ixSheet, size_t ixRow, size_t ixCol, string& val) const {
        auto it1 = content.find(ixSheet);
        if (it1 == content.end()) return false;  // sheet does not exist
        const odsSheet_t& sheet = it1->second;
        auto it2 = sheet.find(ixRow);
        if (it2 == sheet.end()) return false;  // row does not exist
        const odsRow_t row = it2->second;
        auto it3 = row.find(ixCol);
        if (it3 == row.end()) return false;  // cell does not exist
        val = it3->second;
    }
    class block {
       public:
        block(const odsDoc& doc, size_t ixSheet, size_t ixRow, size_t ixCol) : doc(doc), ixSheet(ixSheet), ixRow(ixRow), ixCol(ixCol), nRows(0), nCols(0) {
            string val;
            if (!(doc.getCell(ixSheet, ixRow, ixCol, val) && val == "")) throw runtime_error("odsDoc.block(): Start on empty or non-existing cell");
            // determine extent in horizontal direction
            while (doc.getCell(ixSheet, ixRow, ixCol + nCols, val) && val != "") ++nCols;
            // determine extent in vertical direction
            while (doc.getCell(ixSheet, ixRow + nRows, ixCol, val) && val != "") ++nRows;
        }

       protected:
        const odsDoc& doc;
        size_t ixSheet;
        size_t ixRow;
        size_t ixCol;
        size_t nRows;
        size_t nCols;
    };

   protected:
    // helper class for RAII-wrapping a cleanup function.
    // note: this "lite" version of the design pattern does not protect against copying, which may cause multiple execution of f() if abused
    // see final_action at "guideline support library" https://github.com/microsoft/GSL/blob/main/include/gsl/util
    struct RAII_cleanup {
       public:
        explicit RAII_cleanup(std::function<void()> f) : f(f) {}
        ~RAII_cleanup() { f(); }

       protected:
        std::function<void()> f;
    };
};
}  // namespace MNOGLA