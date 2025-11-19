#include "PluginProcessor.h"
#include "PluginEditor.h"

JazzChorusAmpAudioProcessor::JazzChorusAmpAudioProcessor()
    : AudioProcessor(BusesProperties()
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true))
{
    // Create parameters with appropriate ranges and default values
    // All parameters use normalized 0.0-1.0 range internally

    addParameter(volumeParam = new juce::AudioParameterFloat(
        "volume",
        "Volume",
        0.0f, 1.0f,
        0.7f));

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
        0.6f));                 // Slightly brighter by default

    addParameter(brightParam = new juce::AudioParameterFloat(
        "bright",
        "Bright",
        0.0f, 1.0f,
        0.5f));                 // Bright switch

    addParameter(chorusRateParam = new juce::AudioParameterFloat(
        "chorusRate",
        "Chorus Rate",
        0.0f, 1.0f,
        0.4f));                 // Default moderate rate

    addParameter(chorusDepthParam = new juce::AudioParameterFloat(
        "chorusDepth",
        "Chorus Depth",
        0.0f, 1.0f,
        0.6f));                 // Default moderate depth

    addParameter(chorusMixParam = new juce::AudioParameterFloat(
        "chorusMix",
        "Chorus Mix",
        0.0f, 1.0f,
        0.5f));                 // 50/50 mix

    addParameter(chorusEnabledParam = new juce::AudioParameterBool(
        "chorusEnabled",
        "Chorus Enabled",
        true));                 // On by default (it's the signature sound!)
}

JazzChorusAmpAudioProcessor::~JazzChorusAmpAudioProcessor()
{
}

const juce::String JazzChorusAmpAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool JazzChorusAmpAudioProcessor::acceptsMidi() const
{
    return false;
}

bool JazzChorusAmpAudioProcessor::producesMidi() const
{
    return false;
}

bool JazzChorusAmpAudioProcessor::isMidiEffect() const
{
    return false;
}

double JazzChorusAmpAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int JazzChorusAmpAudioProcessor::getNumPrograms()
{
    return 1;
}

int JazzChorusAmpAudioProcessor::getCurrentProgram()
{
    return 0;
}

void JazzChorusAmpAudioProcessor::setCurrentProgram(int index)
{
    juce::ignoreUnused(index);
}

const juce::String JazzChorusAmpAudioProcessor::getProgramName(int index)
{
    juce::ignoreUnused(index);
    return {};
}

void JazzChorusAmpAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
    juce::ignoreUnused(index, newName);
}

void JazzChorusAmpAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Prepare DSP engine
    jazzChorusDSP.prepare(sampleRate, samplesPerBlock, getTotalNumInputChannels());
}

void JazzChorusAmpAudioProcessor::releaseResources()
{
    jazzChorusDSP.reset();
}

bool JazzChorusAmpAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
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

void JazzChorusAmpAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                               juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(midiMessages);

    juce::ScopedNoDenormals noDenormals;

    // Update DSP parameters from plugin parameters
    jazzChorusDSP.setVolume(*volumeParam);
    jazzChorusDSP.setBass(*bassParam);
    jazzChorusDSP.setMiddle(*middleParam);
    jazzChorusDSP.setTreble(*trebleParam);
    jazzChorusDSP.setBright(*brightParam);
    jazzChorusDSP.setChorusRate(*chorusRateParam);
    jazzChorusDSP.setChorusDepth(*chorusDepthParam);
    jazzChorusDSP.setChorusMix(*chorusMixParam);
    jazzChorusDSP.setChorusEnabled(*chorusEnabledParam);

    // Process audio through Jazz Chorus simulator
    jazzChorusDSP.processBlock(buffer);
}

bool JazzChorusAmpAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* JazzChorusAmpAudioProcessor::createEditor()
{
    return new JazzChorusAmpAudioProcessorEditor(*this);
}

void JazzChorusAmpAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // Save parameter states for DAW project persistence
    juce::MemoryOutputStream stream(destData, true);

    stream.writeFloat(*volumeParam);
    stream.writeFloat(*bassParam);
    stream.writeFloat(*middleParam);
    stream.writeFloat(*trebleParam);
    stream.writeFloat(*brightParam);
    stream.writeFloat(*chorusRateParam);
    stream.writeFloat(*chorusDepthParam);
    stream.writeFloat(*chorusMixParam);
    stream.writeBool(*chorusEnabledParam);
}

void JazzChorusAmpAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // Restore parameter states when loading DAW project
    juce::MemoryInputStream stream(data, static_cast<size_t>(sizeInBytes), false);

    volumeParam->setValueNotifyingHost(stream.readFloat());
    bassParam->setValueNotifyingHost(stream.readFloat());
    middleParam->setValueNotifyingHost(stream.readFloat());
    trebleParam->setValueNotifyingHost(stream.readFloat());
    brightParam->setValueNotifyingHost(stream.readFloat());
    chorusRateParam->setValueNotifyingHost(stream.readFloat());
    chorusDepthParam->setValueNotifyingHost(stream.readFloat());
    chorusMixParam->setValueNotifyingHost(stream.readFloat());
    chorusEnabledParam->setValueNotifyingHost(stream.readBool());
}

// This creates new instances of the plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new JazzChorusAmpAudioProcessor();
}
