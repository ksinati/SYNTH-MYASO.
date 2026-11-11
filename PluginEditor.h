#pragma once
#include "PluginProcessor.h"

class MyasoAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit MyasoAudioProcessorEditor(MyasoAudioProcessor&);
    void paint(juce::Graphics&) override;
    void resized() override;
private:
    MyasoAudioProcessor& processor;
    juce::ComboBox wave;
    juce::Label waveLabel;
    struct Knob {
        juce::Slider slider;
        juce::Label label;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;
    };
    Knob cutoff, attack, decay, sustain, release, level;
    void setupKnob(Knob&, const juce::String&, const juce::String&);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MyasoAudioProcessorEditor)
};
