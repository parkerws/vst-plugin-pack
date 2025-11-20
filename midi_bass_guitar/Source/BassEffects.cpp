#include "BassEffects.h"

BassEffects::BassEffects()
{
}

BassEffects::~BassEffects()
{
}

void BassEffects::prepare(double sampleRate, int samplesPerBlock, int numChannels)
{
    currentSampleRate = sampleRate;

    // Prepare filters
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = numChannels;

    bassFilter.prepare(spec);
    midFilter.prepare(spec);
    trebleFilter.prepare(spec);

    updateFilters();
    updateTonePreset();
    reset();
}

void BassEffects::reset()
{
    bassFilter.reset();
    midFilter.reset();
    trebleFilter.reset();
    envelope = 0.0f;
    previousSample = 0.0f;
}

void BassEffects::processBlock(juce::AudioBuffer<float>& buffer)
{
    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    // Convert to DSP block for filter processing
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);

    // Stage 1: Apply tone-specific processing
    if (currentTone == BassTone::AmpSim || currentTone == BassTone::Vintage)
    {
        // Apply gentle saturation for amp character
        for (int channel = 0; channel < numChannels; ++channel)
        {
            float* channelData = buffer.getWritePointer(channel);
            for (int sample = 0; sample < numSamples; ++sample)
            {
                channelData[sample] = applyAmpSaturation(channelData[sample]);
            }
        }
    }

    // Stage 2: Compression
    if (compressionAmount > 0.01f || currentTone == BassTone::Compressed)
    {
        float effectiveCompression = compressionAmount;
        if (currentTone == BassTone::Compressed)
            effectiveCompression = juce::jmax(effectiveCompression, 0.7f);

        for (int channel = 0; channel < numChannels; ++channel)
        {
            float* channelData = buffer.getWritePointer(channel);
            for (int sample = 0; sample < numSamples; ++sample)
            {
                channelData[sample] = applyCompression(channelData[sample]);
            }
        }
    }

    // Stage 3: EQ
    bassFilter.process(context);
    midFilter.process(context);
    trebleFilter.process(context);

    // Stage 4: Output gain
    for (int channel = 0; channel < numChannels; ++channel)
    {
        float* channelData = buffer.getWritePointer(channel);
        for (int sample = 0; sample < numSamples; ++sample)
        {
            channelData[sample] *= outputGain;
        }
    }
}

void BassEffects::updateFilters()
{
    const double sampleRate = currentSampleRate;

    // Bass - Low shelf at 80Hz (fundamental bass frequencies)
    // Range: -12dB to +12dB
    float bassGainDb = (bassLevel - 0.5f) * 24.0f;
    *bassFilter.state = *FilterCoefs::makeLowShelf(sampleRate, 80.0, 0.7,
                                                     juce::Decibels::decibelsToGain(bassGainDb));

    // Mid - Peaking filter at 500Hz (punch and clarity)
    // Range: -12dB to +12dB
    float midGainDb = (midLevel - 0.5f) * 24.0f;
    *midFilter.state = *FilterCoefs::makePeakFilter(sampleRate, 500.0, 1.0,
                                                      juce::Decibels::decibelsToGain(midGainDb));

    // Treble - High shelf at 3kHz (brightness and string noise)
    // Range: -12dB to +12dB
    float trebleGainDb = (trebleLevel - 0.5f) * 24.0f;
    *trebleFilter.state = *FilterCoefs::makeHighShelf(sampleRate, 3000.0, 0.7,
                                                        juce::Decibels::decibelsToGain(trebleGainDb));
}

void BassEffects::updateTonePreset()
{
    // Set EQ defaults based on tone preset
    switch (currentTone)
    {
        case BassTone::DI:
            // Clean, flat response
            bassLevel = 0.5f;
            midLevel = 0.5f;
            trebleLevel = 0.5f;
            compressionAmount = 0.0f;
            break;

        case BassTone::AmpSim:
            // Warm, slight bass boost, gentle compression
            bassLevel = 0.6f;
            midLevel = 0.55f;
            trebleLevel = 0.45f;
            compressionAmount = 0.3f;
            break;

        case BassTone::Compressed:
            // Heavy compression, balanced EQ
            bassLevel = 0.5f;
            midLevel = 0.6f;  // Boost mids for clarity
            trebleLevel = 0.5f;
            compressionAmount = 0.8f;
            break;

        case BassTone::Bright:
            // Cut bass, boost treble for modern tone
            bassLevel = 0.4f;
            midLevel = 0.55f;
            trebleLevel = 0.7f;
            compressionAmount = 0.2f;
            break;

        case BassTone::Vintage:
            // Boost lows, roll off highs for classic tone
            bassLevel = 0.65f;
            midLevel = 0.5f;
            trebleLevel = 0.35f;
            compressionAmount = 0.2f;
            break;
    }

    updateFilters();
}

float BassEffects::applyCompression(float input)
{
    // Simple envelope follower compression
    const float attack = 0.01f;
    const float release = 0.1f;

    // Track envelope
    float inputAbs = std::abs(input);
    if (inputAbs > envelope)
        envelope = envelope * (1.0f - attack) + inputAbs * attack;
    else
        envelope = envelope * (1.0f - release) + inputAbs * release;

    // Calculate gain reduction
    float threshold = 0.3f;
    float ratio = 1.0f + (compressionAmount * 4.0f); // 1:1 to 5:1 ratio

    if (envelope > threshold)
    {
        float excess = envelope - threshold;
        float reduction = excess * (1.0f - 1.0f / ratio);
        float minEnvelope = 0.001f;
        float targetGain = (threshold + (excess - reduction)) / juce::jmax(envelope, minEnvelope);

        return input * targetGain;
    }

    return input;
}

float BassEffects::applyAmpSaturation(float input)
{
    // Gentle tube-style saturation for amp sim
    // Much more subtle than guitar amp saturation
    float driven = input * 1.5f;

    // Soft clipping with asymmetry
    if (driven > 0.0f)
        driven = std::tanh(driven * 0.8f) / 0.8f;
    else
        driven = std::tanh(driven * 0.9f) / 0.9f;

    return driven * 0.7f;
}

// Parameter setters
void BassEffects::setTone(BassTone tone)
{
    currentTone = tone;
    // updateTonePreset(); // Do not reset EQ/compression on tone change
}

void BassEffects::setBass(float bass)
{
    bassLevel = juce::jlimit(0.0f, 1.0f, bass);
    updateFilters();
}

void BassEffects::setMid(float mid)
{
    midLevel = juce::jlimit(0.0f, 1.0f, mid);
    updateFilters();
}

void BassEffects::setTreble(float treble)
{
    trebleLevel = juce::jlimit(0.0f, 1.0f, treble);
    updateFilters();
}

void BassEffects::setCompression(float compression)
{
    compressionAmount = juce::jlimit(0.0f, 1.0f, compression);
}

void BassEffects::setOutputGain(float gain)
{
    outputGain = juce::jlimit(0.0f, 1.0f, gain);
}
