#include "AmpSimDSP.h"

AmpSimDSP::AmpSimDSP()
{
}

AmpSimDSP::~AmpSimDSP()
{
}

void AmpSimDSP::prepare(double sampleRate, int samplesPerBlock, int numChannels)
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
    presenceFilter.prepare(spec);
    dcBlocker.prepare(spec);

    // Initialize DC blocker (high-pass at 20Hz)
    *dcBlocker.state = *FilterCoefs::makeHighPass(sampleRate, 20.0);

    // Initialize filters with current parameter values
    updateFilters();

    reset();
}

void AmpSimDSP::reset()
{
    bassFilter.reset();
    middleFilter.reset();
    trebleFilter.reset();
    presenceFilter.reset();
    dcBlocker.reset();
}

void AmpSimDSP::processBlock(juce::AudioBuffer<float>& buffer)
{
    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    // Convert to JUCE DSP block for filter processing
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);

    // Calculate actual gain values from normalized parameters
    const float preampGainLinear = 1.0f + (preampGain * 9.0f); // 1-10 range
    const float driveAmount = drive * 10.0f; // 0-10 range
    const float outputGain = masterVolume * masterVolume; // Squared for better taper

    // Process each sample
    for (int channel = 0; channel < numChannels; ++channel)
    {
        float* channelData = buffer.getWritePointer(channel);

        for (int sample = 0; sample < numSamples; ++sample)
        {
            float inputSample = channelData[sample];

            // Stage 1: Preamp gain
            float preampOut = inputSample * preampGainLinear;
            preampOut = softClip(preampOut); // Soft clip at preamp stage

            // Stage 2: Drive/Saturation
            float driveOut = applySaturation(preampOut, driveAmount);

            // Output
            channelData[sample] = driveOut * outputGain;
        }
    }

    // Stage 3: Tone Stack (EQ) - apply to entire block efficiently
    bassFilter.process(context);
    middleFilter.process(context);
    trebleFilter.process(context);
    presenceFilter.process(context);

    // Stage 4: DC blocker to remove any DC offset
    dcBlocker.process(context);

    // Final soft limiting to prevent clipping
    for (int channel = 0; channel < numChannels; ++channel)
    {
        float* channelData = buffer.getWritePointer(channel);
        for (int sample = 0; sample < numSamples; ++sample)
        {
            channelData[sample] = softClip(channelData[sample]);
        }
    }
}

void AmpSimDSP::updateFilters()
{
    const double sampleRate = currentSampleRate;

    // Bass - Low shelf at 120Hz
    // Range: -12dB to +12dB
    float bassGainDb = (bass - 0.5f) * 24.0f;
    *bassFilter.state = *FilterCoefs::makeLowShelf(sampleRate, 120.0, 0.7,
                                                     juce::Decibels::decibelsToGain(bassGainDb));

    // Middle - Peaking filter at 800Hz (critical guitar frequency)
    // Range: -12dB to +12dB
    float middleGainDb = (middle - 0.5f) * 24.0f;
    *middleFilter.state = *FilterCoefs::makePeakFilter(sampleRate, 800.0, 1.5,
                                                         juce::Decibels::decibelsToGain(middleGainDb));

    // Treble - High shelf at 3kHz
    // Range: -12dB to +12dB
    float trebleGainDb = (treble - 0.5f) * 24.0f;
    *trebleFilter.state = *FilterCoefs::makeHighShelf(sampleRate, 3000.0, 0.7,
                                                        juce::Decibels::decibelsToGain(trebleGainDb));

    // Presence - High shelf at 6kHz (amp-like presence control)
    // Range: -6dB to +6dB
    float presenceGainDb = (presence - 0.5f) * 12.0f;
    *presenceFilter.state = *FilterCoefs::makeHighShelf(sampleRate, 6000.0, 0.5,
                                                          juce::Decibels::decibelsToGain(presenceGainDb));
}

float AmpSimDSP::applySaturation(float input, float driveAmount)
{
    // Apply drive gain
    float driven = input * (1.0f + driveAmount * 2.0f);

    // Asymmetric waveshaping (tube-like)
    // Positive and negative cycles clip differently (like real tubes)
    if (driven > 0.0f)
    {
        // Positive cycle - softer clipping
        driven = tanh_approx(driven * 0.8f) * 1.25f;
    }
    else
    {
        // Negative cycle - harder clipping (more harmonics)
        driven = tanh_approx(driven * 1.2f) * 0.83f;
    }

    // Power amp compression (subtle)
    driven = tanh_approx(driven * 0.9f) * 1.1f;

    return driven;
}

float AmpSimDSP::softClip(float input)
{
    // Soft clipping using tanh
    // Keeps signal in safe range while adding gentle harmonic content
    return tanh_approx(input);
}

float AmpSimDSP::tanh_approx(float x)
{
    // Fast tanh approximation for real-time audio
    // Accurate enough for audio, much faster than std::tanh
    if (x > 3.0f) return 1.0f;
    if (x < -3.0f) return -1.0f;

    float x2 = x * x;
    return x * (27.0f + x2) / (27.0f + 9.0f * x2);
}

// Parameter setters
void AmpSimDSP::setPreampGain(float gain)
{
    preampGain = juce::jlimit(0.0f, 1.0f, gain);
}

void AmpSimDSP::setBass(float bassValue)
{
    bass = juce::jlimit(0.0f, 1.0f, bassValue);
    updateFilters();
}

void AmpSimDSP::setMiddle(float middleValue)
{
    middle = juce::jlimit(0.0f, 1.0f, middleValue);
    updateFilters();
}

void AmpSimDSP::setTreble(float trebleValue)
{
    treble = juce::jlimit(0.0f, 1.0f, trebleValue);
    updateFilters();
}

void AmpSimDSP::setDrive(float driveValue)
{
    drive = juce::jlimit(0.0f, 1.0f, driveValue);
}

void AmpSimDSP::setPresence(float presenceValue)
{
    presence = juce::jlimit(0.0f, 1.0f, presenceValue);
    updateFilters();
}

void AmpSimDSP::setMasterVolume(float volume)
{
    masterVolume = juce::jlimit(0.0f, 1.0f, volume);
}
