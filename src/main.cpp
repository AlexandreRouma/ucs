#include <stdio.h>
#include <windows.h>
#include "utils/wav.h"
#include "dsp/mod/gfsk.h"
#include "dsp/taps/from_array.h"
#include "dsp/filter/fir.h"
#include "dsp/channel/frequency_xlator.h"
#include "dsp/convert/complex_to_real.h"
#include "dsp/convert/mono_to_stereo.h"
#include "dsp/buffer/packer.h"
#include <RtAudio.h>


#define SAMPLERATE  48e3
#define SYMBOLRATE  1200.0
#define DEVIATION   (SYMBOLRATE/2.0f)
#define TX_FREQ     19e3

#define FRAME_SIZE  1200

#define QPSK_AMP    0.70710678118f

int callback(void* outputBuffer, void* inputBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status, void* userData) {
    dsp::stream<dsp::stereo_t>* stream = (dsp::stream<dsp::stereo_t>*)userData;
    int count = stream->read();
    if (count < 0) { return 0; }
    memcpy(outputBuffer, stream->readBuf, nBufferFrames * sizeof(dsp::stereo_t));
    stream->flush();
    return 0;
}
RtAudio audio; 

int main() {
    // Create DSP chain
    dsp::stream<float> input;
    dsp::mod::GFSK mod(&input, SYMBOLRATE, SAMPLERATE, 0.6, 31, DEVIATION);
    dsp::channel::FrequencyXlator xlate(&mod.out, TX_FREQ, SAMPLERATE);
    dsp::convert::ComplexToReal c2r(&xlate.out);
    dsp::convert::MonoToStereo m2s(&c2r.out);
    dsp::buffer::Packer<dsp::stereo_t> packer(&m2s.out, 1000);

    // Confiure audio device
    RtAudio::StreamParameters parameters;
    parameters.deviceId = audio.getDefaultOutputDevice();
    parameters.nChannels = 2;
    unsigned int bufferFrames = SAMPLERATE / 60;
    RtAudio::StreamOptions opts;
    opts.flags = RTAUDIO_MINIMIZE_LATENCY;
    opts.streamName = "UCS";

    // Open stream
    audio.openStream(&parameters, NULL, RTAUDIO_FLOAT32, SAMPLERATE, &bufferFrames, &callback, &packer.out, &opts);
    packer.setSampleCount(bufferFrames);
    audio.startStream();

    // Start DSP
    mod.start();
    xlate.start();
    c2r.start();
    m2s.start();
    packer.start();

    // Write data to input
    while (true) {
        for (int i = 0; i < FRAME_SIZE; i++) {
            input.writeBuf[i] = (rand() % 2) ? 1.0f : -1.0f;
        }
        input.swap(FRAME_SIZE);
        printf("Swapped\n");
    }
    
    return 0;
}