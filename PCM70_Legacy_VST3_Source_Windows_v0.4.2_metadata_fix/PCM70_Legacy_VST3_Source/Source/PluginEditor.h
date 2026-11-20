#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

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
    juce::Slider softKnob;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> activeKnobAttachment;
    juce::TextButton decaySelectButton;
    juce::TextButton sizeSelectButton;
    juce::String selectedParameterName { "DECAY" };
    juce::String selectedParameterUnit { "SEC" };
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PCM70LegacyAudioProcessorEditor)
};
