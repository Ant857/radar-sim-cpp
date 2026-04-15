#include "radar_params.h"
#include "chirp.h"
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
 
    return 0;
}
