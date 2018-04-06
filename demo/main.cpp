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
 * @brief Test function
 */
void test(char * n) {
    // Init
    std::string path(n);
    parameters p(path);
    std::vector<std::vector<double>> backup_vector;

    // Simulate
    for(unsigned i=0; i<p.NB_SIMULATIONS; ++i) {
        for(unsigned j=0; j<100; ++j) {
            std::string map_path = "config/backup/map" + std::to_string(j) + ".csv";
            p.GRAPH_DURATION_MATRIX_PATH = map_path;
            run(p,false,true,backup_vector);
        }
    }

    // Save
    initialize_backup(
        std::vector<std::string>{"elapsed_time","total_return"},
        p.get_backup_path_from_parameters_path(),
        ","
    );
    save_matrix(
        backup_vector,
        p.get_backup_path_from_parameters_path(),
        ",",
        std::ofstream::app
    );
}

int main(int argc, char ** argv) {
    try {
        std::clock_t c_start = std::clock();
        srand(time(NULL));

        if(argc == 2) {
            test(argv[1]);
        } else {
            std::cout << "No arguments\n";
        }

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
