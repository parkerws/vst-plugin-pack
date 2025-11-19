#include "JazzChorusDSP.h"

JazzChorusDSP::JazzChorusDSP()
{
}

JazzChorusDSP::~JazzChorusDSP()
{
}

void JazzChorusDSP::prepare(double sampleRate, int samplesPerBlock, int numChannels)
{
    currentSampleRate = sampleRate;

    // Prepare all filters with proper spec
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = numChannels;

    bassFilter.prepare(spec);
    middleFilter.prepare(spec);
    trebleFilter.prepare(spec);
    brightFilter.prepare(spec);

    // Prepare chorus effect
    chorus.prepare(sampleRate, samplesPerBlock, numChannels);

    // Initialize filters with current parameter values
    updateFilters();

    reset();
}

void JazzChorusDSP::reset()
{
    bassFilter.reset();
    middleFilter.reset();
    trebleFilter.reset();
    brightFilter.reset();
    chorus.reset();
}

void JazzChorusDSP::processBlock(juce::AudioBuffer<float>& buffer)
{
    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    // Convert to JUCE DSP block for filter processing
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);

    // Calculate gain value
    const float outputGain = volume * volume; // Squared for better taper

    // Stage 1: Clean preamp - minimal processing (Jazz Chorus is known for clean headroom)
    for (int channel = 0; channel < numChannels; ++channel)
    {
        float* channelData = buffer.getWritePointer(channel);

        for (int sample = 0; sample < numSamples; ++sample)
        {
            float inputSample = channelData[sample];

            // Very subtle solid-state warmth (Jazz Chorus is clean but not sterile)
            float preampOut = applyCleanSaturation(inputSample * 1.2f);

            channelData[sample] = preampOut;
        }
    }

    // Stage 2: EQ - Bright Jazz Chorus character
    bassFilter.process(context);
    middleFilter.process(context);
    trebleFilter.process(context);
    brightFilter.process(context);

    // Stage 3: Chorus effect (the star of the show!)
    chorus.processBlock(buffer);

    // Stage 4: Output volume
    for (int channel = 0; channel < numChannels; ++channel)
    {
        float* channelData = buffer.getWritePointer(channel);
        for (int sample = 0; sample < numSamples; ++sample)
        {
            channelData[sample] *= outputGain;
        }
    }
}

void JazzChorusDSP::updateFilters()
{
    const double sampleRate = currentSampleRate;

    // Bass - Low shelf at 100Hz
    // Range: -12dB to +12dB
    float bassGainDb = (bass - 0.5f) * 24.0f;
    *bassFilter.state = *FilterCoefs::makeLowShelf(sampleRate, 100.0, 0.7,
                                                     juce::Decibels::decibelsToGain(bassGainDb));

    // Middle - Peaking filter at 1kHz (clear, clean midrange)
    // Range: -12dB to +12dB
    float middleGainDb = (middle - 0.5f) * 24.0f;
    *middleFilter.state = *FilterCoefs::makePeakFilter(sampleRate, 1000.0, 1.0,
                                                         juce::Decibels::decibelsToGain(middleGainDb));

    // Treble - High shelf at 4kHz (sparkle and clarity)
    // Range: -12dB to +12dB
    float trebleGainDb = (treble - 0.5f) * 24.0f;
    *trebleFilter.state = *FilterCoefs::makeHighShelf(sampleRate, 4000.0, 0.7,
                                                        juce::Decibels::decibelsToGain(trebleGainDb));

    // Bright - High shelf at 8kHz (extra sparkle when engaged)
    // Range: 0dB to +8dB
    float brightGainDb = bright * 8.0f;
    *brightFilter.state = *FilterCoefs::makeHighShelf(sampleRate, 8000.0, 0.5,
                                                        juce::Decibels::decibelsToGain(brightGainDb));
}

float JazzChorusDSP::applyCleanSaturation(float input)
{
    // Very subtle soft clipping for solid-state warmth
    // Jazz Chorus stays clean, so this is minimal
    if (input > 1.5f)
        return 1.5f;
    if (input < -1.5f)
        return -1.5f;

    // Subtle soft knee
    if (input > 0.8f)
        return 0.8f + (input - 0.8f) * 0.5f;
    if (input < -0.8f)
        return -0.8f + (input + 0.8f) * 0.5f;

    return input;
}

// Parameter setters
void JazzChorusDSP::setVolume(float volumeValue)
{
    volume = juce::jlimit(0.0f, 1.0f, volumeValue);
}

void JazzChorusDSP::setBass(float bassValue)
{
    bass = juce::jlimit(0.0f, 1.0f, bassValue);
    updateFilters();
}

void JazzChorusDSP::setMiddle(float middleValue)
{
    middle = juce::jlimit(0.0f, 1.0f, middleValue);
    updateFilters();
}

void JazzChorusDSP::setTreble(float trebleValue)
{
    treble = juce::jlimit(0.0f, 1.0f, trebleValue);
    updateFilters();
}

void JazzChorusDSP::setBright(float brightValue)
{
    bright = juce::jlimit(0.0f, 1.0f, brightValue);
    updateFilters();
}

void JazzChorusDSP::setChorusRate(float rate)
{
    chorus.setRate(rate);
}

void JazzChorusDSP::setChorusDepth(float depth)
{
    chorus.setDepth(depth);
}

void JazzChorusDSP::setChorusMix(float mix)
{
    chorus.setMix(mix);
}

void JazzChorusDSP::setChorusEnabled(bool enabled)
{
    chorus.setEnabled(enabled);
}
