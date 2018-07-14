#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <list>
#include <memory>
#include <random>
#include <vector>

#include <agent.hpp>
#include <environment.hpp>
#include <exceptions.hpp>
#include <parameters.hpp>
#include <save.hpp>
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
void run(
    const parameters &p,
    bool print,
    bool backup,
    std::vector<std::vector<double>> &backup_vector)
{
    environment en = p.build_environment();
    agent ag = p.build_agent(en);
    unsigned k = 0;
    double total_return = 0.;
    for(k = 0; k < p.SIMULATION_LIMIT_TIME; ++k) {
        ag.apply_policy();
        en.transition(ag.s,ag.s.t,ag.a,ag.r,ag.s_p);
        total_return += ag.r;
        ag.process_reward();
        if(print) print_informations(k,ag);
        ag.step();
        if(en.is_state_terminal(ag.s)) {
            break;
        }
    }
    if(print) {
        std::cout << "Time elapsed: " << ag.s.t << " ";
        std::cout << "total return: " << total_return << "\n";
    }
    if(backup) {
        backup_vector.push_back(std::vector<double>{ag.s.t,total_return});
    }
}

/**
 * @brief Single run
 *
 * Perform a single roll-out according to the parameters and save the data at the given path.
 * @param {char *} n; parameters path
 */
void single_run(char * n) {
    std::string path(n);
    parameters p(path);
    std::vector<std::vector<double>> v;
    run(p,true,true,v);
    save_csv(std::vector<std::string>{"elapsed_time","total_return"},v,p.BACKUP_PATH);
}

int main(int argc, char ** argv) {
    try {
        std::clock_t c_start = std::clock();
        srand(time(NULL));
        if(argc > 1) {
            single_run(argv[1]);
        } else {
            throw no_parameters_path_exception();
        }
        std::clock_t c_end = std::clock();
        double time_elapsed_ms = 1e3 * (c_end - c_start) / CLOCKS_PER_SEC;
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
