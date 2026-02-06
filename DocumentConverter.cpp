//
// Created by mrsolak on 6.02.2026.
//

#include "DocumentConverter.h"

void DocumentConverter::listFormats() const {
    std::cout << "--- Desteklenen Hedef Formatlar ---" << std::endl;
    for (auto const& [kategori, uzantilar] : formatMap) {
        std::cout << kategori << ": ";
        for (const auto& u : uzantilar) std::cout << u << " ";
        std::cout << std::endl;
    }
}

bool DocumentConverter::convert(const std::string& inputPath, const std::string& targetFormat) {
    if (!fs::exists(inputPath)) {
        std::cerr << "Hata: Girdi dosyasi bulunamadi!" << std::endl;
        return false;
    }

    // Komutu inşa et (Tırnak işaretleri boşluklu yollar için kritiktir)
    // --outdir . komutu, çıktının mevcut klasöre düşmesini sağlar
    std::string command = "soffice --headless --convert-to " + targetFormat +
                          " \"" + inputPath + "\" --outdir .";

    std::cout << "Islem baslatiliyor: " << fs::path(inputPath).filename()
              << " -> " << targetFormat << std::endl;

    return (std::system(command.c_str()) == 0);
}