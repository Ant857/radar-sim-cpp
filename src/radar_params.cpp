#include "radar_params.h"
#include <cstdio>

void print_radar_params(const RadarParams& p) {
    std::printf("\n=== Radar System Performance ===\n");
    std::printf("Carrier frequency:      %.1f GHz\n", p.fc / 1e9);
    std::printf("Bandwidth:              %.1f MHz\n", p.bw / 1e6);
    std::printf("Sampling rate:          %.1f MHz\n", p.fs / 1e6);
    std::printf("Pulse width:            %.1f us\n",  p.pulse_width * 1e6);
    std::printf("PRF:                    %.1f kHz\n", p.prf / 1e3);
    std::printf("Pulses per CPI:         %d\n",       p.num_pulses);
    std::printf("Range resolution:       %.1f m\n",   p.range_res);
    std::printf("Max unambiguous range:  %.1f km\n",  p.max_range / 1e3);
    std::printf("Velocity resolution:    %.1f m/s\n", p.vel_res);
    std::printf("Max unambiguous vel:    %.1f m/s\n", p.max_velocity);
    std::printf("Samples per pulse:      %d\n",       p.samples_per_pulse);
    std::printf("Number of targets:      %zu\n",      p.targets.size());
}
