#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SimpleGainPluginAudioProcessorEditor::SimpleGainPluginAudioProcessorEditor(SimpleGainPluginAudioProcessor &p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    // Set the size of the plugin window
    setSize(400, 300);

    // Configure the gain slider
    gainSlider.setSliderStyle(juce::Slider::LinearVertical);
    gainSlider.setRange(-60.0, 12.0, 0.1);
    gainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 20);
    gainSlider.setValue(0.0);

    // Customize the slider colors for a modern look
    gainSlider.setColour(juce::Slider::thumbColourId, juce::Colour(0xffef8354));             // Orange thumb
    gainSlider.setColour(juce::Slider::trackColourId, juce::Colour(0xff4f5d75));             // Grey-blue track
    gainSlider.setColour(juce::Slider::backgroundColourId, juce::Colour(0xff1a1d2e));        // Dark background
    gainSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);           // White text
    gainSlider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xff2d3142)); // Dark text box
    gainSlider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colour(0xff4f5d75));    // Border

    // Add a listener so we know when the slider changes
    gainSlider.addListener(this);

    // Make the slider visible
    addAndMakeVisible(gainSlider);

    // Configure the label
    gainLabel.setText("Gain (dB)", juce::dontSendNotification);
    gainLabel.attachToComponent(&gainSlider, false);
    gainLabel.setJustificationType(juce::Justification::centredTop);

    // Make the label visible
    addAndMakeVisible(gainLabel);

    gainSlider.setTooltip("Adjust output volume\n"
                          "0dB = No change (unity gain)\n"
                          "+6dB = 2x louder\n"
                          "-6dB = Half volume\n"
                          "-60dB = Silence");

    gainLabel.setTooltip("Gain control adjusts the output level of your audio signal");
}

SimpleGainPluginAudioProcessorEditor::~SimpleGainPluginAudioProcessorEditor()
{
    gainSlider.removeListener(this);
}

//==============================================================================
void SimpleGainPluginAudioProcessorEditor::paint(juce::Graphics &g)
{
    juce::ColourGradient gradient(
        juce::Colour(0xff2d3142),
        0.0f, 0.0f,
        juce::Colour(0xff1a1d2e),
        0.0F, (float)getHeight(),
        false);

    g.setGradientFill(gradient);
    g.fillAll();

    // Draw a subtle border around the whole plugin
    g.setColour(juce::Colour(0xff4f5d75));
    g.drawRect(getLocalBounds(), 2);

    // Draw the title with a shadow effect for depth
    auto titleArea = getLocalBounds().removeFromTop(50);

    // Shadow (offset slightly)
    g.setColour(juce::Colours::black.withAlpha(0.5f));
    g.setFont(juce::Font(24.0f, juce::Font::bold));
    g.drawFittedText("Simple Gain", titleArea.translated(2, 2),
                     juce::Justification::centred, 1);

    // Main title text
    g.setColour(juce::Colour(0xffef8354)); // Warm orange
    g.drawFittedText("Simple Gain", titleArea,
                     juce::Justification::centred, 1);

    // Draw a panel background for the controls
    auto controlArea = getLocalBounds().reduced(20, 70);
    g.setColour(juce::Colour(0xff1a1d2e).withAlpha(0.6f));
    g.fillRoundedRectangle(controlArea.toFloat(), 10.0f);

    // Panel outline
    g.setColour(juce::Colour(0xff4f5d75));
    g.drawRoundedRectangle(controlArea.toFloat(), 10.0f, 1.5f);
}

void SimpleGainPluginAudioProcessorEditor::resized()
{
    // Position the slider in the center of the window
    auto bounds = getLocalBounds();
    bounds.removeFromTop(60); // Leave space for title
    gainSlider.setBounds(bounds.withSizeKeepingCentre(100, 180));
}
void SimpleGainPluginAudioProcessorEditor::sliderValueChanged(juce::Slider *slider)
{
    if (slider == &gainSlider)
    {
        // When slider moves, update the processor's gain parameter
        audioProcessor.gainParameter->setValueNotifyingHost(
            audioProcessor.gainParameter->convertTo0to1(gainSlider.getValue()));
    }
}