#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "CustomKnob.h"

/**
 * JazzChorusAmpAudioProcessorEditor
 *
 * Visual amp panel GUI featuring:
 * - Silver/chrome Jazz Chorus-style visual design
 * - Interactive knobs for all parameters
 * - Chorus on/off button
 * - Clean, professional layout
 * - Real-time parameter updates
 */
class JazzChorusAmpAudioProcessorEditor : public juce::AudioProcessorEditor,
                                          private juce::Timer
{
public:
    JazzChorusAmpAudioProcessorEditor(JazzChorusAmpAudioProcessor&);
    ~JazzChorusAmpAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;

    JazzChorusAmpAudioProcessor& audioProcessor;

    // Custom knobs for all parameters
    CustomKnob volumeKnob;
    CustomKnob bassKnob;
    CustomKnob middleKnob;
    CustomKnob trebleKnob;
    CustomKnob brightKnob;
    CustomKnob chorusRateKnob;
    CustomKnob chorusDepthKnob;
    CustomKnob chorusMixKnob;

    // Chorus on/off button
    juce::TextButton chorusButton;

    // Colors for Jazz Chorus aesthetic - silver/chrome
    const juce::Colour silverColor = juce::Colour(0xFFC0C0C0);
    const juce::Colour panelColor = juce::Colour(0xFFE8E8E8);       // Light silver panel
    const juce::Colour accentColor = juce::Colour(0xFFD0D0D0);
    const juce::Colour darkAccent = juce::Colour(0xFF606060);
    const juce::Colour textColor = juce::Colour(0xFF202020);
    const juce::Colour logoColor = juce::Colour(0xFF000000);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(JazzChorusAmpAudioProcessorEditor)
};
