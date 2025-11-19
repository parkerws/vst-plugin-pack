#include "PluginProcessor.h"
#include "PluginEditor.h"

JazzChorusAmpAudioProcessorEditor::JazzChorusAmpAudioProcessorEditor(
    JazzChorusAmpAudioProcessor& p)
    : AudioProcessorEditor(&p),
      audioProcessor(p),
      volumeKnob("VOLUME"),
      bassKnob("BASS"),
      middleKnob("MIDDLE"),
      trebleKnob("TREBLE"),
      brightKnob("BRIGHT"),
      chorusRateKnob("RATE"),
      chorusDepthKnob("DEPTH"),
      chorusMixKnob("MIX")
{
    // Set plugin window size - amp panel dimensions
    setSize(750, 400);

    // Configure knob display ranges
    volumeKnob.setDisplayRange(0.0f, 10.0f, "");
    bassKnob.setDisplayRange(-12.0f, 12.0f, "dB");
    middleKnob.setDisplayRange(-12.0f, 12.0f, "dB");
    trebleKnob.setDisplayRange(-12.0f, 12.0f, "dB");
    brightKnob.setDisplayRange(0.0f, 10.0f, "");
    chorusRateKnob.setDisplayRange(0.1f, 5.0f, "Hz");
    chorusDepthKnob.setDisplayRange(0.0f, 10.0f, "");
    chorusMixKnob.setDisplayRange(0.0f, 100.0f, "%");

    // Wire up knobs to processor parameters
    volumeKnob.onValueChange = [this](float value)
    {
        audioProcessor.volumeParam->setValueNotifyingHost(value);
    };

    bassKnob.onValueChange = [this](float value)
    {
        audioProcessor.bassParam->setValueNotifyingHost(value);
    };

    middleKnob.onValueChange = [this](float value)
    {
        audioProcessor.middleParam->setValueNotifyingHost(value);
    };

    trebleKnob.onValueChange = [this](float value)
    {
        audioProcessor.trebleParam->setValueNotifyingHost(value);
    };

    brightKnob.onValueChange = [this](float value)
    {
        audioProcessor.brightParam->setValueNotifyingHost(value);
    };

    chorusRateKnob.onValueChange = [this](float value)
    {
        audioProcessor.chorusRateParam->setValueNotifyingHost(value);
    };

    chorusDepthKnob.onValueChange = [this](float value)
    {
        audioProcessor.chorusDepthParam->setValueNotifyingHost(value);
    };

    chorusMixKnob.onValueChange = [this](float value)
    {
        audioProcessor.chorusMixParam->setValueNotifyingHost(value);
    };

    // Setup chorus button
    chorusButton.setButtonText("CHORUS");
    chorusButton.setClickingTogglesState(true);
    chorusButton.setToggleState(*audioProcessor.chorusEnabledParam, juce::dontSendNotification);
    chorusButton.onClick = [this]()
    {
        audioProcessor.chorusEnabledParam->setValueNotifyingHost(chorusButton.getToggleState() ? 1.0f : 0.0f);
    };

    // Add all knobs to editor
    addAndMakeVisible(volumeKnob);
    addAndMakeVisible(bassKnob);
    addAndMakeVisible(middleKnob);
    addAndMakeVisible(trebleKnob);
    addAndMakeVisible(brightKnob);
    addAndMakeVisible(chorusRateKnob);
    addAndMakeVisible(chorusDepthKnob);
    addAndMakeVisible(chorusMixKnob);
    addAndMakeVisible(chorusButton);

    // Start timer to sync GUI with parameter changes (e.g., from DAW automation)
    startTimerHz(30); // 30 FPS update rate
}

JazzChorusAmpAudioProcessorEditor::~JazzChorusAmpAudioProcessorEditor()
{
}

void JazzChorusAmpAudioProcessorEditor::paint(juce::Graphics& g)
{
    // Background - light silver panel (Jazz Chorus aesthetic)
    g.fillAll(panelColor);

    // Draw subtle gradient for depth
    {
        juce::ColourGradient gradient(panelColor.brighter(0.1f), getWidth() / 2.0f, 0.0f,
                                      panelColor.darker(0.1f), getWidth() / 2.0f, getHeight(), false);
        g.setGradientFill(gradient);
        g.fillRect(getLocalBounds());
    }

    // Draw top bar with logo
    {
        auto topBar = getLocalBounds().removeFromTop(70);

        // Black accent strip
        g.setColour(logoColor);
        g.fillRect(topBar.removeFromTop(4));

        // Logo/title
        g.setColour(logoColor);
        g.setFont(juce::Font("Arial", 36.0f, juce::Font::bold));
        g.drawText("JAZZ CHORUS", topBar, juce::Justification::centred);
    }

    // Draw section labels
    {
        g.setColour(textColor.withAlpha(0.7f));
        g.setFont(12.0f);

        // EQ section
        g.drawText("EQUALIZER", 50, 90, 180, 20, juce::Justification::centred);

        // Chorus section
        g.drawText("STEREO CHORUS", 280, 90, 400, 20, juce::Justification::centred);

        // Master section
        g.drawText("MASTER", 580, 90, 120, 20, juce::Justification::centred);
    }

    // Draw decorative elements
    {
        g.setColour(darkAccent);

        // Vertical separators
        g.drawLine(250, 110, 250, 350, 2.0f);
        g.drawLine(560, 110, 560, 350, 2.0f);

        // Corner screws (visual detail)
        g.setColour(darkAccent.darker(0.5f));
        for (auto x : {20, getWidth() - 20})
        {
            for (auto y : {20, getHeight() - 20})
            {
                g.fillEllipse(x - 4, y - 4, 8, 8);
                g.setColour(silverColor);
                g.drawEllipse(x - 4, y - 4, 8, 8, 1.0f);
                g.setColour(darkAccent.darker(0.5f));
            }
        }
    }

    // Draw model name at bottom
    {
        g.setColour(textColor.withAlpha(0.8f));
        g.setFont(juce::Font("Arial", 16.0f, juce::Font::bold));
        auto bottomArea = getLocalBounds().removeFromBottom(35);
        g.drawText("SOLID STATE AMPLIFIER", bottomArea, juce::Justification::centred);
    }

    // Draw chorus section background highlight
    {
        auto chorusSection = juce::Rectangle<int>(260, 100, 290, 260);
        g.setColour(accentColor.withAlpha(0.3f));
        g.fillRect(chorusSection);
        g.setColour(darkAccent.withAlpha(0.3f));
        g.drawRect(chorusSection, 1);
    }
}

void JazzChorusAmpAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    bounds.removeFromTop(110);  // Skip header area
    bounds.removeFromBottom(35); // Skip footer area

    const int knobWidth = 80;
    const int knobHeight = 100;
    const int spacing = 10;

    // Layout knobs in sections

    // EQ section (left) - 2 rows
    auto eqSection = bounds.removeFromLeft(250);

    // Top row: Volume, Bright
    auto topRow = eqSection.removeFromTop(130);
    volumeKnob.setBounds(topRow.getCentreX() - knobWidth - spacing / 2,
                         topRow.getCentreY() - knobHeight / 2,
                         knobWidth, knobHeight);
    brightKnob.setBounds(topRow.getCentreX() + spacing / 2,
                         topRow.getCentreY() - knobHeight / 2,
                         knobWidth, knobHeight);

    // Bottom row: Bass, Middle, Treble
    auto bottomRow = eqSection;
    int totalWidth = (knobWidth * 3) + (spacing * 2);
    int startX = bottomRow.getCentreX() - totalWidth / 2;
    int y = bottomRow.getCentreY() - knobHeight / 2;

    bassKnob.setBounds(startX, y, knobWidth, knobHeight);
    middleKnob.setBounds(startX + knobWidth + spacing, y, knobWidth, knobHeight);
    trebleKnob.setBounds(startX + (knobWidth + spacing) * 2, y, knobWidth, knobHeight);

    // Chorus section (center)
    auto chorusSection = bounds.removeFromLeft(300);

    // Chorus knobs in 2 rows
    auto chorusTopRow = chorusSection.removeFromTop(120);
    chorusRateKnob.setBounds(chorusTopRow.getCentreX() - knobWidth - spacing / 2,
                             chorusTopRow.getCentreY() - knobHeight / 2,
                             knobWidth, knobHeight);
    chorusDepthKnob.setBounds(chorusTopRow.getCentreX() + spacing / 2,
                              chorusTopRow.getCentreY() - knobHeight / 2,
                              knobWidth, knobHeight);

    // Chorus mix and button
    auto chorusBottomRow = chorusSection;
    chorusMixKnob.setBounds(chorusBottomRow.getCentreX() - knobWidth / 2,
                            chorusBottomRow.getY() + 10,
                            knobWidth, knobHeight);

    // Chorus button below mix knob
    chorusButton.setBounds(chorusBottomRow.getCentreX() - 50,
                           chorusBottomRow.getY() + 120,
                           100, 30);
}

void JazzChorusAmpAudioProcessorEditor::timerCallback()
{
    // Sync GUI knobs with current parameter values
    // This handles DAW automation and preset loading
    volumeKnob.setValue(*audioProcessor.volumeParam);
    bassKnob.setValue(*audioProcessor.bassParam);
    middleKnob.setValue(*audioProcessor.middleParam);
    trebleKnob.setValue(*audioProcessor.trebleParam);
    brightKnob.setValue(*audioProcessor.brightParam);
    chorusRateKnob.setValue(*audioProcessor.chorusRateParam);
    chorusDepthKnob.setValue(*audioProcessor.chorusDepthParam);
    chorusMixKnob.setValue(*audioProcessor.chorusMixParam);
    chorusButton.setToggleState(*audioProcessor.chorusEnabledParam, juce::dontSendNotification);
}
