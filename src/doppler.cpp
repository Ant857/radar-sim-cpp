#include "doppler.h"
#include <fftw3.h>
#include <cstring>

Eigen::MatrixXcd doppler_process(
    const Eigen::MatrixXcd& compressed,
    const RadarParams& params)
{
    int spp = params.samples_per_pulse;
    int np  = params.num_pulses;

    Eigen::MatrixXcd rdm(spp, np);

    // FFTW buffers for single row
    fftw_complex* in  = fftw_alloc_complex(np);
    fftw_complex* out = fftw_alloc_complex(np);
    fftw_plan plan = fftw_plan_dft_1d(np, in, out, FFTW_FORWARD, FFTW_ESTIMATE);

    // For each range bin
    // FFT across pulses
    for (int i = 0; i < spp; ++i) {
        for (int j = 0; j < np; ++j) {
            in[j][0] = compressed(i, j).real();
            in[j][1] = compressed(i, j).imag();
        }

        fftw_execute(plan);

        // fftshift
        // swap left and right halves to center zero-Doppler
        int half = np / 2;
        for (int j = 0; j < np; ++j) {
            int shifted = (j + half) % np;
            rdm(i, j) = std::complex<double>(out[shifted][0], out[shifted][1]);
        }
    }

    fftw_destroy_plan(plan);
    fftw_free(in);
    fftw_free(out);

    return rdm;
}
