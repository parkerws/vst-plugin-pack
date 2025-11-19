#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

/**
 * MIDIBassGuitarAudioProcessorEditor
 *
 * GUI for MIDI bass guitar virtual instrument featuring:
 * - Articulation selection (Picked, Fingerstyle, Slap, Muted)
 * - Tone preset selection (DI, Amp Sim, Compressed, Bright, Vintage)
 * - EQ controls (Bass, Mid, Treble)
 * - Compression control
 * - Output gain control
 * - Bass-themed visual design
 */
class MIDIBassGuitarAudioProcessorEditor : public juce::AudioProcessorEditor,
                                           private juce::Timer,
                                           private juce::ComboBox::Listener,
                                           private juce::Slider::Listener
{
public:
    MIDIBassGuitarAudioProcessorEditor(MIDIBassGuitarAudioProcessor&);
    ~MIDIBassGuitarAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;
    void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override;
    void sliderValueChanged(juce::Slider* slider) override;

    MIDIBassGuitarAudioProcessor& audioProcessor;

    // Articulation selection
    juce::ComboBox articulationSelector;
    juce::Label articulationLabel;

    // Tone selection
    juce::ComboBox toneSelector;
    juce::Label toneLabel;

    // EQ controls
    juce::Slider bassSlider;
    juce::Label bassLabel;

    juce::Slider midSlider;
    juce::Label midLabel;

    juce::Slider trebleSlider;
    juce::Label trebleLabel;

    // Compression control
    juce::Slider compressionSlider;
    juce::Label compressionLabel;

    // Output gain
    juce::Slider outputGainSlider;
    juce::Label outputGainLabel;

    // Colors - Bass guitar themed (deep blue/black)
    const juce::Colour bassPanelColor = juce::Colour(0xFF1A1A2E);      // Dark blue-black
    const juce::Colour bassAccentColor = juce::Colour(0xFF16213E);     // Slightly lighter blue
    const juce::Colour bassHighlightColor = juce::Colour(0xFF0F3460);  // Blue highlight
    const juce::Colour bassTextColor = juce::Colour(0xFFE0E0E0);       // Light text
    const juce::Colour bassLogoColor = juce::Colour(0xFF00D9FF);       // Cyan accent

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MIDIBassGuitarAudioProcessorEditor)
};
