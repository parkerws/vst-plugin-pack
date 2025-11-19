#include "PluginProcessor.h"
#include "PluginEditor.h"

OrangeAmpSimulatorAudioProcessor::OrangeAmpSimulatorAudioProcessor()
    : AudioProcessor(BusesProperties()
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true))
{
    // Create parameters with appropriate ranges and default values
    // All parameters use normalized 0.0-1.0 range internally

    addParameter(preampGainParam = new juce::AudioParameterFloat(
        "preampGain",           // Parameter ID
        "Preamp Gain",          // Parameter name
        0.0f, 1.0f,             // Range (normalized)
        0.5f));                 // Default value

    addParameter(bassParam = new juce::AudioParameterFloat(
        "bass",
        "Bass",
        0.0f, 1.0f,
        0.5f));                 // 0.5 = flat (no boost/cut)

    addParameter(middleParam = new juce::AudioParameterFloat(
        "middle",
        "Middle",
        0.0f, 1.0f,
        0.5f));

    addParameter(trebleParam = new juce::AudioParameterFloat(
        "treble",
        "Treble",
        0.0f, 1.0f,
        0.5f));

    addParameter(driveParam = new juce::AudioParameterFloat(
        "drive",
        "Drive",
        0.0f, 1.0f,
        0.3f));                 // Default moderate drive

    addParameter(presenceParam = new juce::AudioParameterFloat(
        "presence",
        "Presence",
        0.0f, 1.0f,
        0.5f));

    addParameter(masterVolumeParam = new juce::AudioParameterFloat(
        "masterVolume",
        "Master Volume",
        0.0f, 1.0f,
        0.7f));                 // Default 70% volume
}

OrangeAmpSimulatorAudioProcessor::~OrangeAmpSimulatorAudioProcessor()
{
}

const juce::String OrangeAmpSimulatorAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool OrangeAmpSimulatorAudioProcessor::acceptsMidi() const
{
    return false;
}

bool OrangeAmpSimulatorAudioProcessor::producesMidi() const
{
    return false;
}

bool OrangeAmpSimulatorAudioProcessor::isMidiEffect() const
{
    return false;
}

double OrangeAmpSimulatorAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int OrangeAmpSimulatorAudioProcessor::getNumPrograms()
{
    return 1;
}

int OrangeAmpSimulatorAudioProcessor::getCurrentProgram()
{
    return 0;
}

void OrangeAmpSimulatorAudioProcessor::setCurrentProgram(int index)
{
    juce::ignoreUnused(index);
}

const juce::String OrangeAmpSimulatorAudioProcessor::getProgramName(int index)
{
    juce::ignoreUnused(index);
    return {};
}

void OrangeAmpSimulatorAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
    juce::ignoreUnused(index, newName);
}

void OrangeAmpSimulatorAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Prepare DSP engine
    ampSim.prepare(sampleRate, samplesPerBlock, getTotalNumInputChannels());
}

void OrangeAmpSimulatorAudioProcessor::releaseResources()
{
    ampSim.reset();
}

bool OrangeAmpSimulatorAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    // Support mono and stereo
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // Input and output layout must match
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;

    return true;
}

void OrangeAmpSimulatorAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                                     juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(midiMessages);

    juce::ScopedNoDenormals noDenormals;

    // Update DSP parameters from plugin parameters
    // These are read from the parameter pointers (set by DAW or GUI)
    ampSim.setPreampGain(*preampGainParam);
    ampSim.setBass(*bassParam);
    ampSim.setMiddle(*middleParam);
    ampSim.setTreble(*trebleParam);
    ampSim.setDrive(*driveParam);
    ampSim.setPresence(*presenceParam);
    ampSim.setMasterVolume(*masterVolumeParam);

    // Process audio through amp simulator
    ampSim.processBlock(buffer);
}

bool OrangeAmpSimulatorAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* OrangeAmpSimulatorAudioProcessor::createEditor()
{
    return new OrangeAmpSimulatorAudioProcessorEditor(*this);
}

void OrangeAmpSimulatorAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // Save parameter states for DAW project persistence
    juce::MemoryOutputStream stream(destData, true);

    stream.writeFloat(*preampGainParam);
    stream.writeFloat(*bassParam);
    stream.writeFloat(*middleParam);
    stream.writeFloat(*trebleParam);
    stream.writeFloat(*driveParam);
    stream.writeFloat(*presenceParam);
    stream.writeFloat(*masterVolumeParam);
}

void OrangeAmpSimulatorAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // Restore parameter states when loading DAW project
    juce::MemoryInputStream stream(data, static_cast<size_t>(sizeInBytes), false);

    preampGainParam->setValueNotifyingHost(stream.readFloat());
    bassParam->setValueNotifyingHost(stream.readFloat());
    middleParam->setValueNotifyingHost(stream.readFloat());
    trebleParam->setValueNotifyingHost(stream.readFloat());
    driveParam->setValueNotifyingHost(stream.readFloat());
    presenceParam->setValueNotifyingHost(stream.readFloat());
    masterVolumeParam->setValueNotifyingHost(stream.readFloat());
}

// This creates new instances of the plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new OrangeAmpSimulatorAudioProcessor();
}
