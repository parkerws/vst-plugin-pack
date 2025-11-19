#include "PluginProcessor.h"
#include "PluginEditor.h"

OrangeAmpSimulatorAudioProcessorEditor::OrangeAmpSimulatorAudioProcessorEditor(
    OrangeAmpSimulatorAudioProcessor& p)
    : AudioProcessorEditor(&p),
      audioProcessor(p),
      preampGainKnob("GAIN"),
      bassKnob("BASS"),
      middleKnob("MIDDLE"),
      trebleKnob("TREBLE"),
      driveKnob("DRIVE"),
      presenceKnob("PRESENCE"),
      masterVolumeKnob("MASTER")
{
    // Set plugin window size - amp panel dimensions
    setSize(700, 350);

    // Configure knob display ranges
    preampGainKnob.setDisplayRange(0.0f, 10.0f, "");
    bassKnob.setDisplayRange(-12.0f, 12.0f, "dB");
    middleKnob.setDisplayRange(-12.0f, 12.0f, "dB");
    trebleKnob.setDisplayRange(-12.0f, 12.0f, "dB");
    driveKnob.setDisplayRange(0.0f, 10.0f, "");
    presenceKnob.setDisplayRange(-6.0f, 6.0f, "dB");
    masterVolumeKnob.setDisplayRange(0.0f, 10.0f, "");

    // Wire up knobs to processor parameters
    preampGainKnob.onValueChange = [this](float value)
    {
        audioProcessor.preampGainParam->setValueNotifyingHost(value);
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

    driveKnob.onValueChange = [this](float value)
    {
        audioProcessor.driveParam->setValueNotifyingHost(value);
    };

    presenceKnob.onValueChange = [this](float value)
    {
        audioProcessor.presenceParam->setValueNotifyingHost(value);
    };

    masterVolumeKnob.onValueChange = [this](float value)
    {
        audioProcessor.masterVolumeParam->setValueNotifyingHost(value);
    };

    // Add all knobs to editor
    addAndMakeVisible(preampGainKnob);
    addAndMakeVisible(bassKnob);
    addAndMakeVisible(middleKnob);
    addAndMakeVisible(trebleKnob);
    addAndMakeVisible(driveKnob);
    addAndMakeVisible(presenceKnob);
    addAndMakeVisible(masterVolumeKnob);

    // Start timer to sync GUI with parameter changes (e.g., from DAW automation)
    startTimerHz(30); // 30 FPS update rate
}

OrangeAmpSimulatorAudioProcessorEditor::~OrangeAmpSimulatorAudioProcessorEditor()
{
}

void OrangeAmpSimulatorAudioProcessorEditor::paint(juce::Graphics& g)
{
    // Background - dark panel
    g.fillAll(panelColor);

    // Draw amp panel texture/gradient
    {
        juce::ColourGradient gradient(accentColor, getWidth() / 2.0f, 0.0f,
                                      panelColor, getWidth() / 2.0f, getHeight(), false);
        g.setGradientFill(gradient);
        g.fillRect(getLocalBounds());
    }

    // Draw top bar with logo
    {
        auto topBar = getLocalBounds().removeFromTop(60);

        // Orange accent strip
        g.setColour(orangeColor);
        g.fillRect(topBar.removeFromTop(8));

        // Logo/title
        g.setColour(logoColor);
        g.setFont(juce::Font("Arial", 32.0f, juce::Font::bold));
        g.drawText("ORANGE", topBar, juce::Justification::centred);
    }

    // Draw section labels
    {
        g.setColour(textColor.withAlpha(0.6f));
        g.setFont(11.0f);

        // Preamp section
        g.drawText("PREAMP", 30, 80, 120, 20, juce::Justification::centred);

        // Tone stack section
        g.drawText("TONE STACK", 200, 80, 300, 20, juce::Justification::centred);

        // Master section
        g.drawText("MASTER", 550, 80, 120, 20, juce::Justification::centred);
    }

    // Draw decorative elements
    {
        g.setColour(accentColor.brighter(0.2f));

        // Vertical separators
        g.drawLine(170, 100, 170, 300, 1.0f);
        g.drawLine(530, 100, 530, 300, 1.0f);

        // Corner screws (visual detail)
        g.setColour(accentColor.darker(0.5f));
        for (auto x : {15, getWidth() - 15})
        {
            for (auto y : {15, getHeight() - 15})
            {
                g.fillEllipse(x - 3, y - 3, 6, 6);
                g.setColour(accentColor.brighter(0.3f));
                g.drawEllipse(x - 3, y - 3, 6, 6, 1.0f);
                g.setColour(accentColor.darker(0.5f));
            }
        }
    }

    // Draw model name at bottom
    {
        g.setColour(orangeColor.withAlpha(0.7f));
        g.setFont(juce::Font("Arial", 14.0f, juce::Font::bold));
        auto bottomArea = getLocalBounds().removeFromBottom(30);
        g.drawText("TUBE AMP SIMULATOR", bottomArea, juce::Justification::centred);
    }
}

void OrangeAmpSimulatorAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    bounds.removeFromTop(100);  // Skip header area
    bounds.removeFromBottom(30); // Skip footer area

    const int knobWidth = 80;
    const int knobHeight = 100;
    const int spacing = 20;

    // Layout knobs in sections

    // Preamp section (left)
    auto preampSection = bounds.removeFromLeft(170);
    preampGainKnob.setBounds(preampSection.getCentreX() - knobWidth / 2,
                             preampSection.getCentreY() - knobHeight / 2,
                             knobWidth, knobHeight);

    bounds.removeFromLeft(30); // Spacing

    // Tone stack section (center) - 2 rows
    auto toneSection = bounds.removeFromLeft(300);

    // Top row: Gain and Drive
    auto topRow = toneSection.removeFromTop(120);
    driveKnob.setBounds(topRow.getCentreX() - knobWidth / 2,
                        topRow.getCentreY() - knobHeight / 2,
                        knobWidth, knobHeight);

    // Bottom row: Bass, Middle, Treble, Presence
    auto bottomRow = toneSection;
    int totalWidth = (knobWidth * 3) + (spacing * 2);
    int startX = bottomRow.getCentreX() - totalWidth / 2;
    int y = bottomRow.getCentreY() - knobHeight / 2;

    bassKnob.setBounds(startX, y, knobWidth, knobHeight);
    middleKnob.setBounds(startX + knobWidth + spacing, y, knobWidth, knobHeight);
    trebleKnob.setBounds(startX + (knobWidth + spacing) * 2, y, knobWidth, knobHeight);

    bounds.removeFromLeft(30); // Spacing

    // Master section (right)
    auto masterSection = bounds;

    // Stack presence above master
    presenceKnob.setBounds(masterSection.getCentreX() - knobWidth / 2,
                           masterSection.getCentreY() - knobHeight - 10,
                           knobWidth, knobHeight);

    masterVolumeKnob.setBounds(masterSection.getCentreX() - knobWidth / 2,
                               masterSection.getCentreY() + 10,
                               knobWidth, knobHeight);
}

void OrangeAmpSimulatorAudioProcessorEditor::timerCallback()
{
    // Sync GUI knobs with current parameter values
    // This handles DAW automation and preset loading
    preampGainKnob.setValue(*audioProcessor.preampGainParam);
    bassKnob.setValue(*audioProcessor.bassParam);
    middleKnob.setValue(*audioProcessor.middleParam);
    trebleKnob.setValue(*audioProcessor.trebleParam);
    driveKnob.setValue(*audioProcessor.driveParam);
    presenceKnob.setValue(*audioProcessor.presenceParam);
    masterVolumeKnob.setValue(*audioProcessor.masterVolumeParam);
}
