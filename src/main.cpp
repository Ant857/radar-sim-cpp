#include "radar_params.h"
#include "chirp.h"
#include "simulate.h"
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
 
    return 0;
}
