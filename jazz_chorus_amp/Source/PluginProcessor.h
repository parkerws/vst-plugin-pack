#pragma once
#include <JuceHeader.h>
#include "JazzChorusDSP.h"

/**
 * JazzChorusAmpAudioProcessor
 *
 * Main plugin processor class - handles:
 * - Audio processing via JazzChorusDSP engine
 * - Parameter management and automation
 * - State save/load for DAW projects
 * - Editor creation
 */
class JazzChorusAmpAudioProcessor : public juce::AudioProcessor
{
public:
    JazzChorusAmpAudioProcessor();
    ~JazzChorusAmpAudioProcessor() override;

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

    // Public access to parameters for editor
    juce::AudioParameterFloat* volumeParam;
    juce::AudioParameterFloat* bassParam;
    juce::AudioParameterFloat* middleParam;
    juce::AudioParameterFloat* trebleParam;
    juce::AudioParameterFloat* brightParam;
    juce::AudioParameterFloat* chorusRateParam;
    juce::AudioParameterFloat* chorusDepthParam;
    juce::AudioParameterFloat* chorusMixParam;
    juce::AudioParameterBool* chorusEnabledParam;

private:
    // DSP engine
    JazzChorusDSP jazzChorusDSP;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(JazzChorusAmpAudioProcessor)
};
