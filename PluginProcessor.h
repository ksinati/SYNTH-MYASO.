#pragma once
#include <JuceHeader.h>

class MyasoSound : public juce::SynthesiserSound
{
public:
    bool appliesToNote(int) override { return true; }
    bool appliesToChannel(int) override { return true; }
};

class MyasoVoice : public juce::SynthesiserVoice
{
public:
    bool canPlaySound(juce::SynthesiserSound* s) override { return dynamic_cast<juce::SynthesiserSound*>(s) != nullptr; }
    void setParams(float wave, float cutoff, float attack, float decay, float sustain, float release, float level)
    {
        waveform = (int) wave; filterCutoff = cutoff; ampParams.attack = attack;
        ampParams.decay = decay; ampParams.sustain = sustain; ampParams.release = release;
        outputLevel = level;
    }
    void startNote(int midi, float velocity, juce::SynthesiserSound*, int) override
    {
        frequency = juce::MidiMessage::getMidiNoteInHertz(midi);
        phase = 0.0; phaseDelta = frequency / getSampleRate();
        amp.setSampleRate(getSampleRate());
        amp.setParameters(ampParams);
        amp.noteOn();
        gain = velocity;
        filterState = 0.0f;
    }
    void stopNote(float, bool allowTailOff) override
    {
        amp.noteOff();
        if (!allowTailOff) clearCurrentNote();
    }
    void pitchWheelMoved(int) override {}
    void controllerMoved(int, int) override {}
    void renderNextBlock(juce::AudioBuffer<float>& out, int start, int count) override
    {
        if (!isVoiceActive()) return;
        const float sr = (float)getSampleRate();
        const float a = 1.0f - std::exp(-juce::MathConstants<float>::twoPi * juce::jlimit(30.0f, 18000.0f, filterCutoff) / sr);
        for (int i = 0; i < count; ++i)
        {
            float x = 0.0f;
            switch (waveform)
            {
                case 1: x = phase < 0.5 ? 1.0f : -1.0f; break;
                case 2: x = 2.0f * (float)phase - 1.0f; break;
                case 3: x = (float)std::sin(phase * juce::MathConstants<double>::twoPi); break;
                default: x = (float)std::sin(phase * juce::MathConstants<double>::twoPi)
                              + 0.25f * (float)std::sin(phase * juce::MathConstants<double>::twoPi * 2.0); break;
            }
            phase += phaseDelta;
            if (phase >= 1.0) phase -= 1.0;
            const float env = amp.getNextSample();
            filterState += a * (x - filterState);
            const float y = filterState * env * gain * outputLevel * 0.18f;
            for (int ch = 0; ch < out.getNumChannels(); ++ch)
                out.addSample(ch, start + i, y);
        }
        if (!amp.isActive()) clearCurrentNote();
    }
private:
    juce::ADSR amp;
    juce::ADSR::Parameters ampParams { 0.01f, 0.18f, 0.75f, 0.35f };
    int waveform = 0;
    double phase = 0.0, phaseDelta = 0.0, frequency = 440.0;
    float filterCutoff = 8000.0f, filterState = 0.0f, gain = 0.0f, outputLevel = 0.8f;
};

class MyasoAudioProcessor : public juce::AudioProcessor
{
public:
    MyasoAudioProcessor();
    ~MyasoAudioProcessor() override = default;
    void prepareToPlay(double, int) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return "SYNTH.MYASO"; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 2.0; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return "Default"; }
    void changeProgramName(int, const juce::String&) override {}
    void getStateInformation(juce::MemoryBlock&) override;
    void setStateInformation(const void*, int) override;
    juce::AudioProcessorValueTreeState parameters;
private:
    juce::Synthesiser synth;
    static juce::AudioProcessorValueTreeState::ParameterLayout createLayout();
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MyasoAudioProcessor)
};
