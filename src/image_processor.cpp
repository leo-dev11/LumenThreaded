#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "image_processor.hpp"
#include <iostream>

void ImageProcessor::StbDeleter::operator()(unsigned char *p) const { stbi_image_free(p); }

bool ImageProcessor::loadImage(const std::string &filename) {
    unsigned char *rawData = stbi_load(filename.c_str(), &width, &height, &channels, 0);
    if (!rawData) {
        std::cerr << "Error: Can't load the image" << filename << std::endl;
        return false;
    }
    // Transfer the raw pointer property to unique_ptr
    data.reset(rawData);
    return true;
}

bool ImageProcessor::saveImage(const std::string &filename) {
    if (!data)
        return false;
    return stbi_write_png(filename.c_str(), width, height, channels, data.get(), width * channels);
}

void ImageProcessor::applyGrayscaleParallel(ThreadPool &pool) {
    int numThreads = std::thread::hardware_concurrency();
    int rowsPerStrip = height / numThreads;
    std::vector<std::future<void>> futures;
    for (int i = 0; i < numThreads; i++) {
        int startRow = i * rowsPerStrip;
        int endRow = (i == numThreads - 1) ? height : (i + 1) * rowsPerStrip;
        // Enqueue tasks in the pool
        futures.push_back(pool.enqueue([this, startRow, endRow] { this->processStrip(startRow, endRow); }));
    }
    // Sync barrier, We wait until all threads finish
    for (auto &f : futures) {
        f.get();
    }
}

void ImageProcessor::processStrip(int startRow, int endRow) {
    for (int y = startRow; y < endRow; ++y) {
        for (int x = 0; x < width; ++x) {
            unsigned char *pixel = data.get() + (y * width + x) * channels;
            // Standar formula of luminance for gray scale
            // Gray = 0.299R + 0.587G + 0.114B
            unsigned char gray = static_cast<unsigned char>(0.299f * pixel[0] + 0.587f * pixel[1] + 0.114f * pixel[2]);
            pixel[0] = pixel[1] = pixel[2] = gray;
        }
    }
}
