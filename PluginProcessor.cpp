#include "PluginProcessor.h"
#include "PluginEditor.h"

MyasoAudioProcessor::MyasoAudioProcessor()
 : AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true)),
   parameters(*this, nullptr, "MYASO_STATE", createLayout())
{
    for (int i = 0; i < 12; ++i) synth.addVoice(new MyasoVoice());
    synth.addSound(new MyasoSound());
}
juce::AudioProcessorValueTreeState::ParameterLayout MyasoAudioProcessor::createLayout()
{
    using F = juce::AudioParameterFloat;
    using C = juce::AudioParameterChoice;
    juce::AudioProcessorValueTreeState::ParameterLayout p;
    p.add(std::make_unique<C>("wave", "Форма блевоты",
        juce::StringArray{"Каша", "Прямоугольник", "Пила", "Синус"}, 0));
    p.add(std::make_unique<F>("cutoff", "Гул в кишках", juce::NormalisableRange<float>(80, 16000, 1, 0.35f), 8000));
    p.add(std::make_unique<F>("attack", "Подступает", juce::NormalisableRange<float>(0.001f, 3.0f, 0.001f, 0.4f), 0.01f));
    p.add(std::make_unique<F>("decay", "Тошнит", juce::NormalisableRange<float>(0.01f, 3.0f, 0.001f, 0.4f), 0.18f));
    p.add(std::make_unique<F>("sustain", "Остатки", juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.75f));
    p.add(std::make_unique<F>("release", "Отпускает", juce::NormalisableRange<float>(0.01f, 5.0f, 0.001f, 0.4f), 0.35f));
    p.add(std::make_unique<F>("level", "Громкость вони", juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.8f));
    return p;
}
void MyasoAudioProcessor::prepareToPlay(double sr, int block)
{
    synth.setCurrentPlaybackSampleRate(sr);
    juce::ignoreUnused(block);
}
bool MyasoAudioProcessor::isBusesLayoutSupported(const BusesLayout& l) const
{
    return l.getMainOutputChannelSet() == juce::AudioChannelSet::mono()
        || l.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}
void MyasoAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();
    const auto wave = std::round(parameters.getRawParameterValue("wave")->load() * 3.0f);
    const auto cutoff = parameters.getRawParameterValue("cutoff")->load();
    const auto attack = parameters.getRawParameterValue("attack")->load();
    const auto decay = parameters.getRawParameterValue("decay")->load();
    const auto sustain = parameters.getRawParameterValue("sustain")->load();
    const auto release = parameters.getRawParameterValue("release")->load();
    const auto level = parameters.getRawParameterValue("level")->load();
    for (int i = 0; i < synth.getNumVoices(); ++i)
        if (auto* v = dynamic_cast<MyasoVoice*>(synth.getVoice(i)))
            v->setParams(wave, cutoff, attack, decay, sustain, release, level);
    synth.renderNextBlock(buffer, midi, 0, buffer.getNumSamples());
}
juce::AudioProcessorEditor* MyasoAudioProcessor::createEditor() { return new MyasoAudioProcessorEditor(*this); }
void MyasoAudioProcessor::getStateInformation(juce::MemoryBlock& dest)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, dest);
}
void MyasoAudioProcessor::setStateInformation(const void* data, int size)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, size));
    if (xml && xml->hasTagName(parameters.state.getType()))
        parameters.replaceState(juce::ValueTree::fromXml(*xml));
}
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new MyasoAudioProcessor(); }
