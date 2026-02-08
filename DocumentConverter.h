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
    std::string input;
    std::string output;
    std::string target_format;

public:
    DocumentConverter(std::string input, std::string target_format);

    int convert();
};



#endif //METEHAN_TURGUT_DOCUMENTCONVERTER_H