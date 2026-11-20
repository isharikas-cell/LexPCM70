#include "PluginEditor.h"
#include "DiagnosticLog.h"

PCM70LegacyAudioProcessorEditor::PCM70LegacyAudioProcessorEditor(PCM70LegacyAudioProcessor& p)
    : juce::AudioProcessorEditor(&p), processor(p)
{
    PCM70Diagnostics::log("MINIMAL GUI: constructor entered");
    setSize(640, 360);
    PCM70Diagnostics::log("MINIMAL GUI: setSize returned");
}

void PCM70LegacyAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff20231f));
    g.setColour(juce::Colour(0xffaaa99d));
    g.fillRoundedRectangle(getLocalBounds().toFloat().reduced(12.0f), 8.0f);
    g.setColour(juce::Colour(0xff252923));
    g.setFont(juce::Font(25.0f, juce::Font::bold));
    g.drawText("PCM70 LEGACY — MINIMAL GUI TEST", 24, 35, getWidth() - 48, 42,
               juce::Justification::centred);
    g.setColour(juce::Colour(0xff8b2118));
    g.setFont(juce::Font(18.0f, juce::Font::bold));
    g.drawText("MINIMAL EDITOR 0.4.6", 24, 100, getWidth() - 48, 34,
               juce::Justification::centred);
    g.setColour(juce::Colour(0xff252923));
    g.setFont(juce::Font(14.0f));
    g.drawText("No controls, attachments, or parameter listeners are created.",
               24, 155, getWidth() - 48, 28, juce::Justification::centred);
}

void PCM70LegacyAudioProcessorEditor::resized()
{
}
