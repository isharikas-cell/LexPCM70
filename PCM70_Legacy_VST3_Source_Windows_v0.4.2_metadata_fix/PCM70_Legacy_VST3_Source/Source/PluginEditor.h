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
    juce::Slider softKnob;
    juce::Label valueLabel;
    juce::ComboBox program;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> programAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> parameterAttachment;
    std::array<std::unique_ptr<juce::TextButton>, 10> numberButtons;
    std::array<juce::String, 10> parameterIds {{
        "decay", "size", "predelay", "diffusion", "damping",
        "modulation", "width", "mix", "input", "output"
    }};
    std::array<juce::String, 10> parameterNames {{
        "DECAY", "SIZE", "PREDELAY", "DIFFUSION", "HF DAMP",
        "MODULATION", "WIDTH", "MIX", "INPUT", "OUTPUT"
    }};
    int selectedParameter = 0;
    void selectParameter(int index);
    void updateDisplay();
    void styleButton(juce::TextButton& button, const juce::String& text);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PCM70LegacyAudioProcessorEditor)
};
