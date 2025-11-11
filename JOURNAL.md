# Audio Plugin Development Journal

This journal tracks the learning journey and development progress for audio plugin projects.

---

## Session 1 - 2025-11-11: Simple Gain Plugin - Complete Success! 🎉

### Goals
- Set up first JUCE plugin project ✅
- Understand JUCE plugin architecture ✅
- Learn basic audio processing concepts ✅
- Build a functional gain plugin ✅
- Successfully compile and test ✅

### What We Built

**Project:** `simple_gain_plugin` - A basic gain/volume control plugin

**Files Created:**
- `CMakeLists.txt` - Build configuration
- `Source/PluginProcessor.h` - Audio processor declaration
- `Source/PluginProcessor.cpp` - Audio processor implementation
- `Source/PluginEditor.h` - GUI declaration
- `Source/PluginEditor.cpp` - GUI implementation

### Key Concepts Learned

#### C++ Concepts
- **Header vs Implementation files** - Declarations (.h) vs definitions (.cpp)
- **Class inheritance** - Inheriting from JUCE's AudioProcessor and AudioProcessorEditor
- **Pointers vs References** - `*` (pointers store addresses) vs `&` (references to actual objects)
- **Dereferencing** - Using `*pointer` to get the value at an address
- **Multiple inheritance** - Editor inherits from both AudioProcessorEditor and Slider::Listener
- **Initializer lists** - Constructor initialization before body runs
- **override keyword** - Explicitly marking inherited method implementations
- **const keyword** - Methods that don't modify state, immutable return values

#### Audio Programming Concepts
- **Digital audio representation** - Samples as floats from -1.0 to +1.0 (speaker position)
- **Sample rate** - Number of samples per second (44.1kHz, 48kHz, etc.)
- **Buffer size** - Chunk size processed at once (typically 256-512 samples)
- **Gain as multiplication** - Volume control is literally multiplying sample values
- **Decibels vs Linear** - Logarithmic (dB) vs linear scales, conversion needed for processing
- **Real-time audio processing** - processBlock() called thousands of times per second
- **Audio thread safety** - No memory allocation or slow operations in processBlock()

#### JUCE Architecture
- **Processor/Editor separation** - Audio processing separate from GUI
- **Parameter system** - Bridge between GUI and audio processing
  - Created with `addParameter()` in processor constructor
  - GUI updates via `setValueNotifyingHost()`
  - Audio reads via dereferencing `*parameter`
  - DAW automation handled automatically
- **Normalized parameters** - Internal 0.0-1.0 range converted to display values
- **AudioBuffer** - Container for multi-channel sample data
- **Component system** - GUI widgets as components (Slider, Label, etc.)
- **Listener pattern** - Components notify listeners of changes (e.g., slider movements)

### Understanding the Data Flow

**GUI → Parameter → Audio Processing:**

1. User drags slider to -6dB
2. `sliderValueChanged()` called in Editor
3. Slider value converted to normalized 0.0-1.0 range
4. Parameter updated via `setValueNotifyingHost()`
5. `processBlock()` reads parameter with `*gainParameter`
6. dB value converted to linear gain (0.5)
7. Each audio sample multiplied by 0.5
8. Result: Volume reduced by half

**The Core Processing (The Entire Effect!):**
```cpp
channelData[sample] *= gainLinear;
```
This single line of multiplication IS the gain effect. Everything else is scaffolding.

### Technical Details

#### Parameter Configuration
- **ID:** "gain" (internal identifier)
- **Name:** "Gain" (display name)
- **Range:** -60dB to +12dB
- **Default:** 0dB (unity gain)
- **Type:** AudioParameterFloat

#### GUI Layout
- Window size: 400x300 pixels
- Vertical linear slider (100x180 pixels)
- Text box below slider for numeric input
- Label: "Gain (dB)" centered above slider
- Title: "Simple Gain Plugin" at top

#### Plugin Formats
- VST3 (modern standard)
- Standalone (for testing without DAW)

### Questions Answered

**Q: Where do the implementations of header functions go?**
A: In the corresponding .cpp file. Headers declare "what exists," implementation files define "how it works."

**Q: How do we know what values to put in helper functions?**
A: We decide based on plugin needs. JUCE provides the framework, we fill in specifics. For save/load, we write what we want to preserve.

**Q: How does MemoryInputStream set parameter values?**
A: It doesn't directly - it's just a tool to read binary data. We read the value from the stream, THEN set the parameter ourselves.

**Q: Where did *gainParameter come from?**
A: Created in constructor with `new AudioParameterFloat()`, pointer stored in member variable, dereferenced with `*` to get current value.

**Q: What specifically sets "gain" in the audio sense?**
A: The multiplication `channelData[sample] *= gainLinear`. Digital audio is just numbers representing speaker position. Multiplying those numbers makes them bigger (louder) or smaller (quieter).

### Insights and "Aha" Moments

1. **Audio processing is surprisingly simple** - At its core, it's just math on arrays of numbers
2. **Pointers are essential** - They let multiple parts of code (GUI, audio, DAW automation) reference the same parameter
3. **The separation of concerns is clear** - Audio processing never touches GUI, GUI never processes audio, parameters bridge the gap
4. **JUCE handles the hard parts** - Threading, DAW communication, format differences all abstracted away
5. **Real-time constraints matter** - Can't do normal programming things (allocation, I/O) in audio callback

### Build Journey

**Challenges overcome:**
1. **CMake PATH issue** - Resolved by adding CMake to system PATH
2. **JuceHeader.h not found** - Switched to modular includes (`juce_audio_processors`, `juce_gui_basics`)
3. **Private member access error** - Moved `gainParameter` to public section for Editor access
4. **Permission error on install** - Plugin built successfully, copy to system folder requires admin

**Final result:**
- ✅ Plugin compiles successfully
- ✅ VST3 built and working in DAW
- ✅ Gain control functions correctly
- ✅ Clean, working foundation for future plugins

### Vision Crystallized

**"Accessible Guitar Production Suite"**

Target audience: Metal, hardcore, shoegaze, emo, pop-punk musicians who need affordable, educational tools.

**Long-term roadmap:**
- **Phase 1 (Months 1-3):** Essential utilities (gain, gate, EQ, saturation)
- **Phase 2 (Months 4-6):** Core effects (overdrive, delay, chorus)
- **Phase 3 (Months 7-9):** Advanced tools (IR loader, parametric EQ, compressor)
- **Phase 4 (Months 10-12):** Genre-specific signature tools

**Smart features approach:**
- Focus on "intelligent assistance" not "AI buzzwords"
- Educational features integrated naturally
- Help users find settings they want without overwhelming
- DSP quality and usability first, smart features enhance

**Business model:**
- Start free/open source (build reputation, get feedback)
- Add premium features later (smart assistance, expanded presets)
- Potential commercial release after establishing value

### Next Steps

**This Week:**
- [x] ✅ Build gain plugin successfully
- [x] ✅ Test in DAW
- [x] ✅ Crystallize vision and roadmap
- [ ] Enhance gain plugin GUI (better styling, tooltips)
- [ ] Plan noise gate (research, sketch GUI)
- [ ] Start noise gate implementation

**Next 2-3 Weeks:**
- [ ] Complete Smart Noise Gate plugin
- [ ] Learn: Envelope detection, dynamics processing
- [ ] Add educational features (visual feedback, contextual help)

**Next Project (After Noise Gate):**
**3-Band Guitar EQ** - Frequency-based processing, filter design

### Resources Used
- JUCE framework documentation
- Claude Code for guided learning

### Notes
- Focus is on understanding, not just copying code
- Learning both C++ and DSP concepts simultaneously
- Prefer explanations before implementation
- Background: Guitar/bass/vocals, interested in effects and routing

---

---

## Session 2 - 2025-11-11 (Later): Vision, Roadmap & Next Steps

### Goals
- Define long-term vision ✅
- Create comprehensive roadmap ✅
- Plan smart/educational features approach ✅
- Update documentation for future sessions ✅
- Plan noise gate plugin ⏳

### Major Decisions Made

**Target Audience Defined:**
- Metal, hardcore, shoegaze, emo, pop-punk musicians
- Bedroom producers and beginners
- People who can't afford $100+ plugin packs
- Users who want to understand tools, not just use them

**Smart Features Philosophy:**
- "Intelligent, not AI-powered"
- Educational without being preachy
- Help users discover good settings naturally
- No buzzword marketing - genuine helpfulness only

**Development Approach:**
- Build what I (the developer) would actually use
- Open source initially for feedback and reputation
- Learn by doing - real projects, real skills
- Quality over quantity - each plugin teaches new concepts

### Long-Term Vision Discussion

**Realistic Market Positioning:**
- Not trying to compete with Neural DSP on quality (yet)
- Compete on affordability, education, and genre focus
- Serve underserved niches (emo, hardcore, shoegaze specifically)
- Build community around accessible tools

**Potential Revenue Streams (Future):**
- Free tier: Core functionality
- Pro tier ($30-50): Extended features, premium presets
- AI-assisted tier ($10/month): Smart features with API costs
- Consultation/custom development

**Skills to Develop:**
- Advanced C++ and JUCE mastery
- DSP algorithms and theory
- Audio engineering techniques
- Genre-specific production knowledge
- Later: Machine learning for audio (if pursuing neural modeling)

### Resources for Learning

**Essential Books to Get:**
- "Designing Audio Effect Plugins in C++" by Will Pirkle (THE book for this path)
- "The Audio Programming Book" by Boulanger & Lazzarini
- "DAFX: Digital Audio Effects" by Udo Zölzer

**Online Resources:**
- The Audio Programmer (YouTube)
- musicdsp.org (algorithm archive)
- JUCE Forum (technical help)
- KVRAudio Forums (DSP discussions)

### Next Plugin: Smart Noise Gate

**Why this next:**
- Essential for high-gain guitar (metal, hardcore)
- Teaches dynamics processing (crucial skill)
- Introduces envelope detection
- Visual feedback opportunities (gain reduction meter)
- Immediately useful in actual production

**Features planned:**
- Threshold, Attack, Release, Hold
- Hysteresis to prevent chattering
- Visual gain reduction meter
- Sidechain input capability
- Educational tooltips explaining each parameter
- Real-time visualization of gating action

**Learning goals:**
- Envelope detection (RMS vs peak)
- Smoothing and timing curves
- Dynamic range processing fundamentals
- More complex GUI with metering

---

## Future Sessions

### Session 3 - TBD: Enhance Gain Plugin + Plan Noise Gate

Goals:
- Improve gain plugin GUI aesthetics
- Add tooltips and basic smart features
- Research noise gate parameters and implementations
- Sketch noise gate GUI
- Begin noise gate implementation

---

## Ideas and Future Experiments

**For Gain Plugin Enhancements:**
- Parameter smoothing - Prevent clicks/pops when changing gain quickly
- VU meters - Input/output level visualization
- Auto-level matching - Ensure bypassed vs processed are same volume for fair comparison
- Context-aware tooltips - "Gain at +6dB = 2x louder, may clip hot signals"

**For Future Plugins:**
- Preset system with explanations ("This preset works because...")
- Visual frequency analyzers showing effect of processing
- "Learn" buttons that analyze audio and suggest settings
- A/B comparison features
- Waveform visualization showing processing effects

**Community Engagement Ideas:**
- Share progress on KVRAudio, Reddit
- Create tutorial content explaining concepts learned
- Open source on GitHub for feedback
- Build preset packs with community
- Create demo videos for each plugin
