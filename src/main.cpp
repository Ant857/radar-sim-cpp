#include "radar_params.h"
#include "chirp.h"
#include "simulate.h"
#include "matched_filter.h"
#include "doppler.h"
#include "cfar.h"
#include "export_csv.h"
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
    std::printf("Pulse length: %zu samples\n", pulse.size());
 
    // Simulate received signal
    auto rx_signal = simulate_returns(pulse, params);
    std::printf("\n=== Received Signal ===\n");
    std::printf("Matrix size:  %ld x %ld\n", rx_signal.rows(), rx_signal.cols());
 
    // Matched filter
    auto compressed = matched_filter(rx_signal, pulse, params);
    std::printf("\n=== Matched Filter Complete ===\n");
 
    // Doppler processing
    auto rdm = doppler_process(compressed, params);
    std::printf("\n=== Range-Doppler Map ===\n");
    std::printf("RDM size: %ld x %ld\n", rdm.rows(), rdm.cols());
 
    // CFAR detection
    auto detections = cfar_detect(rdm, params);
 
    // Build axis vectors for display
    std::vector<double> range_axis(params.samples_per_pulse);
    for (int i = 0; i < params.samples_per_pulse; ++i) {
        range_axis[i] = i * RadarParams::c / (2.0 * params.fs);
    }
 
    std::vector<double> vel_axis(params.num_pulses);
    for (int j = 0; j < params.num_pulses; ++j) {
        vel_axis[j] = -params.max_velocity
            + j * 2.0 * params.max_velocity / (params.num_pulses - 1);
    }
 
    // Print detections
    std::printf("\n=== CFAR Detections ===\n");
    if (detections.empty()) {
        std::printf("No targets detected. Try lowering cfar_alpha.\n");
    } else {
        for (size_t d = 0; d < detections.size(); ++d) {
            double r = range_axis[detections[d].range_idx];
            double v = vel_axis[detections[d].doppler_idx];
            std::printf("Detection %zu: Range = %.0f m, Velocity = %.1f m/s, Power = %.2e\n",
                        d + 1, r, v, detections[d].power);
        }
    }
 
    // Print ground truth for comparison
    std::printf("\n=== Ground Truth ===\n");
    for (size_t t = 0; t < params.targets.size(); ++t) {
        std::printf("Target %zu:    Range = %.0f m, Velocity = %.1f m/s\n",
                    t + 1, params.targets[t].range_m, params.targets[t].velocity_mps);
    }
 
 	// Export data to CSV for plotting using matplotlib
 	std::printf("\n=== Exporting CSV Data ===\n");
    export_csv(rx_signal, compressed, rdm, detections, params, "output");
 
    std::printf("\nRun: python3 plot_results.py output\n");
 
    return 0;
}
