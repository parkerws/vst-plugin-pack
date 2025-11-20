#pragma once
#include <JuceHeader.h>
#include "BassVoice.h"
#include "BassEffects.h"

/**
 * BassSamplerEngine - Main sampler engine for bass guitar virtual instrument
 *
 * Manages:
 * - MIDI input and note routing
 * - Multiple voices for polyphony
 * - Articulation selection
 * - Effects processing chain
 * - Output mixing
 *
 * Architecture:
 * MIDI In -> Synthesiser (BassVoice instances) -> Effects -> Audio Out
 */
class BassSamplerEngine
{
public:
    BassSamplerEngine();
    ~BassSamplerEngine();

    void prepare(double sampleRate, int samplesPerBlock, int numChannels);
    void reset();

    // Process MIDI and audio together
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages);

    // Articulation control
    void setArticulation(BassArticulation articulation);
    BassArticulation getCurrentArticulation() const { return currentArticulation; }

    // Effects control (pass-through to BassEffects)
    void setTone(BassTone tone);
    BassTone getCurrentTone() const;

    void setBass(float bass);
    void setMid(float mid);
    void setTreble(float treble);
    void setCompression(float compression);
    void setOutputGain(float gain);

private:
    // JUCE Synthesiser manages multiple voices
    juce::Synthesiser synthesiser;

    // Effects processor
    BassEffects effects;

    // Current settings
    BassArticulation currentArticulation = BassArticulation::Fingerstyle;

    // Number of simultaneous notes (polyphony)
    static constexpr int maxVoices = 8;
};
