#ifndef MATCHED_FILTER_H
#define MATCHED_FILTER_H

#include "radar_params.h"
#include <Eigen/Dense>
#include <vector>
#include <complex>

// Matched filtering, range compression, using frequency-domain convolution
// Input --> rx_signal [samples_per_pulse x num_pulses]
// Output --> compressed [samples_per_pulse x num_pulses]
Eigen::MatrixXcd matched_filter(
    const Eigen::MatrixXcd& rx_signal,
    const std::vector<std::complex<double>>& pulse,
    const RadarParams& params
);

#endif
