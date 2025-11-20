#include "BassVoice.h"

BassVoice::BassVoice()
{
    // Default ADSR for fingerstyle
    updateADSRForArticulation();
}

void BassVoice::prepare(double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused(samplesPerBlock);
    currentSampleRate = sampleRate;
    adsr.setSampleRate(sampleRate);
}

bool BassVoice::canPlaySound(juce::SynthesiserSound* sound)
{
    return dynamic_cast<BassSound*>(sound) != nullptr;
}

void BassVoice::startNote(int midiNoteNumber, float velocity,
                          juce::SynthesiserSound* sound, int currentPitchWheelPosition)
{
    juce::ignoreUnused(sound, currentPitchWheelPosition);

    // Store velocity for dynamics
    currentVelocity = velocity;

    // Calculate frequency from MIDI note number
    currentFrequency = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    angleDelta = currentFrequency * 2.0 * juce::MathConstants<double>::pi / currentSampleRate;

    // Reset phase
    currentAngle = 0.0;

    // Update ADSR based on articulation
    updateADSRForArticulation();

    // Start envelope
    adsr.noteOn();
}

void BassVoice::stopNote(float velocity, bool allowTailOff)
{
    juce::ignoreUnused(velocity);

    if (allowTailOff)
    {
        adsr.noteOff();
    }
    else
    {
        clearCurrentNote();
        adsr.reset();
    }
}

void BassVoice::pitchWheelMoved(int newPitchWheelValue)
{
    juce::ignoreUnused(newPitchWheelValue);
    // Could implement pitch bend here
}

void BassVoice::controllerMoved(int controllerNumber, int newControllerValue)
{
    juce::ignoreUnused(controllerNumber, newControllerValue);
    // Could implement CC modulation here
}

void BassVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    if (!isVoiceActive())
        return;

    while (--numSamples >= 0)
    {
        // Generate bass waveform based on articulation
        float currentSample = generateBassWaveform();

        // Apply ADSR envelope
        currentSample *= adsr.getNextSample();

        // Apply velocity
        currentSample *= currentVelocity;

        // Write to all output channels
        for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
        {
            outputBuffer.addSample(channel, startSample, currentSample);
        }

        // Advance phase
        currentAngle += angleDelta;
        if (currentAngle > 2.0 * juce::MathConstants<double>::pi)
            currentAngle -= 2.0 * juce::MathConstants<double>::pi;

        ++startSample;

        // Stop voice when envelope is complete
        if (!adsr.isActive())
        {
            clearCurrentNote();
            break;
        }
    }
}

void BassVoice::setArticulation(BassArticulation articulation)
{
    currentArticulation = articulation;
    updateADSRForArticulation();
}

void BassVoice::updateADSRForArticulation()
{
    // Different ADSR envelopes for each articulation
    switch (currentArticulation)
    {
        case BassArticulation::Picked:
            // Fast attack, short decay, medium sustain, short release
            adsrParams.attack = 0.001f;   // 1ms - sharp attack
            adsrParams.decay = 0.1f;
            adsrParams.sustain = 0.6f;
            adsrParams.release = 0.1f;
            break;

        case BassArticulation::Fingerstyle:
            // Slower attack, longer decay, higher sustain, longer release
            adsrParams.attack = 0.005f;   // 5ms - softer attack
            adsrParams.decay = 0.15f;
            adsrParams.sustain = 0.8f;
            adsrParams.release = 0.2f;
            break;

        case BassArticulation::Slap:
            // Very fast attack, quick decay, low sustain, very short release
            adsrParams.attack = 0.0005f;  // 0.5ms - percussive
            adsrParams.decay = 0.05f;
            adsrParams.sustain = 0.3f;
            adsrParams.release = 0.05f;
            break;

        case BassArticulation::Muted:
            // Fast attack, very quick decay, very low sustain, very short release
            adsrParams.attack = 0.001f;
            adsrParams.decay = 0.02f;
            adsrParams.sustain = 0.1f;
            adsrParams.release = 0.02f;
            break;
    }

    adsr.setParameters(adsrParams);
}

float BassVoice::generateBassWaveform()
{
    // Generate appropriate waveform based on articulation
    switch (currentArticulation)
    {
        case BassArticulation::Picked:
            return generatePickedTone();
        case BassArticulation::Fingerstyle:
            return generateFingerstyleTone();
        case BassArticulation::Slap:
            return generateSlapTone();
        case BassArticulation::Muted:
            return generateMutedTone();
        default:
            return 0.0f;
    }
}

float BassVoice::generatePickedTone()
{
    // Bright, punchy tone - more harmonics
    // Mix of sine and triangle wave for brightness
    float fundamental = std::sin(currentAngle);
    float harmonic2 = std::sin(currentAngle * 2.0) * 0.3f;
    float harmonic3 = std::sin(currentAngle * 3.0) * 0.15f;

    return (fundamental + harmonic2 + harmonic3) * 0.6f;
}

float BassVoice::generateFingerstyleTone()
{
    // Warm, smooth tone - more fundamental, fewer harmonics
    // Mostly sine wave with slight 2nd harmonic
    float fundamental = std::sin(currentAngle);
    float harmonic2 = std::sin(currentAngle * 2.0) * 0.15f;

    return (fundamental + harmonic2) * 0.7f;
}

float BassVoice::generateSlapTone()
{
    // Percussive, bright tone - lots of high harmonics
    // Mix of multiple harmonics for aggressive character
    float fundamental = std::sin(currentAngle);
    float harmonic2 = std::sin(currentAngle * 2.0) * 0.4f;
    float harmonic3 = std::sin(currentAngle * 3.0) * 0.25f;
    float harmonic4 = std::sin(currentAngle * 4.0) * 0.15f;
    float harmonic5 = std::sin(currentAngle * 5.0) * 0.1f;

    return (fundamental + harmonic2 + harmonic3 + harmonic4 + harmonic5) * 0.5f;
}

float BassVoice::generateMutedTone()
{
    // Dull, short tone - very few harmonics, low-passed
    // Mostly fundamental with quick decay
    float fundamental = std::sin(currentAngle);

    return fundamental * 0.4f;
}
