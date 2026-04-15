#ifndef DOPPLER_H
#define DOPPLER_H

#include "radar_params.h"
#include <Eigen/Dense>

// Doppler processing, FFT across pulses for each range bin
// Creates range-Doppler map with zero-Doppler centered
// Input --> compressed [samples_per_pulse x num_pulses]
// Output --> rdm       [samples_per_pulse x num_pulses]
Eigen::MatrixXcd doppler_process(
    const Eigen::MatrixXcd& compressed,
    const RadarParams& params
);

#endif
