#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "CustomKnob.h"

/**
 * OrangeAmpSimulatorAudioProcessorEditor
 *
 * Visual amp panel GUI featuring:
 * - Orange amp-style visual design
 * - Interactive knobs for all parameters
 * - Amp-like layout and aesthetics
 * - Real-time parameter updates
 */
class OrangeAmpSimulatorAudioProcessorEditor : public juce::AudioProcessorEditor,
                                                private juce::Timer
{
public:
    OrangeAmpSimulatorAudioProcessorEditor(OrangeAmpSimulatorAudioProcessor&);
    ~OrangeAmpSimulatorAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;

    OrangeAmpSimulatorAudioProcessor& audioProcessor;

    // Custom knobs for all parameters
    CustomKnob preampGainKnob;
    CustomKnob bassKnob;
    CustomKnob middleKnob;
    CustomKnob trebleKnob;
    CustomKnob driveKnob;
    CustomKnob presenceKnob;
    CustomKnob masterVolumeKnob;

    // Colors for amp aesthetic
    const juce::Colour orangeColor = juce::Colour(0xFFFF8C00);
    const juce::Colour panelColor = juce::Colour(0xFF1A1A1A);
    const juce::Colour accentColor = juce::Colour(0xFF2A2A2A);
    const juce::Colour textColor = juce::Colour(0xFFE0E0E0);
    const juce::Colour logoColor = juce::Colour(0xFFFFFFFF);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OrangeAmpSimulatorAudioProcessorEditor)
};
