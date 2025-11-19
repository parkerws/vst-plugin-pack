# Minimum CMake version required
cmake_minimum_required(VERSION 3.15)

# Orange Amp Simulator Project
project(OrangeAmpSimulator VERSION 1.0.0)

# Tell CMake where to find JUCE (adjust path if needed)
add_subdirectory(../JUCE JUCE)

# Create the plugin target
juce_add_plugin(OrangeAmpSimulator
    # Plugin identification - unique codes
    COMPANY_NAME "WillParker"
    PLUGIN_MANUFACTURER_CODE WPSP
    PLUGIN_CODE Oras                           # Unique 4-char code for this plugin

    # Plugin formats to build
    FORMATS VST3 Standalone

    # Plugin metadata
    PRODUCT_NAME "Orange Amp Simulator"

    # Plugin type
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE

    # Copy plugin to standard locations after building
    COPY_PLUGIN_AFTER_BUILD TRUE
)

# Add source files
target_sources(OrangeAmpSimulator
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginProcessor.h
        Source/PluginEditor.cpp
        Source/PluginEditor.h
        Source/AmpSimDSP.cpp
        Source/AmpSimDSP.h
        Source/CustomKnob.cpp
        Source/CustomKnob.h
)

# Link JUCE modules
target_link_libraries(OrangeAmpSimulator
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp                          # For advanced DSP features
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

# Compiler definitions
target_compile_definitions(OrangeAmpSimulator
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)
