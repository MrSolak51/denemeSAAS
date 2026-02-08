//
// Created by mrsolak on 6.02.2026.
//

#include "DocumentConverter.h"


DocumentConverter::DocumentConverter(std::string input, std::string target_format) {
    this->input = "in_files/documents/" + input;
    this->target_format = target_format;
}

int DocumentConverter::convert() {
    if (!fs::exists(input)) {
        std::cerr << "Hata: Girdi dosyasi bulunamadi!" << std::endl;
        return false;
    }

    // Komutu inşa et (Tırnak işaretleri boşluklu yollar için kritiktir)
    // --outdir . komutu, çıktının mevcut klasöre düşmesini sağlar
    std::string command = "soffice --headless --convert-to " + target_format +
                          " \"" + input + "\" --outdir out_files/documents/";

    std::cout << "Islem baslatiliyor: " << fs::path(input).filename()
              << " -> " << target_format << std::endl;

    return (std::system(command.c_str()) == 0);
}