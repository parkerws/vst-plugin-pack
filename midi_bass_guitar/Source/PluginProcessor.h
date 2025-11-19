#pragma once
#include <JuceHeader.h>
#include "BassSamplerEngine.h"

/**
 * MIDIBassGuitarAudioProcessor
 *
 * Virtual instrument processor for MIDI bass guitar
 *
 * Handles:
 * - MIDI input (note on/off, velocity)
 * - Bass sample playback/synthesis
 * - Articulation selection (picked, fingerstyle, slap, muted)
 * - Tone processing (DI, amp sim, compressed, etc.)
 * - Effects (EQ, compression)
 * - Parameter automation
 */
class MIDIBassGuitarAudioProcessor : public juce::AudioProcessor
{
public:
    MIDIBassGuitarAudioProcessor();
    ~MIDIBassGuitarAudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    // Public parameters for editor
    juce::AudioParameterChoice* articulationParam;
    juce::AudioParameterChoice* toneParam;
    juce::AudioParameterFloat* bassParam;
    juce::AudioParameterFloat* midParam;
    juce::AudioParameterFloat* trebleParam;
    juce::AudioParameterFloat* compressionParam;
    juce::AudioParameterFloat* outputGainParam;

    // Access to sampler for GUI
    BassSamplerEngine& getSampler() { return sampler; }

private:
    // Bass sampler engine
    BassSamplerEngine sampler;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MIDIBassGuitarAudioProcessor)
};
