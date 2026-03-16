#ifndef IMAGE_PROCESSOR_HPP
#define IMAGE_PROCESSOR_HPP

#include "thread_pool.hpp"
#include <memory>
#include <string>
#include <vector>

class ImageProcessor {
  public:
    ImageProcessor() = default;
    ~ImageProcessor() = default;

    // Load the image in memory using smart pointer custom
    bool loadImage(const std::string &filename);

    // Save the result in the disk
    bool saveImage(const std::string &filename);

    void applyGrayscaleParallel(ThreadPool &pool);

  private:
    int width{0};
    int height{0};
    int channels{0};

    // unique_ptr with a custom deleter to coincide with the STB library
    struct StbDeleter {
        void operator()(unsigned char *p) const;
    };
    std::unique_ptr<unsigned char[], StbDeleter> data;

    void processStrip(int startRow, int endRow);
};

#endif
