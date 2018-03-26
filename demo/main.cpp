#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <memory>
#include <random>
#include <vector>

#include <agent.hpp>
#include <environment.hpp>
#include <parameters.hpp>
#include <utils.hpp>

void run(bool print) {
    parameters p;
    environment en(p);
    agent ag(p);
    for(unsigned t = 0; t < p.SIMULATION_LIMIT_TIME; ++t) {
        if(print) {
            std::cout << t << std::endl;
        }
        ag.apply_policy();
        en.transition(ag);
        ag.process_reward();
        ag.step();
        if(en.is_terminal(ag)) {
            break;
        }
    }
}

/**
 * @brief Main function
 */
int main() {
    try {
        std::clock_t c_start = std::clock();
        srand(time(NULL));

        run(true);

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
