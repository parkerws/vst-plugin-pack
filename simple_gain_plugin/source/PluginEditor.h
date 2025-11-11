#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"

class SimpleGainPluginAudioProcessorEditor : public juce::AudioProcessorEditor,
                                             public juce::Slider::Listener
{
public:
    SimpleGainPluginAudioProcessorEditor(SimpleGainPluginAudioProcessor &);
    ~SimpleGainPluginAudioProcessorEditor() override;

    void paint(juce::Graphics &) override;
    void resized() override;
    void sliderValueChanged(juce::Slider *slider) override;

private:
    SimpleGainPluginAudioProcessor &audioProcessor;
    juce::Slider gainSlider;
    juce::Label gainLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SimpleGainPluginAudioProcessorEditor)
};