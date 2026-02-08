//
// Created by mrsolak on 02.02.2026
//

#include "ImageConverter.h"

ImageConverter::ImageConverter(std::string input, std::string output) {
    this->input = "in_files/images/" + input;
    this->output = "out_files/images/" + output;
}

int ImageConverter::convert() {
    // Read the input image file
    cv::Mat image = cv::imread(this->input);

    if (image.empty()) {
        std::cerr << "Error: Failed to load image! Please check the file path." << std::endl;
        return -1;
    }

    // Automatically detects the file format from the extension and saves it
    if (cv::imwrite(this->output, image)) {
        std::cout << "Successfully converted: " << output << std::endl;
    } else {
        std::cerr << "Error: Failed to save the image!" << std::endl;
        return -1;
    }

    return 0;
}
