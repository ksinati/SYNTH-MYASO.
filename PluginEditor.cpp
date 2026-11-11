#include "PluginEditor.h"

MyasoAudioProcessorEditor::MyasoAudioProcessorEditor(MyasoAudioProcessor& p)
 : AudioProcessorEditor(&p), processor(p)
{
    setSize(760, 500);
    wave.addItemList({"Каша", "Прямоугольник", "Пила", "Синус"}, 1);
    waveLabel.setText("ФОРМА БЛЕВОТЫ", juce::dontSendNotification);
    waveLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(waveLabel); addAndMakeVisible(wave);
    wave.setSelectedId(1);
    wave.onChange = [this] { processor.parameters.getParameter("wave")->setValueNotifyingHost((float)(wave.getSelectedId()-1)/3.0f); };
    setupKnob(cutoff, "ГУЛ В КИШКАХ", "cutoff");
    setupKnob(attack, "ПОДСТУПАЕТ", "attack");
    setupKnob(decay, "ТОШНИТ", "decay");
    setupKnob(sustain, "ОСТАТКИ", "sustain");
    setupKnob(release, "ОТПУСКАЕТ", "release");
    setupKnob(level, "ГРОМКОСТЬ ВОНИ", "level");
}
void MyasoAudioProcessorEditor::setupKnob(Knob& k, const juce::String& title, const juce::String& id)
{
    k.slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    k.slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 78, 20);
    k.label.setText(title, juce::dontSendNotification);
    k.label.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(k.slider); addAndMakeVisible(k.label);
    k.attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.parameters, id, k.slider);
}
void MyasoAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff100b0b));
    g.setColour(juce::Colour(0xff4a1714)); g.fillRect(0, 0, getWidth(), 105);
    g.setColour(juce::Colour(0xffc23a25));
    g.setFont(juce::Font(juce::FontOptions(39.0f, juce::Font::bold)));
    g.drawText("SYNTH.MYASO", 24, 12, 500, 52, juce::Justification::left);
    g.setFont(juce::Font(juce::FontOptions(15.0f)));
    g.drawText("ЗВУК ДЛЯ ТВОЕЙ ДЕПРЕССИИ  •  НЕ ОТКРЫВАТЬ ПОСЛЕ ПОЛУНОЧИ", 28, 66, 650, 24, juce::Justification::left);
    g.setColour(juce::Colour(0xff6e2921)); g.drawRect(12, 116, getWidth()-24, getHeight()-130, 2);
    g.setColour(juce::Colour(0xffb83a27)); g.setFont(juce::Font(juce::FontOptions(19.0f, juce::Font::bold)));
    g.drawText("ШИРИНА ОЧКА: НЕИЗВЕСТНО  |  ПИЗДА ПЛОТНОСТЬ: 100%  |  ВОНЬ: ON", 24, 440, 710, 28, juce::Justification::centred);
}
void MyasoAudioProcessorEditor::resized()
{
    waveLabel.setBounds(28, 135, 220, 25); wave.setBounds(28, 164, 220, 34);
    Knob* knobs[] = { &cutoff, &attack, &decay, &sustain, &release, &level };
    for (int i=0; i<6; ++i) {
        const int x = 25 + i*122;
        knobs[i]->label.setBounds(x, 240, 112, 26);
        knobs[i]->slider.setBounds(x, 270, 112, 130);
    }
}
