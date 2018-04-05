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
    double initial_duration_min;
    double initial_duration_max;
    double duration_variation_max;

    // Parameters for imported map
    std::string terminal_location;
    std::string graph_duration_matrix_path;
    std::string csv_sep;

    map_builder(
        unsigned _sampler_selector,
        unsigned _nb_time_steps,
        unsigned _time_steps_width,
        unsigned _nb_nodes,
        unsigned _min_nb_edges_per_node,
        double _initial_duration_min,
        double _initial_duration_max,
        double _duration_variation_max,
        std::string _terminal_location,
        std::string _graph_duration_matrix_path,
        std::string _csv_sep) :
        sampler_selector(_sampler_selector),
        nb_time_steps(_nb_time_steps),
        time_steps_width(_time_steps_width),
        nb_nodes(_nb_nodes),
        min_nb_edges_per_node(_min_nb_edges_per_node),
        initial_duration_min(_initial_duration_min),
        initial_duration_max(_initial_duration_max),
        duration_variation_max(_duration_variation_max),
        terminal_location(_terminal_location),
        graph_duration_matrix_path(_graph_duration_matrix_path),
        csv_sep(_csv_sep)
    {}

    void bound_variation(double &v) const {
        if(is_less_than(v,-duration_variation_max)) {
            v = -duration_variation_max;
        } else if (is_greater_than(v,duration_variation_max)) {
            v = +duration_variation_max;
        }
    }

    void first_order_random_uniform(std::vector<double> &vec, double &v) const {
        vec.push_back(vec.back() + v);
        v = uniform_double(-duration_variation_max,duration_variation_max);
    }

    void secnd_order_random_uniform(std::vector<double> &vec, double &v) const {
        vec.push_back(vec.back() + v);
        v += uniform_double(-duration_variation_max / 2.,duration_variation_max / 2.);
        bound_variation(v);
    }

    void secnd_order_epsilon_random_uniform(std::vector<double> &vec, double &v) const {
        vec.push_back(vec.back() + v);
        if(is_less_than(uniform_double(0.,1.),0.5)) {
            v += uniform_double(-duration_variation_max / 2.,duration_variation_max / 2.);
        }
        if(is_less_than(uniform_double(0.,1.),0.1)) {
            v = -v;
        }
        bound_variation(v);
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
        std::vector<double> vec = {
            uniform_double(initial_duration_min,initial_duration_max)
        };
        double v = uniform_double(-duration_variation_max,duration_variation_max);
        for(unsigned j=0; j<nb_time_steps; ++j) {
            append_duration(vec,v);
            if(is_less_than(vec.back(),0.)) {
                vec.back() = 0.;
            }
        }
        return vec;
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
        if (fb.open(graph_duration_matrix_path,std::ios::in)) {
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

    /**
     * @brief Build environment from duration matrix
     */
    void build_time_scale_and_map_from_duration_matrix(
        const std::vector<std::vector<std::string>> &dm,
        std::vector<unsigned> &ts,
        std::vector<map_node> &nv) const
    {
        // 0. Extract time scale
        unsigned tref = std::stoul(dm.at(0).at(2));
        for(unsigned j=2; j<dm.at(0).size(); ++j) {
            ts.push_back(std::stoul(dm.at(0).at(j)) - tref);
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
