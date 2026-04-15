#ifndef SIMULATE_H
#define SIMULATE_H

#include "radar_params.h"
#include <vector>
#include <complex>
#include <Eigen/Dense>

// Simulate received radar returns for all targets
// Returns a matrix where rows = fast-time samples, cols = pulse number
Eigen::MatrixXcd simulate_returns(
    const std::vector<std::complex<double>>& pulse,
    const RadarParams& params
);

#endif
