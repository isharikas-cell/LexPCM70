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
    juce::TextButton predelaySelectButton;
    juce::TextButton diffusionSelectButton;
    juce::TextButton dampingSelectButton;
    juce::TextButton modulationSelectButton;
    juce::TextButton widthSelectButton;
    juce::TextButton mixSelectButton;
    juce::TextButton inputSelectButton;
    juce::TextButton outputSelectButton;
    juce::TextButton bankButton;
    bool secondBank = false;
    void selectParameter(const juce::String& parameterId, const juce::String& displayName, const juce::String& unit);
    juce::String selectedParameterName { "DECAY" };
    juce::String selectedParameterUnit { "SEC" };
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PCM70LegacyAudioProcessorEditor)
};
