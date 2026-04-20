#include "cfar.h"
#include <cmath>
#include <vector>

std::vector<Detection> cfar_detect(
    const Eigen::MatrixXcd& rdm,
    const RadarParams& params)
{
    int num_range   = static_cast<int>(rdm.rows());
    int num_doppler = static_cast<int>(rdm.cols());

    int num_train = params.cfar_train;
    int num_guard = params.cfar_guard;
    double alpha  = params.cfar_alpha;
    int window    = num_train + num_guard;

    // Precompute power map: |rdm|^2
    Eigen::MatrixXd rdm_power(num_range, num_doppler);
    for (int i = 0; i < num_range; ++i) {
        for (int j = 0; j < num_doppler; ++j) {
            rdm_power(i, j) = std::norm(rdm(i, j));
        }
    }

    // Slide the 2D CFAR window
    std::vector<Detection> raw_detections;

    for (int i = window; i < num_range - window; ++i) {
        for (int j = window; j < num_doppler - window; ++j) {
            double cut = rdm_power(i, j);

            // Sum full window box
            double full_sum = 0;
            for (int ri = i - window; ri <= i + window; ++ri) {
                for (int ci = j - window; ci <= j + window; ++ci) {
                    full_sum += rdm_power(ri, ci);
                }
            }

            // Sum guard box (to subtract out)
            double guard_sum = 0;
            for (int ri = i - num_guard; ri <= i + num_guard; ++ri) {
                for (int ci = j - num_guard; ci <= j + num_guard; ++ci) {
                    guard_sum += rdm_power(ri, ci);
                }
            }

            int full_count  = (2 * window + 1) * (2 * window + 1);
            int guard_count = (2 * num_guard + 1) * (2 * num_guard + 1);
            int train_count = full_count - guard_count;

            double noise_est = (full_sum - guard_sum) / train_count;

            if (cut > alpha * noise_est) {
                raw_detections.push_back({i, j, cut});
            }
        }
    }

    // Cluster nearby detections — keep the strongest in each cluster
    std::vector<bool> used(raw_detections.size(), false);
    std::vector<Detection> clean;

    for (size_t i = 0; i < raw_detections.size(); ++i) {
        if (used[i]) continue;

        // Find all detections within 5 cells
        Detection best = raw_detections[i];
        used[i] = true;

        for (size_t k = i + 1; k < raw_detections.size(); ++k) {
            if (used[k]) continue;
            if (std::abs(raw_detections[k].range_idx - raw_detections[i].range_idx) < 5 &&
                std::abs(raw_detections[k].doppler_idx - raw_detections[i].doppler_idx) < 5) {
                used[k] = true;
                if (raw_detections[k].power > best.power) {
                    best = raw_detections[k];
                }
            }
        }

        clean.push_back(best);
    }

    return clean;
}
