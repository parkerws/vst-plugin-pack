#pragma once
#include <JuceHeader.h>

/**
 * Bass tone presets/effects chains
 */
enum class BassTone
{
    DI,          // Direct In - clean, unprocessed
    AmpSim,      // Amp simulation with warmth
    Compressed,  // Heavy compression for even dynamics
    Bright,      // Bright EQ for cutting through mix
    Vintage      // Warm, vintage tone with rolled-off highs
};

/**
 * BassEffects - Effects processing chain for bass guitar
 *
 * Features:
 * - Multiple tone presets (DI, Amp Sim, Compressed, etc.)
 * - Compression for even dynamics
 * - EQ (Bass, Mid, Treble)
 * - Amp simulation with saturation
 * - Output level control
 */
class BassEffects
{
public:
    BassEffects();
    ~BassEffects();

    void prepare(double sampleRate, int samplesPerBlock, int numChannels);
    void reset();
    void processBlock(juce::AudioBuffer<float>& buffer);

    // Tone preset selection
    void setTone(BassTone tone);
    BassTone getCurrentTone() const { return currentTone; }

    // EQ controls (0.0 to 1.0 normalized)
    void setBass(float bass);
    void setMid(float mid);
    void setTreble(float treble);

    // Dynamics
    void setCompression(float compression);  // 0 = off, 1 = heavy

    // Output
    void setOutputGain(float gain);

private:
    double currentSampleRate = 44100.0;
    BassTone currentTone = BassTone::DI;

    // Parameters
    float bassLevel = 0.5f;
    float midLevel = 0.5f;
    float trebleLevel = 0.5f;
    float compressionAmount = 0.0f;
    float outputGain = 0.7f;

    // EQ filters
    using Filter = juce::dsp::IIR::Filter<float>;
    using FilterCoefs = juce::dsp::IIR::Coefficients<float>;

    juce::dsp::ProcessorDuplicator<Filter, FilterCoefs> bassFilter;
    juce::dsp::ProcessorDuplicator<Filter, FilterCoefs> midFilter;
    juce::dsp::ProcessorDuplicator<Filter, FilterCoefs> trebleFilter;

    // Compressor (simple dynamics processing)
    float previousSample = 0.0f;
    float envelope = 0.0f;

    // Helper functions
    void updateFilters();
    void updateTonePreset();
    float applyAmpSaturation(float input);
};
