#include "BassSamplerEngine.h"

BassSamplerEngine::BassSamplerEngine()
{
    // Add sounds to the synthesiser
    // BassSound defines what MIDI notes can be played
    synthesiser.addSound(new BassSound());

    // Add voices (polyphony)
    for (int i = 0; i < maxVoices; ++i)
    {
        synthesiser.addVoice(new BassVoice());
    }
}

BassSamplerEngine::~BassSamplerEngine()
{
}

void BassSamplerEngine::prepare(double sampleRate, int samplesPerBlock, int numChannels)
{
    // Prepare synthesiser
    synthesiser.setCurrentPlaybackSampleRate(sampleRate);

    // Prepare all voices
    for (int i = 0; i < synthesiser.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<BassVoice*>(synthesiser.getVoice(i)))
        {
            voice->prepare(sampleRate, samplesPerBlock);
            voice->setArticulation(currentArticulation);
        }
    }

    // Prepare effects
    effects.prepare(sampleRate, samplesPerBlock, numChannels);

    reset();
}

void BassSamplerEngine::reset()
{
    synthesiser.allNotesOff(0, false);
    effects.reset();
}

void BassSamplerEngine::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    // Clear buffer
    buffer.clear();

    // Synthesiser renders all active voices into the buffer
    // It processes the MIDI messages and generates audio
    synthesiser.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());

    // Apply effects processing
    effects.processBlock(buffer);
}

void BassSamplerEngine::setArticulation(BassArticulation articulation)
{
    currentArticulation = articulation;

    // Update all voices with new articulation
    for (int i = 0; i < synthesiser.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<BassVoice*>(synthesiser.getVoice(i)))
        {
            voice->setArticulation(articulation);
        }
    }
}

// Effects pass-through methods
void BassSamplerEngine::setTone(BassTone tone)
{
    effects.setTone(tone);
}

BassTone BassSamplerEngine::getCurrentTone() const
{
    return effects.getCurrentTone();
}

void BassSamplerEngine::setBass(float bass)
{
    effects.setBass(bass);
}

void BassSamplerEngine::setMid(float mid)
{
    effects.setMid(mid);
}

void BassSamplerEngine::setTreble(float treble)
{
    effects.setTreble(treble);
}

void BassSamplerEngine::setCompression(float compression)
{
    effects.setCompression(compression);
}

void BassSamplerEngine::setOutputGain(float gain)
{
    effects.setOutputGain(gain);
}
