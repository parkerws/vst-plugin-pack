#pragma once
#include <JuceHeader.h>

/**
 * Bass Articulations - Different playing techniques
 */
enum class BassArticulation
{
    Picked,      // Bright, punchy attack (pick/plectrum)
    Fingerstyle, // Warm, soft attack (fingers)
    Slap,        // Percussive, aggressive (slap/pop technique)
    Muted        // Dead notes, palm muted
};

/**
 * BassSound - Describes the sound/sample that can be played
 *
 * In a full implementation, this would load actual audio samples.
 * For now, it defines the parameters for synthesis.
 */
class BassSound : public juce::SynthesiserSound
{
public:
    BassSound() {}

    bool appliesToNote(int midiNoteNumber) override
    {
        // Bass guitar range: E1 (MIDI 28) to G4 (MIDI 67)
        return midiNoteNumber >= 28 && midiNoteNumber <= 67;
    }

    bool appliesToChannel(int midiChannel) override
    {
        juce::ignoreUnused(midiChannel);
        return true; // Respond to all MIDI channels
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BassSound)
};

/**
 * BassVoice - Plays individual bass notes
 *
 * Handles:
 * - MIDI note on/off
 * - Velocity sensitivity
 * - Different articulations
 * - ADSR envelope
 * - Realistic bass tone generation
 *
 * NOTE: Currently synthesizes bass tones. Can be extended to play
 * actual audio samples by loading them in startNote() and playing
 * them back instead of synthesis.
 */
class BassVoice : public juce::SynthesiserVoice
{
public:
    BassVoice();

    bool canPlaySound(juce::SynthesiserSound* sound) override;
    void startNote(int midiNoteNumber, float velocity,
                   juce::SynthesiserSound* sound, int currentPitchWheelPosition) override;
    void stopNote(float velocity, bool allowTailOff) override;
    void pitchWheelMoved(int newPitchWheelValue) override;
    void controllerMoved(int controllerNumber, int newControllerValue) override;
    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;

    // Set current articulation
    void setArticulation(BassArticulation articulation);

    // Prepare for playback
    void prepare(double sampleRate, int samplesPerBlock);

private:
    // Current articulation
    BassArticulation currentArticulation = BassArticulation::Fingerstyle;

    // Sound generation
    double currentAngle = 0.0;
    double angleDelta = 0.0;
    double currentSampleRate = 44100.0;
    double currentFrequency = 0.0;
    float currentVelocity = 0.0f;

    // ADSR envelope
    juce::ADSR adsr;
    juce::ADSR::Parameters adsrParams;

    // Articulation-specific parameters
    void updateADSRForArticulation();

    // Tone generation (synthesis - can be replaced with sample playback)
    float generateBassWaveform();
    float generatePickedTone();
    float generateFingerstyleTone();
    float generateSlapTone();
    float generateMutedTone();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BassVoice)
};
