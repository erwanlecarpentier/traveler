#ifndef EXCEPTIONS_HPP_
#define EXCEPTIONS_HPP_

/**
 * @brief Did not succeed in opening duration matrix file
 */
struct duration_matrix_file_exception : std::exception {
    explicit duration_matrix_file_exception() noexcept {}
    virtual ~duration_matrix_file_exception() noexcept {}
    virtual const char * what() const noexcept override {
        return "in parameters: did not succeed in opening duration matrix file.\n";
    }
};

/**
 * @brief Illegal action
 */
struct illegal_action_exception : std::exception {
    explicit illegal_action_exception() noexcept {}
    virtual ~illegal_action_exception() noexcept {}
    virtual const char * what() const noexcept override {
        return "attempted action is illegal at that state (e.g. no edge between nodes).\n";
    }
};

/**
 * @brief No parameters path
 */
struct no_parameters_path_exception : std::exception {
    explicit no_parameters_path_exception() noexcept {}
    virtual ~no_parameters_path_exception() noexcept {}
    virtual const char * what() const noexcept override {
        return "add the path to the parameters in the command line.\n";
    }
};

/**
 * @brief No action (dead end)
 */
struct no_action_exception : std::exception {
    explicit no_action_exception() noexcept {}
    virtual ~no_action_exception() noexcept {}
    virtual const char * what() const noexcept override {
        return "no actions are available (dead end).\n";
    }
};

/**
 * @brief Nonexistent node
 */
struct nonexistent_node_exception : std::exception {
    explicit nonexistent_node_exception() noexcept {}
    virtual ~nonexistent_node_exception() noexcept {}
    virtual const char * what() const noexcept override {
        return "node does not exist (e.g. wrong name).\n";
    }
};

/**
 * @brief Wrong syntax configuration file exception
 *
 * Exception for wrong syntax in configuration file.
 */
struct wrong_syntax_configuration_file_exception : std::exception {
    explicit wrong_syntax_configuration_file_exception() noexcept {}
    virtual ~wrong_syntax_configuration_file_exception() noexcept {}

    virtual const char * what() const noexcept override {
        return "in config file: please make sure that the syntaxes match.\n";
    }
};

#endif // EXCEPTIONS_HPP_
