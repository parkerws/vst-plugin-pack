# Minimum CMake version required
cmake_minimum_required(VERSION 3.15)

# Jazz Chorus Amp Simulator Project
project(JazzChorusAmp VERSION 1.0.0)

# Tell CMake where to find JUCE (adjust path if needed)
add_subdirectory(../JUCE JUCE)

# Create the plugin target
juce_add_plugin(JazzChorusAmp
    # Plugin identification - unique codes
    COMPANY_NAME "WillParker"
    PLUGIN_MANUFACTURER_CODE WPSP
    PLUGIN_CODE Jzca                           # Unique 4-char code for this plugin

    # Plugin formats to build
    FORMATS VST3 Standalone

    # Plugin metadata
    PRODUCT_NAME "Jazz Chorus Amp"

    # Plugin type
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE

    # Copy plugin to standard locations after building
    COPY_PLUGIN_AFTER_BUILD TRUE
)

# Add source files
target_sources(JazzChorusAmp
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginProcessor.h
        Source/PluginEditor.cpp
        Source/PluginEditor.h
        Source/JazzChorusDSP.cpp
        Source/JazzChorusDSP.h
        Source/ChorusEffect.cpp
        Source/ChorusEffect.h
        Source/CustomKnob.cpp
        Source/CustomKnob.h
)

# Link JUCE modules
target_link_libraries(JazzChorusAmp
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp                          # For advanced DSP features
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

# Compiler definitions
target_compile_definitions(JazzChorusAmp
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)
