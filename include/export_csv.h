#ifndef EXPORT_CSV_H
#define EXPORT_CSV_H

#include "radar_params.h"
#include "cfar.h"
#include <Eigen/Dense>
#include <vector>
#include <string>

// Export all radar data to CSV files for plotting
// These CSV files are used by python matplotlib script
void export_csv(
    const Eigen::MatrixXcd& rx_signal,
    const Eigen::MatrixXcd& compressed,
    const Eigen::MatrixXcd& rdm,
    const std::vector<Detection>& detections,
    const RadarParams& params,
    const std::string& output_dir = "."
);

#endif // EXPORT_CSV_H
