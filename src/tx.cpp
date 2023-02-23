#include "tx.h"
#include "dsp/taps/band_pass.h"

TX::TX(double samplerate, double symbolrate, double deviation, double freq) {
    this->samplerate = samplerate;
    this->symbolrate = symbolrate;
    this->deviation = deviation;
    this->freq = freq;

    // Init DSP
    //mod.init(&input, symbolrate, samplerate, 0.6, 31, deviation);
    
    r2c.init(&input);
    bandpass = dsp::taps::bandPass<dsp::complex_t>(100.0, 2900.0, 100.0, samplerate);
    fir.init(&r2c.out, bandpass);
    xlate.init(&fir.out, freq, samplerate);
    c2r.init(&xlate.out);
    m2s.init(&c2r.out);
    packer.init(&m2s.out, 1000);

    // Confiure audio device
    RtAudio::StreamParameters parameters;
    parameters.deviceId = audio.getDefaultOutputDevice();
    parameters.nChannels = 2;
    unsigned int bufferFrames = samplerate / 60.0;
    RtAudio::StreamOptions opts;
    opts.flags = RTAUDIO_MINIMIZE_LATENCY;
    opts.streamName = "UCS";

    // Open stream
    audio.openStream(&parameters, NULL, RTAUDIO_FLOAT32, samplerate, &bufferFrames, &callback, this, &opts);
    packer.setSampleCount(bufferFrames);
}

void TX::start() {
    if (running) { return; }

    // Start audio stream
    audio.startStream();

    // Start DSP
    //mod.start();
    r2c.start();
    fir.start();
    xlate.start();
    c2r.start();
    m2s.start();
    packer.start();

    running = true;
}

void TX::stop() {
    if (!running) { return; }

    // Stop audio stream
    packer.out.stopWriter();
    audio.stopStream();
    packer.out.clearWriteStop();

    // Stop DSP
    //mod.stop();
    r2c.stop();
    fir.stop();
    xlate.stop();
    c2r.stop();
    m2s.stop();
    packer.stop();

    running = false;
}

int TX::callback(void* out, void* in, unsigned int len, double time, RtAudioStreamStatus status, void* ctx) {
    TX* _this = (TX*)ctx;
    int count = _this->packer.out.read();
    if (count < 0) { return 0; }
    memcpy(out, _this->packer.out.readBuf, count * sizeof(dsp::stereo_t));
    _this->packer.out.flush();
    return 0;
}
