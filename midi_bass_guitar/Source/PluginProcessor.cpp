#include "PluginProcessor.h"
#include "PluginEditor.h"

MIDIBassGuitarAudioProcessor::MIDIBassGuitarAudioProcessor()
    : AudioProcessor(BusesProperties()
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true))
{
    // Articulation selection parameter
    addParameter(articulationParam = new juce::AudioParameterChoice(
        "articulation",
        "Articulation",
        juce::StringArray{"Picked", "Fingerstyle", "Slap", "Muted"},
        1)); // Default to Fingerstyle

    // Tone preset parameter
    addParameter(toneParam = new juce::AudioParameterChoice(
        "tone",
        "Tone",
        juce::StringArray{"DI", "Amp Sim", "Compressed", "Bright", "Vintage"},
        0)); // Default to DI

    // EQ parameters
    addParameter(bassParam = new juce::AudioParameterFloat(
        "bass",
        "Bass",
        0.0f, 1.0f,
        0.5f)); // Flat

    addParameter(midParam = new juce::AudioParameterFloat(
        "mid",
        "Mid",
        0.0f, 1.0f,
        0.5f)); // Flat

    addParameter(trebleParam = new juce::AudioParameterFloat(
        "treble",
        "Treble",
        0.0f, 1.0f,
        0.5f)); // Flat

    // Compression parameter
    addParameter(compressionParam = new juce::AudioParameterFloat(
        "compression",
        "Compression",
        0.0f, 1.0f,
        0.0f)); // Off by default

    // Output gain parameter
    addParameter(outputGainParam = new juce::AudioParameterFloat(
        "outputGain",
        "Output Gain",
        0.0f, 1.0f,
        0.7f)); // 70% default
}

MIDIBassGuitarAudioProcessor::~MIDIBassGuitarAudioProcessor()
{
}

const juce::String MIDIBassGuitarAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool MIDIBassGuitarAudioProcessor::acceptsMidi() const
{
    return true; // This is a virtual instrument - needs MIDI!
}

bool MIDIBassGuitarAudioProcessor::producesMidi() const
{
    return false;
}

bool MIDIBassGuitarAudioProcessor::isMidiEffect() const
{
    return false;
}

double MIDIBassGuitarAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int MIDIBassGuitarAudioProcessor::getNumPrograms()
{
    return 1;
}

int MIDIBassGuitarAudioProcessor::getCurrentProgram()
{
    return 0;
}

void MIDIBassGuitarAudioProcessor::setCurrentProgram(int index)
{
    juce::ignoreUnused(index);
}

const juce::String MIDIBassGuitarAudioProcessor::getProgramName(int index)
{
    juce::ignoreUnused(index);
    return {};
}

void MIDIBassGuitarAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
    juce::ignoreUnused(index, newName);
}

void MIDIBassGuitarAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Prepare sampler engine
    sampler.prepare(sampleRate, samplesPerBlock, getTotalNumOutputChannels());
}

void MIDIBassGuitarAudioProcessor::releaseResources()
{
    sampler.reset();
}

bool MIDIBassGuitarAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    // Only support stereo output
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    return true;
}

void MIDIBassGuitarAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                                juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    // Update sampler parameters from plugin parameters
    // Articulation
    int articulationIndex = articulationParam->getIndex();
    BassArticulation articulation = static_cast<BassArticulation>(articulationIndex);
    sampler.setArticulation(articulation);

    // Tone preset
    int toneIndex = toneParam->getIndex();
    BassTone tone = static_cast<BassTone>(toneIndex);
    sampler.setTone(tone);

    // EQ
    sampler.setBass(*bassParam);
    sampler.setMid(*midParam);
    sampler.setTreble(*trebleParam);

    // Dynamics
    sampler.setCompression(*compressionParam);

    // Output
    sampler.setOutputGain(*outputGainParam);

    // Process MIDI and generate audio through sampler
    // This calls the synthesiser's renderNextBlock with MIDI, then applies effects
    sampler.processBlock(buffer, midiMessages);
}

bool MIDIBassGuitarAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* MIDIBassGuitarAudioProcessor::createEditor()
{
    return new MIDIBassGuitarAudioProcessorEditor(*this);
}

void MIDIBassGuitarAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // Save parameter states
    juce::MemoryOutputStream stream(destData, true);

    stream.writeInt(articulationParam->getIndex());
    stream.writeInt(toneParam->getIndex());
    stream.writeFloat(*bassParam);
    stream.writeFloat(*midParam);
    stream.writeFloat(*trebleParam);
    stream.writeFloat(*compressionParam);
    stream.writeFloat(*outputGainParam);
}

void MIDIBassGuitarAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // Restore parameter states
    juce::MemoryInputStream stream(data, static_cast<size_t>(sizeInBytes), false);

    articulationParam->setValueNotifyingHost(articulationParam->convertTo0to1(stream.readInt()));
    toneParam->setValueNotifyingHost(toneParam->convertTo0to1(stream.readInt()));
    bassParam->setValueNotifyingHost(stream.readFloat());
    midParam->setValueNotifyingHost(stream.readFloat());
    trebleParam->setValueNotifyingHost(stream.readFloat());
    compressionParam->setValueNotifyingHost(stream.readFloat());
    outputGainParam->setValueNotifyingHost(stream.readFloat());
}

// This creates new instances of the plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MIDIBassGuitarAudioProcessor();
}
