#include "PluginProcessor.h"
#include "PluginEditor.h"

MIDIBassGuitarAudioProcessorEditor::MIDIBassGuitarAudioProcessorEditor(
    MIDIBassGuitarAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    // Set window size - bass instrument panel
    setSize(650, 450);

    // Articulation selector
    articulationLabel.setText("ARTICULATION", juce::dontSendNotification);
    articulationLabel.setColour(juce::Label::textColourId, bassTextColor);
    articulationLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(articulationLabel);

    articulationSelector.addItem("Picked", 1);
    articulationSelector.addItem("Fingerstyle", 2);
    articulationSelector.addItem("Slap", 3);
    articulationSelector.addItem("Muted", 4);
    articulationSelector.setSelectedId(audioProcessor.articulationParam->getIndex() + 1);
    articulationSelector.addListener(this);
    addAndMakeVisible(articulationSelector);

    // Tone selector
    toneLabel.setText("TONE", juce::dontSendNotification);
    toneLabel.setColour(juce::Label::textColourId, bassTextColor);
    toneLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(toneLabel);

    toneSelector.addItem("DI", 1);
    toneSelector.addItem("Amp Sim", 2);
    toneSelector.addItem("Compressed", 3);
    toneSelector.addItem("Bright", 4);
    toneSelector.addItem("Vintage", 5);
    toneSelector.setSelectedId(audioProcessor.toneParam->getIndex() + 1);
    toneSelector.addListener(this);
    addAndMakeVisible(toneSelector);

    // Bass EQ slider
    bassLabel.setText("BASS", juce::dontSendNotification);
    bassLabel.setColour(juce::Label::textColourId, bassTextColor);
    bassLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(bassLabel);

    bassSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    bassSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    bassSlider.setRange(0.0, 1.0, 0.01);
    bassSlider.setValue(*audioProcessor.bassParam);
    bassSlider.addListener(this);
    addAndMakeVisible(bassSlider);

    // Mid EQ slider
    midLabel.setText("MID", juce::dontSendNotification);
    midLabel.setColour(juce::Label::textColourId, bassTextColor);
    midLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(midLabel);

    midSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    midSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    midSlider.setRange(0.0, 1.0, 0.01);
    midSlider.setValue(*audioProcessor.midParam);
    midSlider.addListener(this);
    addAndMakeVisible(midSlider);

    // Treble EQ slider
    trebleLabel.setText("TREBLE", juce::dontSendNotification);
    trebleLabel.setColour(juce::Label::textColourId, bassTextColor);
    trebleLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(trebleLabel);

    trebleSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    trebleSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    trebleSlider.setRange(0.0, 1.0, 0.01);
    trebleSlider.setValue(*audioProcessor.trebleParam);
    trebleSlider.addListener(this);
    addAndMakeVisible(trebleSlider);

    // Compression slider
    compressionLabel.setText("COMPRESSION", juce::dontSendNotification);
    compressionLabel.setColour(juce::Label::textColourId, bassTextColor);
    compressionLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(compressionLabel);

    compressionSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    compressionSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    compressionSlider.setRange(0.0, 1.0, 0.01);
    compressionSlider.setValue(*audioProcessor.compressionParam);
    compressionSlider.addListener(this);
    addAndMakeVisible(compressionSlider);

    // Output gain slider
    outputGainLabel.setText("OUTPUT", juce::dontSendNotification);
    outputGainLabel.setColour(juce::Label::textColourId, bassTextColor);
    outputGainLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(outputGainLabel);

    outputGainSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    outputGainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    outputGainSlider.setRange(0.0, 1.0, 0.01);
    outputGainSlider.setValue(*audioProcessor.outputGainParam);
    outputGainSlider.addListener(this);
    addAndMakeVisible(outputGainSlider);

    // Start timer for parameter updates
    startTimerHz(30);
}

MIDIBassGuitarAudioProcessorEditor::~MIDIBassGuitarAudioProcessorEditor()
{
}

void MIDIBassGuitarAudioProcessorEditor::paint(juce::Graphics& g)
{
    // Background - deep blue-black (bass themed)
    g.fillAll(bassPanelColor);

    // Draw gradient for depth
    {
        juce::ColourGradient gradient(bassAccentColor, getWidth() / 2.0f, 0.0f,
                                      bassPanelColor, getWidth() / 2.0f, getHeight(), false);
        g.setGradientFill(gradient);
        g.fillRect(getLocalBounds());
    }

    // Draw top header
    {
        auto header = getLocalBounds().removeFromTop(80);

        // Cyan accent line
        g.setColour(bassLogoColor);
        g.fillRect(header.removeFromTop(4));

        // Title
        g.setColour(bassLogoColor);
        g.setFont(juce::Font("Arial", 40.0f, juce::Font::bold));
        g.drawText("MIDI BASS GUITAR", header, juce::Justification::centred);
    }

    // Draw section labels
    {
        g.setColour(bassTextColor.withAlpha(0.6f));
        g.setFont(13.0f);

        // Articulation & Tone section
        g.drawText("PLAYING TECHNIQUE", 30, 100, 280, 20, juce::Justification::centred);

        // EQ section
        g.drawText("EQUALIZER", 30, 240, 590, 20, juce::Justification::centred);
    }

    // Draw decorative elements
    {
        // Bass strings visualization
        g.setColour(bassHighlightColor.withAlpha(0.3f));
        for (int i = 0; i < 4; ++i)
        {
            int y = 90 + (i * 8);
            g.fillRect(340, y, 280, 2);
        }

        // Corner dots
        g.setColour(bassLogoColor.withAlpha(0.3f));
        for (auto x : {20, getWidth() - 20})
        {
            for (auto y : {20, getHeight() - 20})
            {
                g.fillEllipse(x - 3, y - 3, 6, 6);
            }
        }
    }

    // Draw virtual instrument label
    {
        g.setColour(bassTextColor.withAlpha(0.7f));
        g.setFont(15.0f);
        auto footer = getLocalBounds().removeFromBottom(30);
        g.drawText("VIRTUAL INSTRUMENT - MIDI INPUT REQUIRED", footer, juce::Justification::centred);
    }
}

void MIDIBassGuitarAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    bounds.removeFromTop(120);  // Skip header
    bounds.removeFromBottom(30); // Skip footer

    // Top section - Articulation and Tone selectors
    auto topSection = bounds.removeFromTop(100);

    auto leftTop = topSection.removeFromLeft(320);
    articulationLabel.setBounds(leftTop.removeFromTop(25));
    articulationSelector.setBounds(leftTop.reduced(30, 10));

    auto rightTop = topSection;
    toneLabel.setBounds(rightTop.removeFromTop(25));
    toneSelector.setBounds(rightTop.reduced(30, 10));

    bounds.removeFromTop(40); // Spacing

    // EQ section - knobs in a row
    auto eqSection = bounds.removeFromTop(120);

    const int knobWidth = 100;
    const int spacing = 20;
    const int totalWidth = (knobWidth * 5) + (spacing * 4);
    const int startX = (getWidth() - totalWidth) / 2;

    // Bass
    auto bassArea = eqSection.withX(startX).withWidth(knobWidth);
    bassLabel.setBounds(bassArea.removeFromTop(20));
    bassSlider.setBounds(bassArea);

    // Mid
    auto midArea = eqSection.withX(startX + (knobWidth + spacing)).withWidth(knobWidth);
    midLabel.setBounds(midArea.removeFromTop(20));
    midSlider.setBounds(midArea);

    // Treble
    auto trebleArea = eqSection.withX(startX + (knobWidth + spacing) * 2).withWidth(knobWidth);
    trebleLabel.setBounds(trebleArea.removeFromTop(20));
    trebleSlider.setBounds(trebleArea);

    // Compression
    auto compArea = eqSection.withX(startX + (knobWidth + spacing) * 3).withWidth(knobWidth);
    compressionLabel.setBounds(compArea.removeFromTop(20));
    compressionSlider.setBounds(compArea);

    // Output
    auto outputArea = eqSection.withX(startX + (knobWidth + spacing) * 4).withWidth(knobWidth);
    outputGainLabel.setBounds(outputArea.removeFromTop(20));
    outputGainSlider.setBounds(outputArea);
}

void MIDIBassGuitarAudioProcessorEditor::timerCallback()
{
    // Sync GUI with parameters (for DAW automation)
    articulationSelector.setSelectedId(audioProcessor.articulationParam->getIndex() + 1, juce::dontSendNotification);
    toneSelector.setSelectedId(audioProcessor.toneParam->getIndex() + 1, juce::dontSendNotification);

    bassSlider.setValue(*audioProcessor.bassParam, juce::dontSendNotification);
    midSlider.setValue(*audioProcessor.midParam, juce::dontSendNotification);
    trebleSlider.setValue(*audioProcessor.trebleParam, juce::dontSendNotification);
    compressionSlider.setValue(*audioProcessor.compressionParam, juce::dontSendNotification);
    outputGainSlider.setValue(*audioProcessor.outputGainParam, juce::dontSendNotification);
}

void MIDIBassGuitarAudioProcessorEditor::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged == &articulationSelector)
    {
        int index = articulationSelector.getSelectedId() - 1;
        audioProcessor.articulationParam->setValueNotifyingHost(index / 3.0f);
    }
    else if (comboBoxThatHasChanged == &toneSelector)
    {
        int index = toneSelector.getSelectedId() - 1;
        audioProcessor.toneParam->setValueNotifyingHost(index / 4.0f);
    }
}

void MIDIBassGuitarAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &bassSlider)
        audioProcessor.bassParam->setValueNotifyingHost(bassSlider.getValue());
    else if (slider == &midSlider)
        audioProcessor.midParam->setValueNotifyingHost(midSlider.getValue());
    else if (slider == &trebleSlider)
        audioProcessor.trebleParam->setValueNotifyingHost(trebleSlider.getValue());
    else if (slider == &compressionSlider)
        audioProcessor.compressionParam->setValueNotifyingHost(compressionSlider.getValue());
    else if (slider == &outputGainSlider)
        audioProcessor.outputGainParam->setValueNotifyingHost(outputGainSlider.getValue());
}
