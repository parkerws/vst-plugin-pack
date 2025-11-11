# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is an audio plugin development workspace using the JUCE framework focused on creating an **affordable, educational suite of guitar production tools** for metal, hardcore, shoegaze, emo, pop punk, and adjacent genres.

**Vision:** Build high-quality, accessible plugins that help musicians understand their tools while providing professional results - an alternative to expensive plugin packs.

**Approach:**
- Learn by building real, useful tools
- Educational features integrated naturally (not primary focus)
- "Smart" assistance without AI buzzwords
- Start free/open source, potentially commercialize later
- Serve the community while developing expertise

**Current Project:** `simple_gain_plugin` - A basic gain/volume control plugin (VST3 and Standalone formats) ✅ COMPLETED AND WORKING

## Project Structure

```
audio_development/
├── JUCE/                      # JUCE framework (v7.x)
├── simple_gain_plugin/        # Current project
│   ├── CMakeLists.txt        # Build configuration
│   └── Source/
│       ├── PluginProcessor.h    # Audio processing (header)
│       ├── PluginProcessor.cpp  # Audio processing (implementation)
│       ├── PluginEditor.h       # GUI (header)
│       └── PluginEditor.cpp     # GUI (implementation)
└── CLAUDE.md                 # This file
```

## Build System

This project uses **CMake** (not Projucer) for modern build management.

### Build Commands

**Initial configuration (from `simple_gain_plugin` directory):**
```bash
cmake -B build
```

**Build the plugin:**
```bash
cmake --build build --config Release
```

**Build for debugging:**
```bash
cmake --build build --config Debug
```

**Clean build:**
```bash
rm -rf build
cmake -B build
cmake --build build --config Release
```

### Build Output Locations

After building, plugins are copied to standard locations:
- **VST3:** `C:\Program Files\Common Files\VST3\SimpleGainPlugin.vst3`
- **Standalone:** `build\SimpleGainPlugin_artefacts\Release\Standalone\SimpleGainPlugin.exe`

## Architecture Overview

### JUCE Plugin Architecture

JUCE plugins follow a processor/editor separation pattern:

**PluginProcessor** (the "brain"):
- Handles all audio processing in `processBlock()`
- Manages parameters (gain, drive, frequency, etc.)
- Handles state save/load for DAW projects
- Runs on the audio thread (real-time safe)

**PluginEditor** (the "face"):
- Displays the GUI
- Captures user interaction
- Updates processor parameters
- Runs on the GUI thread (separate from audio)

### Audio Processing Flow

```
Audio Input (from DAW/Interface)
    ↓
prepareToPlay() - Initialize (called once when audio starts)
    ↓
processBlock() - Process audio chunks (called thousands of times/second)
    ↓
Read parameter values (*parameter)
    ↓
Apply effect (modify buffer samples)
    ↓
Audio Output (to speakers/next plugin)
```

### Parameter System

Parameters are the bridge between GUI and audio processing:

1. **Creation:** Parameters created in processor constructor using `addParameter()`
2. **GUI Updates:** Editor modifies via `parameter->setValueNotifyingHost()`
3. **Audio Reads:** Processor reads via `*parameter` (dereference pointer)
4. **DAW Automation:** JUCE handles automatically

Parameters use normalized 0.0-1.0 internally, converted to/from display values (dB, Hz, etc.)

## Key Concepts for Audio Processing

### Digital Audio Fundamentals

- Audio samples are floats from -1.0 to +1.0 representing speaker position
- Sample rate (e.g., 48kHz) = 48,000 samples per second
- Buffer size (e.g., 512 samples) = chunk size processed at once
- processBlock() called ~93 times/second at 48kHz with 512 buffer

### Gain/Volume

Gain is simple multiplication:
```cpp
outputSample = inputSample * gainLinear;
```

- Decibels (logarithmic) must convert to linear for multiplication
- 0dB = 1.0 (no change), -6dB ≈ 0.5 (half), +6dB ≈ 2.0 (double)
- Use `juce::Decibels::decibelsToGain()` for conversion

### Real-Time Safety

The audio thread must NEVER:
- Allocate/free memory (`new`/`delete`)
- Wait for locks or I/O
- Call slow functions

All allocation happens in `prepareToPlay()`, processing in `processBlock()` just reads/writes buffers.

## Current Implementation: Simple Gain Plugin

**What it does:** Multiplies all audio samples by a gain value (-60dB to +12dB)

**Key files:**
- `PluginProcessor.cpp:processBlock()` - Core audio processing (line 35)
- `PluginEditor.cpp:sliderValueChanged()` - GUI to parameter connection (line 54)

**Core processing logic:**
```cpp
float gainLinear = juce::Decibels::decibelsToGain(*gainParameter);
for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
{
    float* channelData = buffer.getWritePointer(channel);
    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        channelData[sample] *= gainLinear;  // The actual effect!
    }
}
```

## Development Workflow

### Testing Plugins

1. **Standalone mode:** Run the .exe directly for quick testing
2. **In DAW:** Load the VST3 in your DAW (Reaper, Ableton, FL Studio, etc.)
3. **With audio:** Feed it guitar/vocal input and adjust gain

### Debugging

- Use Debug build for better error messages
- Check JUCE forum for common issues
- `DBG()` macro for console logging (Debug builds only)

### Adding New Parameters

1. Add pointer in `PluginProcessor.h` private section
2. Create in constructor with `addParameter()`
3. Read in `processBlock()` with `*parameter`
4. Add GUI control in `PluginEditor.cpp` constructor
5. Connect in `sliderValueChanged()`

## Development Roadmap: "Accessible Guitar Production Suite"

### Phase 1: Foundation (Months 1-3) - Essential Utilities
1. ✅ **Enhanced Gain Plugin** - Learning basics, add VU meters, tooltips, parameter smoothing
2. **Smart Noise Gate** (NEXT) - Threshold, attack, release, hysteresis, visual feedback
3. **3-Band Guitar EQ** - Tailored for guitar frequencies with visual response
4. **Clip/Saturation Utility** - Simple waveshaping with waveform visualization

**Milestone:** "Essential Guitar Utilities v1.0" - Free, open source

### Phase 2: Core Effects (Months 4-6) - Tone Shaping
5. **Overdrive/Distortion** - Multiple clipping algorithms, oversampling, tone controls
6. **Simple Delay** - Time-based effects with tempo sync and feedback
7. **Basic Chorus/Modulation** - LFO implementation, thickening, stereo width

**Milestone:** "Guitar Production Toolkit v1.0" - Free with donation option

### Phase 3: Advanced Tools (Months 7-9) - Professional Quality
8. **Cabinet IR Loader** - Convolution-based cabinet simulation with IR library
9. **Parametric EQ (Advanced)** - 6-8 bands, interactive GUI, spectrum analyzer
10. **Simple Compressor** - Dynamics control with visual gain reduction

**Milestone:** "Complete Guitar Production Suite v2.0" - Pay What You Want ($0-50 suggested)

### Phase 4: Signature Tools (Months 10-12) - Genre-Specific
11. **"Shoegaze Wall" Effect** - Layered reverbs, shimmer, reverse, massive textures
12. **"Metal Tightness" Multi-Tool** - Aggressive gate, transient shaper, preset chain
13. **"Emo/Pop-Punk Character" Suite** - Genre-specific overdrive, delays, reverbs

**Milestone:** "Genre Toolkit Bundle v1.0" - Commercial release ($30-50)

### Smart Features Philosophy

**"Intelligent, Not AI-Powered"** - Focus on genuinely helpful features:

**Tier 1: Smart Assistance (No ML Required)**
- Auto-level matching for fair A/B comparisons
- Smart parameter suggestions based on audio analysis
- Context-aware tooltips that adapt to current settings
- Visual problem-frequency indicators
- "Learn" buttons that analyze and set optimal parameters

**Tier 2: Educational Integration**
- Real-time waveform/spectrum visualization showing effect of changes
- Tooltips explaining what parameters do in musical terms
- Preset explanations ("This works because...")
- Built-in audio examples (optional)
- Links to learning resources

**Tier 3: Future ML Features (12+ months)**
- Genre detection → preset suggestions
- Reference track matching
- Natural language preset search
- Personalized recommendations based on usage patterns

**Key Principle:** Smart features should guide users naturally without overwhelming or becoming the main focus. DSP quality and usability come first.

## Common Issues

**CMake can't find JUCE:**
- Verify JUCE path in CMakeLists.txt `add_subdirectory(../JUCE JUCE)` is correct
- JUCE should be in `D:\repos\audio_development\JUCE`

**Plugin doesn't appear in DAW:**
- Check if copied to system VST3 folder
- Rescan plugins in DAW
- Verify plugin built successfully (check build output)

**No sound processing:**
- Check `processBlock()` is actually modifying samples
- Verify parameter is being read correctly
- Ensure gain isn't at minimum (-60dB)

## Resources

- **JUCE Documentation:** https://docs.juce.com
- **JUCE Forum:** https://forum.juce.com
- **JUCE Tutorials:** https://juce.com/learn/tutorials
- **The Audio Programmer (YouTube):** Great JUCE tutorials

## Target Audience

**Primary users:**
- Bedroom producers and musicians (metal, hardcore, emo, shoegaze, pop punk)
- Guitar/bass players who can't afford $100+ plugin packs
- Beginners learning audio production
- Musicians who want to understand their tools, not just use them

**Value proposition:**
- Affordable or free (vs Neural DSP, Positive Grid at $100-400)
- Educational without being preachy
- Genre-specific optimization
- Community-driven development

## Notes for Claude Code

- **Developer is learning** both C++ and audio DSP - explain both code patterns and audio theory
- **Prefers guidance over automation** - explain concepts thoroughly, let them implement
- **Learn by doing** - Build real, usable projects that serve actual needs
- **Background:** Guitarist/vocalist, interested in effects for metal/hardcore/emo/shoegaze/pop-punk
- **Teaching approach:** Show complete examples with detailed explanations, then have user type code themselves
- **Long-term vision:** Build a suite of plugins that serve underserved genres while developing expertise
- **Commercial potential:** Start open source, potentially monetize later with premium features/support
