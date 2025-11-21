#pragma once
#include <JuceHeader.h>
#include "ChorusEffect.h"

/**
 * JazzChorusDSP - Core DSP engine for Jazz Chorus amp simulation
 *
 * REAL SOLID-STATE AMP SIMULATION including:
 *
 * Signal chain:
 * Input -> Clean Solid-State Preamp -> EQ (Bright voicing) ->
 * Stereo Chorus -> Cabinet IR Convolution -> Output
 *
 * The Jazz Chorus is famous for:
 * - Ultra-clean solid-state preamp with high headroom
 * - Bright, sparkly tone character
 * - Iconic stereo chorus effect
 * - Minimal distortion (designed to stay clean at high volumes)
 *
 * Features:
 * - Solid-state preamp modeling (clean with subtle transistor character)
 * - Bright EQ voicing (treble, middle, bass)
 * - Stereo chorus effect (rate, depth, mix)
 * - Cabinet impulse response convolution (2x12 open-back simulation)
 * - Volume control
 * - Chorus on/off switch
 */
class JazzChorusDSP
{
public:
    JazzChorusDSP();
    ~JazzChorusDSP();

    // Prepare for playback
    void prepare(double sampleRate, int samplesPerBlock, int numChannels);

    // Process audio block
    void processBlock(juce::AudioBuffer<float>& buffer);

    // Reset DSP state
    void reset();

    // Parameter setters (0.0 to 1.0 normalized)
    void setVolume(float volume);        // Overall volume
    void setBass(float bass);            // Bass EQ (-12dB to +12dB)
    void setMiddle(float middle);        // Middle EQ (-12dB to +12dB)
    void setTreble(float treble);        // Treble EQ (-12dB to +12dB)
    void setBright(float bright);        // Bright switch/boost
    void setChorusRate(float rate);      // Chorus LFO rate
    void setChorusDepth(float depth);    // Chorus modulation depth
    void setChorusMix(float mix);        // Chorus wet/dry mix
    void setChorusEnabled(bool enabled); // Chorus on/off

private:
    // Sample rate
    double currentSampleRate = 44100.0;

    // Parameter values (normalized 0-1)
    float volume = 0.7f;
    float bass = 0.5f;
    float middle = 0.5f;
    float treble = 0.6f;     // Slightly boosted by default (Jazz Chorus character)
    float bright = 0.5f;

    // EQ filters using JUCE DSP
    using Filter = juce::dsp::IIR::Filter<float>;
    using FilterCoefs = juce::dsp::IIR::Coefficients<float>;

    juce::dsp::ProcessorDuplicator<Filter, FilterCoefs> bassFilter;
    juce::dsp::ProcessorDuplicator<Filter, FilterCoefs> middleFilter;
    juce::dsp::ProcessorDuplicator<Filter, FilterCoefs> trebleFilter;
    juce::dsp::ProcessorDuplicator<Filter, FilterCoefs> brightFilter;

    // Chorus effect
    ChorusEffect chorus;

    // Cabinet IR convolution (REAL amp simulation!)
    juce::dsp::Convolution cabinetConvolution;
    bool cabinetLoaded = false;

    // Helper functions
    void updateFilters();
    void loadCabinetIR();
    float applyCleanSaturation(float input); // Subtle solid-state character
};
