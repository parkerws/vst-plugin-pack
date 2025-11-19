#pragma once
#include <JuceHeader.h>

/**
 * AmpSimDSP - Core DSP engine for Orange-style tube amp simulation
 *
 * Signal chain:
 * Input -> Preamp Gain -> Tone Stack (EQ) -> Drive/Saturation -> Power Amp -> Output
 *
 * Features:
 * - Preamp gain stage with soft clipping
 * - 3-band EQ (Bass, Middle, Treble) modeled after tube amp tone stacks
 * - Adjustable drive with multiple waveshaping algorithms
 * - Power amp compression and saturation
 * - Presence control for high-frequency character
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

    // Helper functions
    void updateFilters();
    float applySaturation(float input, float driveAmount);
    float softClip(float input);
    float tanh_approx(float x);  // Fast tanh approximation
};
