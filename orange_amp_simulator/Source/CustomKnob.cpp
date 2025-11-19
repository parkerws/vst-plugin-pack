#include "CustomKnob.h"

CustomKnob::CustomKnob(const juce::String& labelText)
    : label(labelText)
{
    setSize(80, 100); // Default size
}

CustomKnob::~CustomKnob()
{
}

void CustomKnob::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    auto knobArea = bounds.removeFromTop(80).reduced(10);

    // Draw knob body (3D effect with gradient)
    {
        juce::ColourGradient gradient(knobDarkColor, knobArea.getCentreX(), knobArea.getY(),
                                      knobColor, knobArea.getCentreX(), knobArea.getBottom(), false);
        g.setGradientFill(gradient);
        g.fillEllipse(knobArea.toFloat());

        // Outer ring (darker)
        g.setColour(knobDarkColor);
        g.drawEllipse(knobArea.toFloat(), 2.0f);

        // Inner highlight
        auto highlightArea = knobArea.reduced(5);
        g.setColour(knobColor.brighter(0.3f));
        g.drawEllipse(highlightArea.toFloat(), 1.5f);
    }

    // Draw indicator line showing knob position
    {
        // Rotation: -135° to +135° (270° total range)
        const float rotationAngle = -2.356f + (value * 4.712f); // -135° to +135° in radians
        const float centerX = knobArea.getCentreX();
        const float centerY = knobArea.getCentreY();
        const float radius = knobArea.getWidth() * 0.35f;

        const float indicatorX = centerX + radius * std::cos(rotationAngle);
        const float indicatorY = centerY + radius * std::sin(rotationAngle);

        g.setColour(indicatorColor);
        g.drawLine(centerX, centerY, indicatorX, indicatorY, 3.0f);

        // Draw dot at end of indicator
        g.fillEllipse(indicatorX - 3, indicatorY - 3, 6, 6);
    }

    // Draw label text
    {
        g.setColour(textColor);
        g.setFont(14.0f);
        g.drawText(label, bounds, juce::Justification::centred);
    }

    // Draw value when hovering or dragging
    if (isHovered || isDragging)
    {
        g.setColour(knobColor);
        g.setFont(12.0f);
        auto valueArea = knobArea.reduced(15);
        g.drawText(getDisplayValueString(), valueArea, juce::Justification::centred);
    }
}

void CustomKnob::resized()
{
}

void CustomKnob::mouseDown(const juce::MouseEvent& event)
{
    isDragging = true;
    dragStartY = event.getPosition().y;
    dragStartValue = value;
    repaint();
}

void CustomKnob::mouseDrag(const juce::MouseEvent& event)
{
    if (isDragging)
    {
        // Vertical drag changes value
        const int dragDistance = dragStartY - event.getPosition().y;
        const float sensitivity = 0.005f; // Adjust for desired sensitivity
        float newValue = dragStartValue + (dragDistance * sensitivity);

        // Clamp to 0-1 range
        newValue = juce::jlimit(0.0f, 1.0f, newValue);

        if (newValue != value)
        {
            setValue(newValue);

            if (onValueChange)
                onValueChange(value);
        }
    }
}

void CustomKnob::mouseUp(const juce::MouseEvent& event)
{
    juce::ignoreUnused(event);
    isDragging = false;
    repaint();
}

void CustomKnob::mouseEnter(const juce::MouseEvent& event)
{
    juce::ignoreUnused(event);
    isHovered = true;
    repaint();
}

void CustomKnob::mouseExit(const juce::MouseEvent& event)
{
    juce::ignoreUnused(event);
    isHovered = false;
    repaint();
}

void CustomKnob::setValue(float newValue)
{
    value = juce::jlimit(0.0f, 1.0f, newValue);
    repaint();
}

void CustomKnob::setDisplayRange(float minVal, float maxVal, const juce::String& suffix)
{
    displayMin = minVal;
    displayMax = maxVal;
    displaySuffix = suffix;
}

juce::String CustomKnob::getDisplayValueString() const
{
    // Map 0-1 value to display range
    float displayValue = displayMin + (value * (displayMax - displayMin));

    // Format with 1 decimal place
    juce::String valueStr = juce::String(displayValue, 1);
    return valueStr + displaySuffix;
}
