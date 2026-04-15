#ifndef CHIRP_H
#define CHIRP_H
 
#include "radar_params.h"
#include <vector>
#include <complex>
 
// Generate a linear frequency modulated (LFM) chirp pulse
std::vector<std::complex<double>> generate_chirp(const RadarParams& params);
 
#endif // CHIRP_H
