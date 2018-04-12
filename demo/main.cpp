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
 */
void single_run(char * n) {
    std::string path(n);
    parameters p(path);
    std::vector<std::vector<double>> v;
    run(p,true,false,v);
}

/**
 * @brief Generate map
 */
void generate_maps(char * n) {
    // Init
    std::string path(n);
    parameters p(path);
    p.GENERATE_MAP = true;
    p.SAVE_DURATION_MATRIX = true;
    // Build
    for(unsigned i=0; i<100; ++i) {
        std::cout << "build map " << i << std::endl;
        p.OUTPUT_DURATION_MATRIX = "config/map" + std::to_string(i) + ".cfg";
        environment en = p.build_environment();
    }
}

std::string get_p_path(const char * param_id) {
    std::string pid(param_id);
    return "config/backup/parameters" + pid + ".cfg";
}

std::string get_m_path(const char * map_id) {
    std::string mid(map_id);
    return "config/backup/maps/map" + mid + ".csv";
}

std::string get_b_path(const char * param_id, const char * map_id) {
    std::string pid(param_id);
    std::string mid(map_id);
   return "data/svg" + pid + "_" + mid + ".csv";
}

/**
 * @brief Test function
 */
void test(const char * param_id, const char * map_id) {
    // Init
    std::string parameters_path = get_p_path(param_id);
    parameters p(parameters_path);
    p.BACKUP_PATH = get_b_path(param_id,map_id);
    p.INPUT_DURATION_MATRIX = get_m_path(map_id);
    std::vector<std::vector<double>> backup_vector;
    // Run
    for(unsigned i=0; i<p.NB_SIMULATIONS; ++i) {
        run(p,false,true,backup_vector);
    }
    // Save
    save_csv(
        std::vector<std::string>{"elapsed_time","total_return"},
        backup_vector,
        p.BACKUP_PATH
    );
}

int main(int argc, char ** argv) {
    try {
        std::clock_t c_start = std::clock();
        srand(time(NULL));

        if(argc > 1) {
            //single_run(argv[1]);
            test(argv[1],argv[2]);
            //generate_maps(argv[1]);
        } else {
            std::cout << "No parameters\n";
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
