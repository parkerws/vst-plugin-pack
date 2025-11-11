#include "PluginProcessor.h"
#include "PluginEditor.h"

  //==============================================================================
  SimpleGainPluginAudioProcessor::SimpleGainPluginAudioProcessor()
       : AudioProcessor (BusesProperties()
                         .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                         .withOutput ("Output", juce::AudioChannelSet::stereo(), true))
  {
      addParameter (gainParameter = new juce::AudioParameterFloat (
          "gain",              // parameter ID
          "Gain",              // parameter name
          -60.0f,              // minimum value (dB)
          12.0f,               // maximum value (dB)
          0.0f));              // default value (dB)
  }

  SimpleGainPluginAudioProcessor::~SimpleGainPluginAudioProcessor()
  {
  }

  //==============================================================================
  void SimpleGainPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
  {
      // Use this method as the place to do any pre-playback
      // initialisation that you need..
  }

  void SimpleGainPluginAudioProcessor::releaseResources()
  {
      // When playback stops, you can use this as an opportunity to free up any
      // spare memory, etc.
  }

  void SimpleGainPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&
  midiMessages)
  {
      juce::ScopedNoDenormals noDenormals;

      // Get the gain value from the parameter and convert from dB to linear
      float gainInDecibels = *gainParameter;
      float gainLinear = juce::Decibels::decibelsToGain (gainInDecibels);

      // Apply gain to all channels
      for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
      {
          // Get pointer to the channel's data
          float* channelData = buffer.getWritePointer (channel);

          // Process each sample in this channel
          for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
          {
              channelData[sample] *= gainLinear;
          }
      }
  }

  //==============================================================================
  juce::AudioProcessorEditor* SimpleGainPluginAudioProcessor::createEditor()
  {
      return new SimpleGainPluginAudioProcessorEditor (*this);
  }

  bool SimpleGainPluginAudioProcessor::hasEditor() const
  {
      return true;
  }

  //==============================================================================
  const juce::String SimpleGainPluginAudioProcessor::getName() const
  {
      return JucePlugin_Name;
  }

  bool SimpleGainPluginAudioProcessor::acceptsMidi() const
  {
      return false;
  }

  bool SimpleGainPluginAudioProcessor::producesMidi() const
  {
      return false;
  }

  bool SimpleGainPluginAudioProcessor::isMidiEffect() const
  {
      return false;
  }

  double SimpleGainPluginAudioProcessor::getTailLengthSeconds() const
  {
      return 0.0;
  }

  //==============================================================================
  int SimpleGainPluginAudioProcessor::getNumPrograms()
  {
      return 1;   // Some hosts don't cope very well if you tell them there are 0 programs
  }

  int SimpleGainPluginAudioProcessor::getCurrentProgram()
  {
      return 0;
  }

  void SimpleGainPluginAudioProcessor::setCurrentProgram (int index)
  {
  }

  const juce::String SimpleGainPluginAudioProcessor::getProgramName (int index)
  {
      return {};
  }

  void SimpleGainPluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
  {
  }

  //==============================================================================
  void SimpleGainPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
  {
      // Save the gain parameter value
      juce::MemoryOutputStream stream (destData, true);
      stream.writeFloat (*gainParameter);
  }

  void SimpleGainPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
  {
      // Restore the gain parameter value
      juce::MemoryInputStream stream (data, static_cast<size_t> (sizeInBytes), false);
      gainParameter->setValueNotifyingHost (stream.readFloat());
  }

  //==============================================================================
  // This creates new instances of the plugin..
  juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
  {
      return new SimpleGainPluginAudioProcessor();
  }