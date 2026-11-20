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
    juce::ComboBox program;
    juce::Label title, subtitle;
    struct Knob : juce::Component
    {
        juce::Slider slider;
        juce::Label label;
        Knob(juce::AudioProcessorValueTreeState&, const juce::String&, const juce::String&);
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;
        void resized() override;
    };
    std::array<std::unique_ptr<Knob>, 10> knobs;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> programAttachment;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PCM70LegacyAudioProcessorEditor)
};
