#include "image_processor.hpp"
#include "thread_pool.hpp"
#include <chrono>
#include <filesystem>
#include <iostream>

int main() {
    const std::string inputPath = "input.png";
    const std::string outputPath = "output_parallel.png";

    if (!std::filesystem::exists(inputPath)) {
        std::cerr << "Error: Can't find input.png in the directory." << std::endl;
        return 1;
    }

    unsigned int numThreads = std::thread::hardware_concurrency();
    ThreadPool pool(numThreads);
    ImageProcessor processor;

    std::cout << "Lumen-Threaded Image Processor" << std::endl;
    std::cout << "Using " << numThreads << " threads." << std::endl;
    // Load image
    if (!processor.loadImage(inputPath))
        return 1;
    // Performance benchmarking
    auto start = std::chrono::high_resolution_clock::now();
    processor.applyGrayscaleParallel(pool);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;

    if (processor.saveImage(outputPath)) {
        std::cout << "Process completed :D" << std::endl;
        std::cout << "Process time: " << elapsed.count() << " ms" << std::endl;
        std::cout << "Result saved in: " << outputPath << std::endl;
    } else {
        std::cerr << "Error saving the image." << std::endl;
    }
    return 0;
}
