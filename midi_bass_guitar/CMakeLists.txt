# Minimum CMake version required
cmake_minimum_required(VERSION 3.15)

# MIDI Bass Guitar Virtual Instrument Project
project(MIDIBassGuitar VERSION 1.0.0)

# Tell CMake where to find JUCE (adjust path if needed)
add_subdirectory(../JUCE JUCE)

# Create the plugin target
juce_add_plugin(MIDIBassGuitar
    # Plugin identification - unique codes
    COMPANY_NAME "WillParker"
    PLUGIN_MANUFACTURER_CODE WPSP
    PLUGIN_CODE Mbgp                           # Unique 4-char code for this plugin

    # Plugin formats to build
    FORMATS VST3 Standalone

    # Plugin metadata
    PRODUCT_NAME "MIDI Bass Guitar"

    # Plugin type - THIS IS A SYNTH/INSTRUMENT!
    IS_SYNTH TRUE                              # Virtual instrument (not an effect)
    NEEDS_MIDI_INPUT TRUE                      # Requires MIDI input
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE

    # Copy plugin to standard locations after building
    COPY_PLUGIN_AFTER_BUILD TRUE
)

# Add source files
target_sources(MIDIBassGuitar
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginProcessor.h
        Source/PluginEditor.cpp
        Source/PluginEditor.h
        Source/BassSamplerEngine.cpp
        Source/BassSamplerEngine.h
        Source/BassVoice.cpp
        Source/BassVoice.h
        Source/BassEffects.cpp
        Source/BassEffects.h
)

# Link JUCE modules
target_link_libraries(MIDIBassGuitar
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp                          # For effects and filters
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

# Compiler definitions
target_compile_definitions(MIDIBassGuitar
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)
