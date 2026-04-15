#include "radar_params.h"
#include "chirp.h"
#include "simulate.h"
#include "matched_filter.h"
#include "doppler.h"
#include <cstdio>
#include <complex>
#include <vector>
 
void print_radar_params(const RadarParams& p);
 
int main() {
	// Radar Params
    RadarParams params;
    print_radar_params(params);
    
    // Generate chirp
    auto pulse = generate_chirp(params);
    std::printf("\n=== Chirp Waveform ===\n");
    std::printf("Pulse length:    %zu samples\n", pulse.size());
    std::printf("Chirp rate:      %.2e Hz/s\n", params.bw / params.pulse_width);
    std::printf("First sample:    (%.4f, %.4f)\n", pulse.front().real(), pulse.front().imag());
    std::printf("Last sample:     (%.4f, %.4f)\n", pulse.back().real(), pulse.back().imag());
 
    // Verify unit magnitude, all samples should have |pulse| = 1
    double mag = std::abs(pulse[pulse.size() / 2]);
    std::printf("Mid-pulse |mag|: %.6f (should be 1.0)\n", mag);
    
    // Simulate received signal
    auto rx_signal = simulate_returns(pulse, params);
    std::printf("\n=== Received Signal ===\n");
    std::printf("Matrix size:     %ld rows x %ld cols\n",
			rx_signal.rows(), rx_signal.cols());
 
    // Show peak amplitude per target
    // Targets should create visible peaks at their delay positions
    for (size_t t = 0; t < params.targets.size(); ++t) {
        int delay = static_cast<int>(std::round(2.0 * params.targets[t].range_m / RadarParams::c * params.fs));
        double peak = std::abs(rx_signal(delay, 0));
        std::printf("Target %zu (range %.0f m): sample %d, |amplitude| = %.2e\n",
        		t + 1, params.targets[t].range_m, delay, peak);
    }
 
    // Noise floor estimate, from last 100 samples of pulse 1
    double noise_sum = 0;
    int n_samples = 100;
    for (int i = rx_signal.rows() - n_samples; i < rx_signal.rows(); ++i) {
        noise_sum += std::abs(rx_signal(i, 0));
    }
    std::printf("Avg noise floor: %.2e\n", noise_sum / n_samples);
    
    // Matched filter, range compression
    auto compressed = matched_filter(rx_signal, pulse, params);
    std::printf("\n=== After Matched Filtering ===\n");
 
    // Find peaks in first pulse, these should correspond to target ranges
    for (size_t t = 0; t < params.targets.size(); ++t) {
        int delay = static_cast<int>(std::round(
            2.0 * params.targets[t].range_m / RadarParams::c * params.fs));
 
        double raw_amp = std::abs(rx_signal(delay, 0));
        double comp_amp = std::abs(compressed(delay, 0));
 
        std::printf("Target %zu (%.0f m): raw |amp| = %.2e, compressed |amp| = %.2e\n",
                    t + 1, params.targets[t].range_m, raw_amp, comp_amp);
    }
    
    // Doppler processing
    auto rdm = doppler_process(compressed, params);
    std::printf("\n=== Range-Doppler Map ===\n");
    std::printf("RDM size: %ld x %ld\n", rdm.rows(), rdm.cols());
 
    // Find peak in RDM for each target
    std::vector<double> range_axis(params.samples_per_pulse);
    for (int i = 0; i < params.samples_per_pulse; ++i) {
        range_axis[i] = i * RadarParams::c / (2.0 * params.fs);
    }
 
    std::vector<double> vel_axis(params.num_pulses);
    for (int j = 0; j < params.num_pulses; ++j) {
        vel_axis[j] = -params.max_velocity
            + j * 2.0 * params.max_velocity / (params.num_pulses - 1);
    }
 
    // Search for peaks near each true target location
    std::printf("\n=== Target Detection Check ===\n");
    for (size_t t = 0; t < params.targets.size(); ++t) {
        double tgt_range = params.targets[t].range_m;
        double tgt_vel   = params.targets[t].velocity_mps;
        double best_power = 0;
        int best_r = 0, best_d = 0;
 
        for (int i = 0; i < params.samples_per_pulse; ++i) {
            if (std::abs(range_axis[i] - tgt_range) > 500) continue;
            for (int j = 0; j < params.num_pulses; ++j) {
                double pwr = std::norm(rdm(i, j));  // |z|^2
                if (pwr > best_power) {
                    best_power = pwr;
                    best_r = i;
                    best_d = j;
                }
            }
        }
 
        std::printf("Target %zu: true (%.0f m, %.1f m/s) → peak at (%.0f m, %.1f m/s), power = %.2e\n",
                    t + 1, tgt_range, tgt_vel,
                    range_axis[best_r], vel_axis[best_d], best_power);
    }
 
    return 0;
}
