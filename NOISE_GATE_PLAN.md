# Smart Noise Gate - Planning Document

## Overview

A noise gate for high-gain guitar that teaches users about dynamics processing while providing professional results.

**Target use cases:**
- Cleaning up noisy guitar recordings (hum, buzz)
- Tightening palm mutes for metal/hardcore
- Removing amp noise between playing
- Learning how gates work

---

## What IS a Noise Gate?

**Simple explanation:** A gate is like an automatic mute button - when the signal is quiet (below threshold), it mutes the output. When loud (above threshold), it lets sound through.

**Why needed for guitar:**
- High-gain amps amplify everything (including noise)
- Single-coil pickups can be noisy
- Want silence between notes/riffs
- Palm mutes should be tight and defined

**Real-world analogy:** Like a motion-sensor light - turns on when it detects movement (sound), turns off when no movement (silence).

---

## Core Parameters

### 1. **Threshold** (Most Important!)
- **What it is:** The level where the gate opens/closes
- **Range:** -60dB to 0dB
- **Default:** -40dB (good starting point for guitar)
- **What it controls:** How loud the signal must be to pass through

**Educational tip:** "Threshold = the minimum volume needed to open the gate. Too low = doesn't cut noise. Too high = cuts wanted notes."

### 2. **Attack**
- **What it is:** How quickly the gate opens when signal exceeds threshold
- **Range:** 0.1ms to 50ms
- **Default:** 0.5ms (very fast for guitar transients)
- **What it controls:** Preserves note attacks

**Educational tip:** "Attack = how fast the gate opens. Fast (0.5ms) preserves sharp pick attacks. Slow (10ms+) can soften transients."

### 3. **Release**
- **What it is:** How quickly the gate closes when signal falls below threshold
- **Range:** 10ms to 2000ms
- **Default:** 100ms (natural decay)
- **What it controls:** How notes fade out

**Educational tip:** "Release = how fast the gate closes. Too fast = unnatural cutoff. Too slow = lets noise through."

### 4. **Hold**
- **What it is:** How long the gate stays open after signal drops below threshold
- **Range:** 0ms to 500ms
- **Default:** 50ms
- **What it controls:** Keeps gate open briefly to avoid chattering

**Educational tip:** "Hold = time gate stays open after signal drops. Prevents rapid opening/closing on borderline signals."

### 5. **Hysteresis** (Advanced but Important!)
- **What it is:** Different thresholds for opening vs closing
- **Range:** 0dB to 12dB
- **Default:** 3dB
- **What it controls:** Prevents "chattering" (rapid open/close)

**How it works:**
- Opens at threshold (e.g., -40dB)
- Closes at threshold minus hysteresis (e.g., -43dB)
- Creates a "dead zone" that prevents oscillation

**Educational tip:** "Hysteresis = gap between open and close thresholds. Prevents gate from flickering on/off rapidly."

---

## Visual Feedback (Educational!)

### Essential Visualizations:

1. **Gain Reduction Meter**
   - Shows how much signal is being attenuated
   - Range: 0dB (no reduction) to -∞ (fully closed)
   - Updates in real-time
   - Color-coded: Green (open), Red (closed)

2. **Threshold Indicator on Input Meter**
   - Input level meter with threshold line
   - Shows where signal is relative to threshold
   - Helps users set threshold visually

3. **Gate State Indicator**
   - Simple "OPEN" / "CLOSED" display
   - Or LED-style indicator
   - Immediate visual feedback

4. **Envelope Follower Visualization (Optional)**
   - Shows detected envelope vs threshold
   - Helps understand attack/release behavior
   - Advanced feature

---

## DSP Concepts to Learn

### 1. **Envelope Detection**
Two approaches:

**Peak Detection (Simpler):**
```cpp
// Track peak level
peakLevel = max(abs(sample), peakLevel * decay);
```

**RMS Detection (More Musical):**
```cpp
// Root Mean Square - average energy
rms = sqrt(mean(sample * sample));
```

**When to use which:**
- Peak: Fast transients, drums, percussive (good for guitar!)
- RMS: Vocals, sustained sounds, more "musical"

### 2. **Smoothing / Ballistics**

Attack and release aren't instant - they're curves:

```cpp
// Exponential smoothing
if (targetGain > currentGain) {
    // Attack (opening)
    currentGain += (targetGain - currentGain) * attackCoefficient;
} else {
    // Release (closing)
    currentGain += (targetGain - currentGain) * releaseCoefficient;
}
```

Calculate coefficients from time constants:
```cpp
attackCoeff = 1.0 - exp(-1.0 / (attackTime * sampleRate));
releaseCoeff = 1.0 - exp(-1.0 / (releaseTime * sampleRate));
```

### 3. **Hysteresis State Machine**

```
State: CLOSED
  if (level > threshold) → OPEN

State: OPEN
  if (level < threshold - hysteresis) → start hold timer

State: HOLD
  if (hold time expired) → RELEASE
  if (level > threshold) → back to OPEN

State: RELEASE
  Apply release curve → CLOSED
```

---

## GUI Design (Sketch)

```
┌─────────────────────────────────────────┐
│    Smart Noise Gate                     │
├─────────────────────────────────────────┤
│                                         │
│  Input [========|====] Threshold        │
│        ↑ meter  ↑ line                  │
│                                         │
│  Gain Reduction [||||........]          │
│                                         │
│  ┌──────────┐  ┌──────────┐           │
│  │THRESHOLD │  │  ATTACK  │           │
│  │  -40 dB  │  │  0.5 ms  │           │
│  └──────────┘  └──────────┘           │
│                                         │
│  ┌──────────┐  ┌──────────┐           │
│  │ RELEASE  │  │   HOLD   │           │
│  │  100 ms  │  │   50 ms  │           │
│  └──────────┘  └──────────┘           │
│                                         │
│  Advanced: [v]                          │
│  ┌──────────┐                          │
│  │HYSTERESIS│                          │
│  │   3 dB   │                          │
│  └──────────┘                          │
│                                         │
│  Gate: [OPEN] ← status indicator       │
└─────────────────────────────────────────┘
```

**Design notes:**
- Vertical layout (fits better in DAW)
- Visual metering prominent
- Advanced features collapsible
- Clear parameter labels with units
- Real-time status feedback

---

## Smart Features

### 1. **Auto-Threshold Detection**
```
[Learn] button - Analyze 3-5 seconds of audio
1. Measure noise floor (quietest parts)
2. Measure signal peaks (loudest parts)
3. Set threshold halfway between (in dB)
4. Set hysteresis based on noise variation
```

### 2. **Context-Aware Tooltips**
- Threshold too low: "Threshold is below noise floor - gate won't close"
- Threshold too high: "Threshold is cutting wanted notes - try lowering"
- Attack too slow: "Slow attack may soften pick attacks"
- Release too fast: "Fast release may cut sustain unnaturally"

### 3. **Preset System with Explanations**
- **"Metal Palm Mutes"** - Fast attack (0.3ms), medium release (80ms), tight threshold
  - *"Preserves sharp attacks, quick release for tight rhythm"*
- **"Clean Guitar"** - Medium attack (2ms), slow release (200ms), gentle threshold
  - *"Natural feel, lets notes ring out"*
- **"Noise Cleanup"** - Fast attack (0.5ms), fast release (50ms), aggressive threshold
  - *"Aggressively cuts noise between playing"*

### 4. **Visual Problem Detection**
- If gate is chattering (opening/closing rapidly): "Increase hysteresis or hold time"
- If nothing is getting through: "Threshold may be too high"
- If noise persists: "Threshold may be too low"

---

## Implementation Steps

### Step 1: Basic Structure (1-2 hours)
- [ ] Create project folder `smart_noise_gate`
- [ ] Copy CMakeLists.txt from gain plugin, modify names
- [ ] Create basic header files (Processor.h, Editor.h)
- [ ] Add parameters (threshold, attack, release, hold, hysteresis)
- [ ] Build and verify it compiles

### Step 2: Core DSP (3-4 hours)
- [ ] Implement envelope detection (start with peak)
- [ ] Implement threshold comparison
- [ ] Implement basic open/close logic
- [ ] Test with audio - gate should open/close

### Step 3: Attack/Release Smoothing (2-3 hours)
- [ ] Calculate attack/release coefficients
- [ ] Implement exponential smoothing
- [ ] Test that gate opens/closes smoothly

### Step 4: Hold and Hysteresis (2-3 hours)
- [ ] Add hold timer
- [ ] Implement hysteresis state machine
- [ ] Test prevents chattering

### Step 5: GUI with Metering (3-4 hours)
- [ ] Create sliders for all parameters
- [ ] Add input level meter
- [ ] Add gain reduction meter
- [ ] Add gate state indicator
- [ ] Position everything

### Step 6: Educational Features (2-3 hours)
- [ ] Add tooltips to all parameters
- [ ] Implement context-aware tips
- [ ] Add presets with explanations
- [ ] Test usability

---

## Research Checklist

Before starting implementation, study these:

### **Try These Gates (Free):**
- [ ] TDR Nova (multiband, but has gate mode)
- [ ] Sonic Anomaly's Transpire
- [ ] Cockos ReaGate (comes with Reaper)
- [ ] MeldaProduction MAutoVolume (free)

**What to notice:**
- How do they visualize gate action?
- What parameters do they expose?
- How do attack/release times feel?
- What makes them easy or hard to use?

### **Watch/Read:**
- [ ] "How gates work" tutorial videos
- [ ] Attack/release explained for gates
- [ ] Hysteresis explanation
- [ ] Common gating mistakes

### **Test with Your Own Guitar:**
- [ ] Record clean guitar (low noise)
- [ ] Record high-gain guitar (lots of noise)
- [ ] Record palm mutes
- [ ] Use these as test audio

---

## Success Criteria

Plugin is successful when:

1. **Functionally works:**
   - [ ] Effectively removes noise between notes
   - [ ] Preserves note attacks (no softening)
   - [ ] Smooth open/close (no clicking)
   - [ ] Doesn't chatter on borderline signals

2. **Educationally effective:**
   - [ ] User can understand what each parameter does
   - [ ] Visual feedback shows what's happening
   - [ ] Tooltips provide helpful context
   - [ ] Presets demonstrate different use cases

3. **Professionally usable:**
   - [ ] Sounds natural, not obviously gated
   - [ ] Performs well with real guitar signals
   - [ ] Low CPU usage
   - [ ] Stable, no crashes or artifacts

4. **Learning achieved:**
   - [ ] You understand envelope detection
   - [ ] You can implement dynamics processing
   - [ ] You grasp state machines for audio
   - [ ] You can create informative GUI feedback

---

## Common Pitfalls to Avoid

1. **Clicking on gate open/close** - Use proper attack/release smoothing
2. **Chattering** - Implement hysteresis and hold time
3. **Cutting wanted transients** - Fast attack is crucial for guitar
4. **Unnatural decay** - Release time needs to match playing style
5. **Too many parameters** - Keep it simple, hide advanced features

---

## Next Steps

1. **Research phase** (1 hour):
   - Install and try 2-3 free gates
   - Take notes on what works well
   - Identify what you'd do differently

2. **Design phase** (30 mins):
   - Sketch GUI on paper
   - Decide parameter ranges
   - Plan visual feedback

3. **Implementation** (12-16 hours total):
   - Follow implementation steps above
   - Test frequently with real guitar
   - Iterate based on what sounds good

**Ready to start researching gates?**
