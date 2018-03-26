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

#endif // EXCEPTIONS_HPP_
