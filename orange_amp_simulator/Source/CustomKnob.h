#pragma once
#include <JuceHeader.h>

/**
 * CustomKnob - Rotary knob component styled like a tube amp knob
 *
 * Features:
 * - Rotary control with mouse drag
 * - Visual indicator line showing current position
 * - Text label below knob
 * - Value display when hovering/dragging
 * - Orange color scheme to match amp aesthetic
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

    // Colors
    const juce::Colour knobColor = juce::Colour(0xFFFF8C00);        // Orange
    const juce::Colour knobDarkColor = juce::Colour(0xFFD67300);    // Darker orange
    const juce::Colour panelColor = juce::Colour(0xFF2A2A2A);       // Dark gray
    const juce::Colour textColor = juce::Colour(0xFFE0E0E0);        // Light gray
    const juce::Colour indicatorColor = juce::Colour(0xFFFFFFFF);   // White

    // Helper to get display value string
    juce::String getDisplayValueString() const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CustomKnob)
};
