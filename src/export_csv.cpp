#include "export_csv.h"
#include <fstream>
#include <cmath>
#include <cstdio>

void export_csv(
    const Eigen::MatrixXcd& rx_signal,
    const Eigen::MatrixXcd& compressed,
    const Eigen::MatrixXcd& rdm,
    const std::vector<Detection>& detections,
    const RadarParams& params,
    const std::string& output_dir)
{
    int spp = params.samples_per_pulse;
    int np  = params.num_pulses;

    // Build axis vectors
    std::vector<double> range_axis(spp);
    for (int i = 0; i < spp; ++i) {
        range_axis[i] = i * RadarParams::c / (2.0 * params.fs);
    }

    std::vector<double> vel_axis(np);
    for (int j = 0; j < np; ++j) {
        vel_axis[j] = -params.max_velocity
            + j * 2.0 * params.max_velocity / (np - 1);
    }

    // Raw received signal of first pulse
    {
        std::string path = output_dir + "/raw_signal.csv";
        std::ofstream f(path);
        f << "range_km,amplitude\n";
        for (int i = 0; i < spp; ++i) {
            f << range_axis[i] / 1000.0 << ","
              << rx_signal(i, 0).real() << "\n";
        }
        std::printf("Wrote %s\n", path.c_str());
    }

    // Compressed signal of first pulse
    {
        std::string path = output_dir + "/compressed_signal.csv";
        std::ofstream f(path);
        f << "range_km,amplitude\n";
        for (int i = 0; i < spp; ++i) {
            f << range_axis[i] / 1000.0 << ","
              << std::abs(compressed(i, 0)) << "\n";
        }
        std::printf("Wrote %s\n", path.c_str());
    }

    // Range doppler map
    {
        std::string path = output_dir + "/rdm.csv";
        std::ofstream f(path);

        // Header
        f << "range_km";
        for (int j = 0; j < np; ++j) {
            f << "," << vel_axis[j];
        }
        f << "\n";

        // Data
        for (int i = 0; i < spp; ++i) {
            f << range_axis[i] / 1000.0;
            for (int j = 0; j < np; ++j) {
                double mag = std::abs(rdm(i, j));
                double db = (mag > 0) ? 20.0 * std::log10(mag) : -200.0;
                f << "," << db;
            }
            f << "\n";
        }
        std::printf("Wrote %s\n", path.c_str());
    }

    // Show detections
    {
        std::string path = output_dir + "/detections.csv";
        std::ofstream f(path);
        f << "range_km,velocity_mps,power\n";
        for (const auto& det : detections) {
            f << range_axis[det.range_idx] / 1000.0 << ","
              << vel_axis[det.doppler_idx] << ","
              << det.power << "\n";
        }
        std::printf("Wrote %s\n", path.c_str());
    }

    // Show ground truth
    {
        std::string path = output_dir + "/ground_truth.csv";
        std::ofstream f(path);
        f << "range_km,velocity_mps,rcs\n";
        for (const auto& tgt : params.targets) {
            f << tgt.range_m / 1000.0 << ","
              << tgt.velocity_mps << ","
              << tgt.rcs_m2 << "\n";
        }
        std::printf("Wrote %s\n", path.c_str());
    }
}
