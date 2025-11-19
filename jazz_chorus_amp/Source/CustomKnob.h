#pragma once
#include <JuceHeader.h>

/**
 * CustomKnob - Rotary knob component styled for Jazz Chorus amp
 *
 * Features:
 * - Rotary control with mouse drag
 * - Visual indicator line showing current position
 * - Text label below knob
 * - Value display when hovering/dragging
 * - Silver/chrome color scheme to match Jazz Chorus aesthetic
 */
class CustomKnob : public juce::Component
{
public:
    CustomKnob(const juce::String& labelText);
    ~CustomKnob() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseDrag(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;
    void mouseEnter(const juce::MouseEvent& event) override;
    void mouseExit(const juce::MouseEvent& event) override;

    // Value management (0.0 to 1.0)
    void setValue(float newValue);
    float getValue() const { return value; }

    // Callback when value changes
    std::function<void(float)> onValueChange;

    // Display range (for showing values to user)
    void setDisplayRange(float minVal, float maxVal, const juce::String& suffix = "");

private:
    juce::String label;
    float value = 0.5f;          // Current value (0-1)
    bool isDragging = false;
    bool isHovered = false;
    int dragStartY = 0;
    float dragStartValue = 0.0f;

    // Display range
    float displayMin = 0.0f;
    float displayMax = 10.0f;
    juce::String displaySuffix;

    // Colors - Silver/Chrome aesthetic for Jazz Chorus
    const juce::Colour knobColor = juce::Colour(0xFFC0C0C0);        // Silver
    const juce::Colour knobDarkColor = juce::Colour(0xFF909090);    // Dark silver
    const juce::Colour knobHighlight = juce::Colour(0xFFE8E8E8);    // Bright silver
    const juce::Colour panelColor = juce::Colour(0xFF2A2A2A);       // Dark background
    const juce::Colour textColor = juce::Colour(0xFF303030);        // Dark text
    const juce::Colour indicatorColor = juce::Colour(0xFF000000);   // Black indicator

    // Helper to get display value string
    juce::String getDisplayValueString() const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CustomKnob)
};
