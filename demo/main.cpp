#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <random>
#include <vector>

#include <utils.hpp>

void run() {
    std::cout << "Hello world\n";
}

/**
 * @brief Main function
 */
int main() {
    try {
        std::clock_t c_start = std::clock();
        srand(time(NULL));

        run();

        std::clock_t c_end = std::clock();
        double time_elapsed_ms = 1000. * (c_end - c_start) / CLOCKS_PER_SEC;
        std::cout << "Program run in " << time_elapsed_ms << "ms" << std::endl;
    }
    catch(const std::exception &e) {
        std::cerr << "Error in main(): standard exception caught: " << e.what() << std::endl;
    }
    catch(...) {
        std::cerr << "Error in main(): unknown exception caught" << std::endl;
    }
    return 0;
}
