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

    // Prepare cabinet convolution
    cabinetConvolution.prepare(spec);
    loadCabinetIR();

    // Initialize 2x oversampling for better tube modeling
    oversampler = std::make_unique<Oversample>(numChannels, 1,
                                                 Oversample::filterHalfBandPolyphaseIIR,
                                                 false); // Not using steep filter
    oversampler->initProcessing(samplesPerBlock);

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
    cabinetConvolution.reset();
    powerAmpEnvelope = 0.0f;

    if (oversampler)
        oversampler->reset();
}

void AmpSimDSP::processBlock(juce::AudioBuffer<float>& buffer)
{
    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    // Calculate actual gain values from normalized parameters
    const float preampGainValue = 1.0f + (preampGain * 19.0f); // 1-20 range (tube amps!)
    const float driveAmount = drive * 10.0f; // 0-10 range
    const float outputGain = masterVolume * masterVolume; // Squared for better taper

    // REAL AMP SIMULATION:
    // Upsample for better nonlinear processing (reduces aliasing from tube saturation)
    juce::dsp::AudioBlock<float> block(buffer);
    auto oversampledBlock = oversampler->processSamplesUp(block);

    // Process each sample through tube stages with oversampling
    for (size_t channel = 0; channel < oversampledBlock.getNumChannels(); ++channel)
    {
        float* channelData = oversampledBlock.getChannelPointer(channel);

        for (size_t sample = 0; sample < oversampledBlock.getNumSamples(); ++sample)
        {
            float inputSample = channelData[sample];

            // Stage 1: 12AX7 Tube Preamp (high gain triode)
            float preampOut = tubePreamp(inputSample, preampGainValue);

            // Stage 2: Drive/Saturation (second gain stage)
            float driveOut = applySaturation(preampOut, driveAmount);

            // Stage 3: EL84 Power Amp with sag simulation
            float powerAmpOut = tubePowerAmp(driveOut);

            // Stage 4: Output transformer saturation
            float transformerOut = outputTransformer(powerAmpOut);

            channelData[sample] = transformerOut;
        }
    }

    // Downsample back to original sample rate
    oversampler->processSamplesDown(block);

    // Post-tube processing
    juce::dsp::ProcessContextReplacing<float> context(block);

    // Tone Stack (EQ) - apply to entire block efficiently
    bassFilter.process(context);
    middleFilter.process(context);
    trebleFilter.process(context);
    presenceFilter.process(context);

    // DC blocker to remove any DC offset
    dcBlocker.process(context);

    // CABINET SIMULATION - The key difference from a pedal!
    if (cabinetLoaded)
    {
        cabinetConvolution.process(context);
    }

    // Output gain
    for (int channel = 0; channel < numChannels; ++channel)
    {
        float* channelData = buffer.getWritePointer(channel);
        for (int sample = 0; sample < numSamples; ++sample)
        {
            channelData[sample] *= outputGain;
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

void AmpSimDSP::loadCabinetIR()
{
    // Load a simulated 4x12 Orange cabinet impulse response
    // In a real implementation, this would load an actual IR file
    // For now, we'll create a simple IR that simulates cabinet characteristics

    const int irLength = 2048; // Typical IR length (about 46ms at 44.1kHz)
    juce::AudioBuffer<float> ir(2, irLength); // Stereo IR
    ir.clear();

    // Create a simple simulated cabinet response
    // Real cabinets have:
    // - Initial transient (speaker cone response)
    // - Reflections from cabinet walls
    // - Frequency-dependent decay

    for (int channel = 0; channel < 2; ++channel)
    {
        float* irData = ir.getWritePointer(channel);

        for (int i = 0; i < irLength; ++i)
        {
            float t = static_cast<float>(i) / static_cast<float>(irLength);

            // Initial impulse with some high-frequency content
            float impulse = (i < 10) ? (1.0f - t * 10.0f) : 0.0f;

            // Cabinet resonances (simplified)
            float resonance = std::sin(t * 100.0f * juce::MathConstants<float>::twoPi) * 0.3f;

            // Exponential decay
            float decay = std::exp(-t * 8.0f);

            // Random reflections (cabinet complexity)
            float reflection = (std::sin(t * 500.0f) + std::sin(t * 1200.0f)) * 0.1f;

            irData[i] = (impulse + resonance + reflection) * decay * 0.5f;
        }
    }

    // Load the IR into the convolution engine
    cabinetConvolution.loadImpulseResponse(std::move(ir),
                                           currentSampleRate,
                                           juce::dsp::Convolution::Stereo::yes,
                                           juce::dsp::Convolution::Trim::no,
                                           juce::dsp::Convolution::Normalise::yes);
    cabinetLoaded = true;
}

float AmpSimDSP::tubePreamp(float input, float gain)
{
    // 12AX7 tube preamp modeling
    // Characteristics: High gain, asymmetric clipping, soft knee

    // Apply gain
    float gained = input * gain;

    // 12AX7 has different characteristics for positive/negative cycles
    // This is due to the grid current and plate current asymmetry
    if (gained > 0.0f)
    {
        // Positive cycle: softer clipping due to grid current
        float threshold = 0.7f;
        if (gained > threshold)
        {
            float excess = gained - threshold;
            gained = threshold + tanh_approx(excess * 1.5f) / 1.5f;
        }
    }
    else
    {
        // Negative cycle: harder clipping
        float threshold = -0.8f;
        if (gained < threshold)
        {
            float excess = gained - threshold;
            gained = threshold + tanh_approx(excess * 2.0f) / 2.0f;
        }
    }

    return gained;
}

float AmpSimDSP::tubePowerAmp(float input)
{
    // EL84 power amp modeling with sag simulation
    // Characteristics: Moderate compression, symmetric clipping, power supply sag

    // Power supply sag simulation
    // When signal is loud, power supply voltage drops slightly
    float inputAbs = std::abs(input);

    // Update envelope follower (attack/release)
    if (inputAbs > powerAmpEnvelope)
        powerAmpEnvelope = powerAmpEnvelope * 0.7f + inputAbs * 0.3f; // Fast attack
    else
        powerAmpEnvelope = powerAmpEnvelope * 0.99f + inputAbs * 0.01f; // Slow release

    // Sag reduces available headroom when envelope is high
    float sagAmount = powerAmpEnvelope * 0.3f; // Up to 30% voltage sag
    float availableHeadroom = 1.0f - sagAmount;

    // Apply sag (compression effect)
    float compressed = input * availableHeadroom;

    // EL84 push-pull output stage (more symmetric than single-ended)
    float output = tanh_approx(compressed * 1.2f) / 1.2f;

    // Add subtle even harmonics (push-pull characteristic)
    float evenHarmonic = output * output * 0.05f;
    if (input < 0.0f)
        evenHarmonic = -evenHarmonic;

    return output + evenHarmonic;
}

float AmpSimDSP::outputTransformer(float input)
{
    // Output transformer saturation
    // Characteristics: Soft saturation, high-frequency rolloff, adds "weight"

    // Transformers have soft saturation at high levels
    float saturated = input;

    if (std::abs(input) > 0.5f)
    {
        // Soft saturation curve
        saturated = tanh_approx(input * 0.8f) / 0.8f;
    }

    // Add transformer "warmth" (slight asymmetry)
    float warmth = saturated * std::abs(saturated) * 0.02f;

    return saturated + warmth;
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
