#include "ChorusEffect.h"

ChorusEffect::ChorusEffect()
{
}

ChorusEffect::~ChorusEffect()
{
}

void ChorusEffect::prepare(double sampleRate, int samplesPerBlock, int numChannels)
{
    juce::ignoreUnused(samplesPerBlock);

    currentSampleRate = sampleRate;

    // Allocate delay buffer (need enough for longest delay time)
    delayBuffer.setSize(numChannels, maxDelayBufferSize);
    delayBuffer.clear();
    delayBufferWritePos = 0;

    // Setup smoothing (20ms ramp time to prevent clicks)
    smoothedRate.reset(sampleRate, 0.02);
    smoothedDepth.reset(sampleRate, 0.02);
    smoothedMix.reset(sampleRate, 0.02);

    updateLFOIncrement();
    reset();
}

void ChorusEffect::reset()
{
    delayBuffer.clear();
    delayBufferWritePos = 0;
    lfoPhase = 0.0f;
}

void ChorusEffect::processBlock(juce::AudioBuffer<float>& buffer)
{
    if (!isEnabled)
        return;

    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    // Update smoothed parameter values
    smoothedRate.setTargetValue(rate);
    smoothedDepth.setTargetValue(depth);
    smoothedMix.setTargetValue(mix);

    for (int sample = 0; sample < numSamples; ++sample)
    {
        // Get current smoothed values
        float currentRate = smoothedRate.getNextValue();
        float currentDepth = smoothedDepth.getNextValue();
        float currentMix = smoothedMix.getNextValue();

        // Update LFO
        updateLFOIncrement();
        lfoPhase += lfoPhaseIncrement;
        if (lfoPhase >= 1.0f)
            lfoPhase -= 1.0f;

        // Process each channel with slightly different LFO phase for stereo width
        for (int channel = 0; channel < numChannels; ++channel)
        {
            float* channelData = buffer.getWritePointer(channel);
            float* delayData = delayBuffer.getWritePointer(channel);

            // Dry signal
            float inputSample = channelData[sample];

            // Write to delay buffer
            delayData[delayBufferWritePos] = inputSample;

            // Calculate modulated delay time
            // Base delay: ~5ms, modulation depth: ±3ms
            float lfoPhaseOffset = (channel == 1) ? 0.5f : 0.0f; // 180° phase difference for stereo
            float lfoSample = getLFOSample(lfoPhase + lfoPhaseOffset);

            float baseDelayMs = 5.0f;
            float modulationMs = 3.0f * currentDepth;
            float delayMs = baseDelayMs + (lfoSample * modulationMs);
            float delaySamples = (delayMs / 1000.0f) * currentSampleRate;

            // Get delayed sample with interpolation
            float delayedSample = getInterpolatedSample(channel, delaySamples);

            // Mix dry and wet signals
            float outputSample = inputSample * (1.0f - currentMix) + delayedSample * currentMix;
            channelData[sample] = outputSample;
        }

        // Advance write position
        delayBufferWritePos++;
        if (delayBufferWritePos >= maxDelayBufferSize)
            delayBufferWritePos = 0;
    }
}

void ChorusEffect::updateLFOIncrement()
{
    // Convert rate (0-1) to frequency (0.1Hz - 5Hz)
    float lfoFreq = 0.1f + (rate * 4.9f);
    lfoPhaseIncrement = lfoFreq / static_cast<float>(currentSampleRate);
}

float ChorusEffect::getLFOSample(float phase)
{
    // Sine wave LFO (smooth modulation)
    while (phase >= 1.0f)
        phase -= 1.0f;
    while (phase < 0.0f)
        phase += 1.0f;

    return std::sin(phase * 2.0f * juce::MathConstants<float>::pi);
}

float ChorusEffect::getInterpolatedSample(int channel, float delayInSamples)
{
    // Calculate read position (write position minus delay)
    float readPos = delayBufferWritePos - delayInSamples;

    // Wrap around if necessary
    while (readPos < 0.0f)
        readPos += maxDelayBufferSize;
    while (readPos >= maxDelayBufferSize)
        readPos -= maxDelayBufferSize;

    // Linear interpolation between two samples
    int readPosInt = static_cast<int>(readPos);
    float frac = readPos - readPosInt;

    int nextReadPos = (readPosInt + 1) % maxDelayBufferSize;

    float* delayData = delayBuffer.getReadPointer(channel);
    float sample1 = delayData[readPosInt];
    float sample2 = delayData[nextReadPos];

    return sample1 + frac * (sample2 - sample1);
}

// Parameter setters
void ChorusEffect::setRate(float rateValue)
{
    rate = juce::jlimit(0.0f, 1.0f, rateValue);
}

void ChorusEffect::setDepth(float depthValue)
{
    depth = juce::jlimit(0.0f, 1.0f, depthValue);
}

void ChorusEffect::setMix(float mixValue)
{
    mix = juce::jlimit(0.0f, 1.0f, mixValue);
}

void ChorusEffect::setEnabled(bool enabled)
{
    isEnabled = enabled;
}
