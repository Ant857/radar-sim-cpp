#ifndef CFAR_H
#define CFAR_H

#include "radar_params.h"
#include <Eigen/Dense>
#include <vector>
#include <utility>

struct Detection {
    int range_idx;
    int doppler_idx;
    double power;
};

// 2D CA-CFAR detection on the range-Doppler map
// Returns a list of detections after clustering nearby hits
std::vector<Detection> cfar_detect(
    const Eigen::MatrixXcd& rdm,
    const RadarParams& params
);

#endif // CFAR_H
