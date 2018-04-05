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
#include <exceptions.hpp>
#include <parameters.hpp>
#include <utils.hpp>

void print_informations(unsigned k, agent &ag) {
    std::cout << "step: " << k;
    std::cout << " time: " << ag.s.t;
    std::cout << " location: " << ag.s.get_name();
    std::cout << " goto: " << ag.a.direction;
    std::cout << " r: " << ag.r << std::endl;
}

/**
 * @brief Run using the parameters
 */
void run(const char *cfg_path, bool print) {
    parameters p(cfg_path);
    environment en = p.build_environment();
    agent ag = p.build_agent(en);
    unsigned k = 0;
    for(k = 0; k < p.SIMULATION_LIMIT_TIME; ++k) {
        ag.apply_policy();
        en.transition(ag.s,ag.s.t,ag.a,ag.r,ag.s_p);
        ag.process_reward();
        if(print) print_informations(k,ag);
        ag.step();
        if(en.is_state_terminal(ag.s)) {
            break;
        }
    }
    if(print) print_informations(k + 1,ag);
}

int main() {
    try {
        std::clock_t c_start = std::clock();
        srand(time(NULL));
        run("config/parameters.cfg",true);
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
