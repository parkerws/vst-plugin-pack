#pragma once
#include <JuceHeader.h>

/**
 * AmpSimDSP - Core DSP engine for Orange-style tube amp simulation
 *
 * REAL AMP SIMULATION including:
 *
 * Signal chain:
 * Input -> Tube Preamp (12AX7 model) -> Tone Stack (EQ) -> Tube Drive Stage ->
 * Power Amp (EL84 model) -> Output Transformer -> Cabinet IR Convolution -> Output
 *
 * Features:
 * - Multi-stage tube preamp modeling (12AX7 triode characteristics)
 * - Realistic tone stack (James-Baxandall style)
 * - Tube drive stage with asymmetric clipping
 * - Power amp modeling (EL84 push-pull with compression/sag)
 * - Output transformer saturation
 * - Cabinet impulse response convolution (4x12 Orange cabinet simulation)
 * - Presence control for negative feedback modeling
 */
class AmpSimDSP
{
public:
    AmpSimDSP();
    ~AmpSimDSP();

    // Prepare for playback
    void prepare(double sampleRate, int samplesPerBlock, int numChannels);

    // Process audio block
    void processBlock(juce::AudioBuffer<float>& buffer);

    // Reset DSP state
    void reset();

    // Parameter setters (0.0 to 1.0 normalized)
    void setPreampGain(float gain);      // Input gain (0-10)
    void setBass(float bass);            // Bass EQ (-12dB to +12dB)
    void setMiddle(float middle);        // Middle EQ (-12dB to +12dB)
    void setTreble(float treble);        // Treble EQ (-12dB to +12dB)
    void setDrive(float drive);          // Overdrive amount (0-10)
    void setPresence(float presence);    // High-frequency sparkle
    void setMasterVolume(float volume);  // Output volume

private:
    // Sample rate
    double currentSampleRate = 44100.0;

    // Parameter values (normalized 0-1)
    float preampGain = 0.5f;
    float bass = 0.5f;
    float middle = 0.5f;
    float treble = 0.5f;
    float drive = 0.3f;
    float presence = 0.5f;
    float masterVolume = 0.7f;

    // EQ filters using JUCE DSP
    using Filter = juce::dsp::IIR::Filter<float>;
    using FilterCoefs = juce::dsp::IIR::Coefficients<float>;

    juce::dsp::ProcessorDuplicator<Filter, FilterCoefs> bassFilter;
    juce::dsp::ProcessorDuplicator<Filter, FilterCoefs> middleFilter;
    juce::dsp::ProcessorDuplicator<Filter, FilterCoefs> trebleFilter;
    juce::dsp::ProcessorDuplicator<Filter, FilterCoefs> presenceFilter;

    // DC blocking filter to remove DC offset from saturation
    juce::dsp::ProcessorDuplicator<Filter, FilterCoefs> dcBlocker;

    // Cabinet IR convolution (REAL amp simulation!)
    juce::dsp::Convolution cabinetConvolution;
    bool cabinetLoaded = false;

    // Oversampling for better nonlinear processing
    using Oversample = juce::dsp::Oversampling<float>;
    std::unique_ptr<Oversample> oversampler;

    // Power amp state (for sag simulation)
    float powerAmpEnvelope = 0.0f;

    // Helper functions
    void updateFilters();
    void loadCabinetIR();

    // Tube modeling
    float tubePreamp(float input, float gain);      // 12AX7 preamp stage
    float tubePowerAmp(float input);                // EL84 power amp
    float outputTransformer(float input);           // Output transformer saturation

    // Legacy (now enhanced)
    float applySaturation(float input, float driveAmount);
    float softClip(float input);
    float tanh_approx(float x);  // Fast tanh approximation
};
