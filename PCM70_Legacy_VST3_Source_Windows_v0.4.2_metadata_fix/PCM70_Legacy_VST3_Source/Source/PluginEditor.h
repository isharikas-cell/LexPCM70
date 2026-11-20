#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include <array>

class PCM70LegacyAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit PCM70LegacyAudioProcessorEditor(PCM70LegacyAudioProcessor&);
    ~PCM70LegacyAudioProcessorEditor() override = default;
    void paint(juce::Graphics&) override;
    void resized() override;
private:
    PCM70LegacyAudioProcessor& processor;
    juce::ComboBox programSelector;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> programAttachment;
    static constexpr int parameterCount = 14;
    std::array<juce::Slider, parameterCount> controls;
    std::array<juce::Label, parameterCount> labels;
    std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>, parameterCount> attachments;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PCM70LegacyAudioProcessorEditor)
};
