//
// Created by mrsolak on 6.02.2026.
//

#ifndef METEHAN_TURGUT_DOCUMENTCONVERTER_H
#define METEHAN_TURGUT_DOCUMENTCONVERTER_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cstdlib>
#include <filesystem>

namespace fs = std::filesystem;

class DocumentConverter {
private:
    // Desteklenen format kategorileri
    const std::map<std::string, std::vector<std::string>> formatMap = {
        {"Belge", {"pdf", "docx", "doc", "odt", "rtf", "txt", "html"}},
        {"Sunum", {"pptx", "ppt", "odp", "pdf"}},
        {"Tablo", {"xlsx", "xls", "ods", "csv", "pdf"}},
        {"Görsel", {"png", "jpg", "jpeg", "svg", "eps"}}
    };

public:
    void listFormats() const;
    bool convert(const std::string& inputPath, const std::string& targetFormat);
};



#endif //METEHAN_TURGUT_DOCUMENTCONVERTER_H