//
// Created by mrsolak on 2.02.2026.
//

#ifndef METEHAN_TURGUT_IMAGECONVERTER_H
#define METEHAN_TURGUT_IMAGECONVERTER_H

#include <opencv2/opencv.hpp>
#include <iostream>


class ImageConverter {
private:
    std::string input;
    std::string output;
public:
    ImageConverter(std::string input, std::string output);

    int convert();

};


#endif //METEHAN_TURGUT_IMAGECONVERTER_H