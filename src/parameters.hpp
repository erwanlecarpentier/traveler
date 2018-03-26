#ifndef PARAMETERS_HPP_
#define PARAMETERS_HPP_

#include <fstream>
#include <sstream>

#include <exceptions.hpp>
#include <state.hpp>

class parameters {
public:
    unsigned SIMULATION_LIMIT_TIME = 3;
    std::string GRAPH_DURATION_MATRIX_PATH = "data/duration_matrix.csv";
    char CSV_SEP = ';';
    state INITIAL_STATE;

    parameters() {
        //
    }

    std::vector<std::vector<std::string>> extract_duration_matrix() {
        std::filebuf fb;
        if (fb.open(GRAPH_DURATION_MATRIX_PATH,std::ios::in)) {
            std::vector<std::vector<std::string>> dm;
            std::istream is(&fb);
            while(true) {
                std::vector<std::string> result;
                std::string line;
                std::getline(is,line);
                std::stringstream ss(line);
                std::string cell;

                while(std::getline(ss,cell,CSV_SEP)) {
                    result.push_back(cell);
                }
                if(is.eof()) {
                    break;
                } else {
                    dm.push_back(result);
                }
            }
            fb.close();
            return dm;
        } else {
            throw duration_matrix_file_exception();
        }
    }

    bool is_node_already_created(std::string s, const std::vector<map_node> &v) const {
        for(auto &elt: v) {
            if(elt.name.compare(s) == 0) {
                return true;
            }
        }
        return false;
    }

    std::vector<unsigned> get_durations(
        unsigned i,
        const std::vector<std::vector<std::string>> &dm) const
    {
        std::vector<unsigned> v;
        for(unsigned j=2; j<dm.at(i).size(); ++j) {
            v.push_back(std::stoul(dm.at(i).at(j),nullptr,0));
        }
        return v;
    }

    void parse_map(std::vector<map_node> &v) {
        std::vector<std::vector<std::string>> dm = extract_duration_matrix();
        // 1. Create nodes
        for(unsigned i=1; i<dm.size(); ++i) {
            for(unsigned j=0; j<2; ++j) {
                if(!is_node_already_created(dm.at(i).at(j),v)) {
                    v.emplace_back(map_node(dm.at(i).at(j)));
                }
            }
        }
        // 2. Create edges
        for(unsigned i=1; i<dm.size(); ++i) {
            unsigned orig = 0, dest = 0;
            for(unsigned k=0; k<v.size(); ++k) {
                if(v.at(k).name.compare(dm.at(i).at(0)) == 0) {
                    orig = k;
                }
                if(v.at(k).name.compare(dm.at(i).at(1)) == 0) {
                    dest = k;
                }
            }
            v.at(orig).edges.emplace_back(&v.at(dest));
            v.at(orig).edges_costs.push_back(get_durations(i,dm));
        }
    }
};

#endif // PARAMETERS_HPP_
