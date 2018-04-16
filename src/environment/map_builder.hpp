#ifndef MAP_BUILDER_HPP_
#define MAP_BUILDER_HPP_

class map_builder {
public:
    // Parameters for auto-generated map
    unsigned sampler_selector;
    unsigned nb_time_steps;
    unsigned time_steps_width;
    unsigned nb_nodes;
    unsigned min_nb_edges_per_node;
    double duration_min;
    double duration_max;
    double lip;

    // Parameters for imported map
    std::string terminal_location;
    std::string input_duration_matrix;
    std::string csv_sep;

    map_builder(
        unsigned _sampler_selector,
        unsigned _nb_time_steps,
        unsigned _time_steps_width,
        unsigned _nb_nodes,
        unsigned _min_nb_edges_per_node,
        double _duration_min,
        double _duration_max,
        double _lip,
        std::string _terminal_location,
        std::string _input_duration_matrix,
        std::string _csv_sep) :
        sampler_selector(_sampler_selector),
        nb_time_steps(_nb_time_steps),
        time_steps_width(_time_steps_width),
        nb_nodes(_nb_nodes),
        min_nb_edges_per_node(_min_nb_edges_per_node),
        duration_min(_duration_min),
        duration_max(_duration_max),
        lip(_lip),
        terminal_location(_terminal_location),
        input_duration_matrix(_input_duration_matrix),
        csv_sep(_csv_sep)
    {}

    void bound_variation(double &v) const {
        if(is_less_than(v,-lip)) {
            v = -lip;
        } else if (is_greater_than(v,lip)) {
            v = +lip;
        }
    }

    void first_order_random_uniform(std::vector<double> &vec, double &v) const {
        vec.push_back(vec.back() + v);
        v = uniform_double(-lip,lip);
    }

    void secnd_order_random_uniform(std::vector<double> &vec, double &v) const {
        vec.push_back(vec.back() + v);
        v += uniform_double(-lip / 2.,lip / 2.);
        bound_variation(v);
    }

    void secnd_order_epsilon_random_uniform(std::vector<double> &vec, double &v) const {
        vec.push_back(vec.back() + v);
        if(is_less_than(uniform_double(0.,1.),0.5)) {
            v += uniform_double(-lip / 2.,lip / 2.);
        }
        if(is_less_than(uniform_double(0.,1.),0.1)) {
            v = -v;
        }
        bound_variation(v);
    }

    void const_fun(std::vector<double> &v, unsigned length) const {
        double cs = v.back();
        for(unsigned i=0; i<length; ++i) {
            v.emplace_back(cs);
        }
    }

    void bound(double &value) const {
        if(is_less_than(value,duration_min)) {
            value = duration_min;
        } else if(is_greater_than(value,duration_max)) {
            value = duration_max;
        }
    }

    void cos_fun(std::vector<double> &v, unsigned length) const {
        double mag = ((double)length) * lip;
        double increment = uniform_double(-mag,mag);
        double x = 0.;
        double cs = v.back();
        for(unsigned i=0; i<length; ++i) {
            double new_value = cs + (increment / 2.) * (1. - cos(x * 3.14159265359 / ((double) length)));
            v.push_back(new_value);
            x += 1.;
        }
    }

    std::vector<double> cos_heuristic() const {
        std::vector<double> v = {
            uniform_double(duration_min,duration_max)
        };
        unsigned min_length = ((unsigned) nb_time_steps / 20.);
        unsigned max_length = ((unsigned) nb_time_steps / 3.0);
        while(v.size() < nb_time_steps) {
            unsigned length = uniform_integer(min_length,max_length);
            if(uniform_integer(0,2) == 0) {
                const_fun(v,length);
            } else {
                cos_fun(v,length);
            }
        }
        v.resize(nb_time_steps+1);
        return v;
    }

    /**
     * @brief Append duration
     *
     * Append a new sampled duration to the duration vector and modify the
     * inter time-step duration variation according to the selected model.
     */
    void append_duration(std::vector<double> &vec, double &v) const {
        switch(sampler_selector) {
            case 0: {
                first_order_random_uniform(vec,v);
                break;
            }
            case 1: {
                secnd_order_random_uniform(vec,v);
                break;
            }
            case 2: {
                secnd_order_epsilon_random_uniform(vec,v);
                break;
            }
            default: {
                first_order_random_uniform(vec,v);
                break;
            }
        }
    }

    /**
     * @brief Sample durations
     *
     * Build a vector containing the durations associated to an edge
     * at each time step.
     */
    std::vector<double> sample_durations() const {
        if(sampler_selector == 3) { // case 3
            return cos_heuristic();
        } else { // cases 0 1 2 or default
            std::vector<double> vec = {
                uniform_double(duration_min,duration_max)
            };
            double v = uniform_double(-lip,lip);
            for(unsigned j=0; j<nb_time_steps; ++j) {
                append_duration(vec,v);
                if(is_less_than(vec.back(),0.)) {
                    vec.back() = 0.;
                }
            }
            return vec;
        }
    }

    /**
     * @brief Create random edge
     */
    std::vector<std::string> create_random_edge(
        unsigned orig_ind,
        unsigned dest_ind,
        const std::vector<std::string> &nodes_names) const
    {
        std::vector<std::string> new_line;
        new_line.push_back(nodes_names.at(orig_ind));
        new_line.push_back(nodes_names.at(dest_ind));
        std::vector<double> durations = sample_durations();
        for(auto &d : durations) {
            new_line.push_back(std::to_string(d));
        }
        return new_line;
    }

    /**
     * @brief Build a random connected directed duration matrix
     */
    std::vector<std::vector<std::string>> build_random_connected_directed_duration_matrix() const {
        std::vector<std::vector<std::string>> dm;
        // 0. time scale
        std::vector<std::string> first_line;
        first_line.push_back("start");
        first_line.push_back("goal");
        for(unsigned i=0; i<nb_time_steps+1; ++i) {
            first_line.push_back(std::to_string(i * time_steps_width));
        }
        dm.push_back(first_line);
        // 1. Nodes
        std::vector<std::string> nodes_names;
        for(unsigned i=0; i<nb_nodes; ++i) {
            nodes_names.push_back("n" + std::to_string(i));
        }
        // 2. Build minimum of edges per node
        for(unsigned i=0; i<nb_nodes; ++i) {
            for(unsigned k=0; k<min_nb_edges_per_node; ++k) {
                unsigned dest_ind = i;
                while(dest_ind == i) {
                    dest_ind = rand_indice(nodes_names);
                }
                dm.push_back(create_random_edge(i,dest_ind,nodes_names));
            }
        }
        // 2. Additional edges for non-reachable nodes
        for(unsigned i=0; i<nb_nodes; ++i) {
            bool is_reachable = false;
            for(unsigned k=1; k<dm.size(); ++k) {
                if(dm.at(k).at(1).compare(nodes_names.at(i)) == 0) {
                    is_reachable = true;
                    break;
                }
            }
            if(!is_reachable) {
                unsigned orig_ind = i;
                while(orig_ind == i) {
                    orig_ind = rand_indice(nodes_names);
                }
                dm.push_back(create_random_edge(orig_ind,i,nodes_names));
            }
        }
        return dm;
    }

    /**
     * @brief Does edge exist
     */
    bool does_edge_exist(
        std::vector<std::vector<std::string>> dm,
        std::string orig_name,
        std::string dest_name) const
    {
        if(orig_name.compare(dest_name) == 0) {
            return true;
        } else {
            for(unsigned i=1; i<dm.size(); ++i) {
                if(dm.at(i).at(0).compare(orig_name) == 0) {
                    if(dm.at(i).at(1).compare(dest_name) == 0) {
                        return true;
                    }
                }
            }
            return false;
        }
    }

    /**
     * @brief Build a random connected symmetric directed duration matrix
     */
    std::vector<std::vector<std::string>> build_random_connected_symmetric_directed_duration_matrix() const {
        std::vector<std::vector<std::string>> dm;
        // 0. time scale
        std::vector<std::string> first_line;
        first_line.push_back("start");
        first_line.push_back("goal");
        for(unsigned i=0; i<nb_time_steps+1; ++i) {
            first_line.push_back(std::to_string(i * time_steps_width));
        }
        dm.push_back(first_line);
        // 1. Build minimum of edges per node + the come-back edge
        std::vector<std::string> nodes_names;
        for(unsigned i=0; i<nb_nodes; ++i) {
            nodes_names.push_back("n" + std::to_string(i));
        }
        std::vector<unsigned> nodes_edges_counter(nodes_names.size(),0);
        // 2. Edges
        for(unsigned i=0; i<nb_nodes; ++i) {
            for(unsigned k=nodes_edges_counter.at(i); k<min_nb_edges_per_node; ++k) {
                unsigned dest_ind = i;
                while(dest_ind == i || does_edge_exist(dm,nodes_names.at(i),nodes_names.at(dest_ind))) {
                    dest_ind = rand_indice(nodes_names);
                }
                nodes_edges_counter.at(i)++;
                nodes_edges_counter.at(dest_ind)++;
                dm.push_back(create_random_edge(i,dest_ind,nodes_names));
                dm.push_back(create_random_edge(dest_ind,i,nodes_names));
            }
        }
        return dm;
    }

    /**
     * @brief Extract the duration matrix
     */
    std::vector<std::vector<std::string>> extract_duration_matrix() const {
        std::filebuf fb;
        if (fb.open(input_duration_matrix,std::ios::in)) {
            std::vector<std::vector<std::string>> dm;
            std::istream is(&fb);
            while(true) {
                std::vector<std::string> result;
                std::string line;
                std::getline(is,line);
                std::stringstream ss(line);
                std::string cell;

                while(std::getline(ss,cell,csv_sep.at(0))) {
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

    /**
     * @brief Save duration matrix
     *
     * Save the input duration matrix at the specified output path.
     */
    void save_duration_matrix(
        const std::vector<std::vector<std::string>> &dm,
        const std::string &path) const
    {
        std::ofstream ofs;
        ofs.open(path);
        assert(dm.size() > 1);
        assert(dm.at(0).size() > 2);
        unsigned nrow = dm.size();
        unsigned ncol = dm.at(0).size();
        for(unsigned i=0; i<nrow; ++i) {
            for(unsigned j=0; j<ncol; ++j) {
                ofs << dm.at(i).at(j);
                if(j < ncol - 1) {
                    ofs << csv_sep.at(0);
                }
            }
            ofs << "\n";
        }
        ofs.close();
    }

    /**
     * @brief Is node already created
     */
    bool is_node_already_created(std::string s, const std::vector<map_node> &v) const {
        for(auto &elt: v) {
            if(elt.name.compare(s) == 0) {
                return true;
            }
        }
        return false;
    }

    /**
     * @brief Get durations
     */
    std::vector<double> get_durations(
        unsigned i,
        const std::vector<std::vector<std::string>> &dm) const
    {
        std::vector<double> v;
        for(unsigned j=2; j<dm.at(i).size(); ++j) {
            v.push_back(std::stod(dm.at(i).at(j)));
        }
        return v;
    }

    /**
     * @brief Build environment from duration matrix
     */
    void build_time_scale_and_map_from_duration_matrix(
        const std::vector<std::vector<std::string>> &dm,
        std::vector<double> &ts,
        std::vector<map_node> &nv) const
    {
        // 0. Extract time scale
        double tref = std::stod(dm.at(0).at(2));
        for(unsigned j=2; j<dm.at(0).size(); ++j) {
            ts.push_back(std::stod(dm.at(0).at(j)) - tref);
        }
        // 1. Create nodes and assign termination criterion
        for(unsigned i=1; i<dm.size(); ++i) {
            for(unsigned j=0; j<2; ++j) {
                if(!is_node_already_created(dm.at(i).at(j),nv)) {
                    if(dm.at(i).at(j).compare(terminal_location) == 0) {
                        nv.emplace_back(map_node(dm.at(i).at(j),true));
                    } else {
                        nv.emplace_back(map_node(dm.at(i).at(j),false));
                    }
                }
            }
        }
        // 2. Create edges
        for(unsigned i=1; i<dm.size(); ++i) {
            unsigned orig_nd_indice = 0, dest_nd_indice = 0;
            for(unsigned k=0; k<nv.size(); ++k) {
                if(nv.at(k).name.compare(dm.at(i).at(0)) == 0) {
                    orig_nd_indice = k;
                }
                if(nv.at(k).name.compare(dm.at(i).at(1)) == 0) {
                    dest_nd_indice = k;
                }
            }
            nv.at(orig_nd_indice).edges.emplace_back(&nv.at(dest_nd_indice));
            nv.at(orig_nd_indice).edges_costs.push_back(get_durations(i,dm));
        }
    }
};

#endif // MAP_BUILDER_HPP_
