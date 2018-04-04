#ifndef LINEAR_ALGEBRA_HPP_
#define LINEAR_ALGEBRA_HPP_

#include <Eigen/Dense>

/**
 * @brief Build a polynomial feature matrix from a vector of input
 */
Eigen::MatrixXd build_poly_feature_matrix(
    const std::vector<double> &input,
    unsigned degree)
{
	Eigen::MatrixXd phi(input.size(),degree+1);
	for(unsigned i=0; i<input.size(); ++i) {
        double value = 1.;
        for(unsigned j=0; j<degree+1; ++j) {
            phi(i,j) = value;
            value *= input.at(i);
        }
	}
	return phi;
}

/**
 * @brief Build a vector from a vector of output
 */
Eigen::VectorXd build_output_vector(const std::vector<double> &output) {
	Eigen::VectorXd y(output.size());
	for(unsigned i=0; i<output.size(); ++i) {
		y(i) = output.at(i);
	}
	return y;
}

/**
 * @brief Build the left hand side matrix of a linear problem
 */
Eigen::MatrixXd build_lmatrix(const Eigen::MatrixXd &phi, double lambda) {
	return phi.transpose()*phi + lambda*Eigen::MatrixXd::Identity(phi.cols(),phi.cols());
}

/**
 * @brief Build the right hand side vector of a linear problem
 */
Eigen::VectorXd build_rvector(const Eigen::MatrixXd &phi, const Eigen::VectorXd &y) {
	return phi.transpose() * y;
}

/**
 * @brief Compute the coefficient of a polynomial regression
 */
Eigen::VectorXd polynomial_regression(
    const std::vector<double> &input,
    const std::vector<double> &output,
    double lambda,
    unsigned degree)
{
    Eigen::MatrixXd phi = build_poly_feature_matrix(input,degree); // feature matrix
	Eigen::VectorXd y = build_output_vector(output); // output vector
	Eigen::MatrixXd a = build_lmatrix(phi,lambda); // left matrix
	Eigen::VectorXd b = build_rvector(phi,y); // right vector
	return a.jacobiSvd(Eigen::ComputeThinU|Eigen::ComputeThinV).solve(b); // solve
}

/**
 * @brief Compute a prediction of a quadratic regression at the given point
 */
double polynomial_regression_prediction_at(double x, const Eigen::VectorXd &coeff) {
    double xcoeff = 1.;
    double value = 0.;
    for(unsigned i=0; i<coeff.rows(); ++i) {
        value += coeff(i) * xcoeff;
        xcoeff *= x;
    }
	return value;
}

#endif // LINEAR_ALGEBRA_HPP_
