  # Minimum CMake version required
  cmake_minimum_required(VERSION 3.15)

  # Your project name - can be anything you want
  project(SimpleGainPlugin VERSION 0.0.1)

  # Tell CMake where to find JUCE (adjust the path to match your setup)
  # This assumes JUCE is in a folder next to simple_gain_plugin
  add_subdirectory(../JUCE JUCE)

  # This is the main command that creates your plugin
  juce_add_plugin(SimpleGainPlugin
      # Plugin identification - these MUST be unique if you make multiple plugins
      COMPANY_NAME "WillParker"                    # Your name or company
      PLUGIN_MANUFACTURER_CODE WPSP              # 4-character code (must start with uppercase)
      PLUGIN_CODE Sgpn                           # 4-character plugin ID (unique to this plugin)

      # Plugin formats to build
      FORMATS VST3 Standalone                    # VST3 and standalone app

      # Plugin metadata
      PRODUCT_NAME "Simple Gain"                 # Display name users see

      # Plugin type
      IS_SYNTH FALSE                             # We're making an effect, not a synth
      NEEDS_MIDI_INPUT FALSE                     # Gain doesn't need MIDI
      NEEDS_MIDI_OUTPUT FALSE
      IS_MIDI_EFFECT FALSE

      # Copy the plugin to standard locations after building
      COPY_PLUGIN_AFTER_BUILD TRUE               # Useful for testing
  )

  # Add your source files here (we'll create these next)
  target_sources(SimpleGainPlugin
      PRIVATE
          Source/PluginProcessor.cpp
          Source/PluginProcessor.h
          Source/PluginEditor.cpp
          Source/PluginEditor.h
  )

  # Link JUCE modules your plugin needs
  target_link_libraries(SimpleGainPlugin
      PRIVATE
          juce::juce_audio_utils              # Core audio utilities
      PUBLIC
          juce::juce_recommended_config_flags  # Recommended compiler settings
          juce::juce_recommended_lto_flags     # Link-time optimization
          juce::juce_recommended_warning_flags # Good compiler warnings
  )