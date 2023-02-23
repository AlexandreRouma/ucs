#pragma once
#include "dsp/mod/gfsk.h"
#include "dsp/taps/from_array.h"
#include "dsp/filter/fir.h"
#include "dsp/channel/frequency_xlator.h"
#include "dsp/convert/complex_to_real.h"
#include "dsp/convert/mono_to_stereo.h"
#include "dsp/buffer/packer.h"
#include "dsp/convert/real_to_complex.h"
#include "dsp/filter/fir.h"
#include "dsp/mod/quadrature.h"
#include <RtAudio.h>

class TX {
public:
    TX(double samplerate, double symbolrate, double deviation, double freq);

    void start();
    void stop();

    dsp::stream<float> input;

private:
    static int callback(void* out, void* in, unsigned int len, double time, RtAudioStreamStatus status, void* ctx);

    double samplerate;
    double symbolrate;
    double deviation;
    double freq;
    bool running = false;

    //dsp::mod::GFSK mod;
    dsp::convert::RealToComplex r2c;
    dsp::filter::FIR<dsp::complex_t, dsp::complex_t> fir;
    dsp::tap<dsp::complex_t> bandpass;
    dsp::channel::FrequencyXlator xlate;
    dsp::convert::ComplexToReal c2r;
    dsp::convert::MonoToStereo m2s;
    dsp::buffer::Packer<dsp::stereo_t> packer;
    RtAudio audio; 
};