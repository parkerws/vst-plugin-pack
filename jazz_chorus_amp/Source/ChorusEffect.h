#pragma once
#include <JuceHeader.h>

/**
 * ChorusEffect - Stereo chorus modulation effect
 *
 * Implements the iconic Jazz Chorus stereo chorus effect:
 * - Dual LFO modulated delay lines
 * - Stereo width control
 * - Rate and depth controls
 * - Warm analog-style modulation
 *
 * The Jazz Chorus chorus is known for its wide, shimmering sound
 * that doesn't sound overly processed.
 */
class ChorusEffect
{
public:
    ChorusEffect();
    ~ChorusEffect();

    void prepare(double sampleRate, int samplesPerBlock, int numChannels);
    void reset();
    void processBlock(juce::AudioBuffer<float>& buffer);

    // Parameters (0.0 to 1.0 normalized)
    void setRate(float rate);           // LFO speed (0.1Hz - 5Hz)
    void setDepth(float depth);         // Modulation depth
    void setMix(float mix);             // Dry/wet mix
    void setEnabled(bool enabled);      // Bypass control

private:
    double currentSampleRate = 44100.0;
    bool isEnabled = true;

    // Parameters
    float rate = 0.5f;
    float depth = 0.5f;
    float mix = 0.5f;

    // Delay buffer for chorus effect
    static constexpr int maxDelayBufferSize = 8192;
    juce::AudioBuffer<float> delayBuffer;
    int delayBufferWritePos = 0;

    // LFO (Low Frequency Oscillator) for modulation
    float lfoPhase = 0.0f;
    float lfoPhaseIncrement = 0.0f;

    // Smoothing filters to prevent zipper noise
    juce::SmoothedValue<float> smoothedRate;
    juce::SmoothedValue<float> smoothedDepth;
    juce::SmoothedValue<float> smoothedMix;

    // Helper functions
    void updateLFOIncrement();
    float getLFOSample(float phase);
    float getInterpolatedSample(int channel, float delayInSamples);
};
