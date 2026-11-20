#include "PluginEditor.h"
#include "DiagnosticLog.h"

namespace
{
const juce::Colour caseColour(0xff252724);
const juce::Colour panelColour(0xffaaa99d);
const juce::Colour inkColour(0xff252923);
const juce::Colour green(0xffa8f0a0);
}

PCM70LegacyAudioProcessorEditor::PCM70LegacyAudioProcessorEditor(PCM70LegacyAudioProcessor& p)
    : juce::AudioProcessorEditor(&p), processor(p)
{
    PCM70Diagnostics::log("PROGRAM SELECT GUI 0.4.8: constructor entered");

    // The program selector is the only interactive control in this test stage.
    programSelector.addItem("CONCERT HALL", 1);
    programSelector.addItem("RICH PLATE", 2);
    programSelector.addItem("WARM ROOM", 3);
    programSelector.addItem("AMBIENCE", 4);
    programSelector.setSelectedItemIndex(
        juce::roundToInt(processor.parameters.getRawParameterValue("program")->load()),
        juce::dontSendNotification);
    programSelector.setJustificationType(juce::Justification::centredLeft);
    programSelector.setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xff101810));
    programSelector.setColour(juce::ComboBox::textColourId, green);
    programSelector.setColour(juce::ComboBox::outlineColourId, juce::Colour(0xff566b52));
    programSelector.setColour(juce::ComboBox::arrowColourId, green);
    programSelector.setTooltip("Select reverb program");
    addAndMakeVisible(programSelector);
    programAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        processor.parameters, "program", programSelector);

    setSize(760, 470);
    programSelector.setBounds(60, 143, 440, 34);
    PCM70Diagnostics::log("PROGRAM SELECT GUI 0.4.8: setSize and selector setup returned");
    PCM70Diagnostics::log("PROGRAM SELECT GUI 0.4.8: editor ready");
}

void PCM70LegacyAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff111310));
    auto outer = getLocalBounds().toFloat().reduced(8.0f);
    g.setColour(caseColour);
    g.fillRoundedRectangle(outer, 9.0f);
    g.setColour(juce::Colour(0xff5b6057));
    g.drawRoundedRectangle(outer, 9.0f, 2.0f);

    auto face = juce::Rectangle<float>(20.0f, 20.0f, 720.0f, 430.0f);
    g.setColour(panelColour);
    g.fillRoundedRectangle(face, 5.0f);
    g.setColour(juce::Colour(0xffd1d0c4));
    g.drawRoundedRectangle(face.reduced(2.0f), 4.0f, 1.0f);

    g.setColour(inkColour);
    g.setFont(juce::Font(22.0f, juce::Font::bold));
    g.drawText("LEXICON", 38, 30, 180, 26, juce::Justification::centredLeft);
    g.setFont(juce::Font(12.0f, juce::Font::bold));
    g.drawText("PCM 70  DIGITAL EFFECTS PROCESSOR", 38, 57, 360, 18, juce::Justification::centredLeft);
    g.drawText("LEGACY", 642, 35, 76, 20, juce::Justification::centredRight);

    g.setColour(juce::Colour(0xff101810));
    g.fillRoundedRectangle(juce::Rectangle<float>(45, 100, 475, 174), 4.0f);
    g.setColour(juce::Colour(0xff566b52));
    g.drawRoundedRectangle(juce::Rectangle<float>(45, 100, 475, 174), 4.0f, 2.0f);
    g.setColour(green.withAlpha(0.75f));
    g.setFont(juce::Font("Consolas", 11.0f, juce::Font::bold));
    g.drawText("PROGRAM / EDIT", 62, 112, 200, 18, juce::Justification::left);
    g.setFont(juce::Font("Consolas", 11.0f, juce::Font::bold));
    g.drawText("PARAMETER", 62, 207, 110, 18, juce::Justification::left);
    g.setFont(juce::Font("Consolas", 15.0f, juce::Font::bold));
    g.drawText("DECAY       4.8 SEC", 62, 230, 420, 24, juce::Justification::left);

    g.setColour(inkColour);
    g.setFont(juce::Font(10.0f, juce::Font::bold));
    g.drawText("PARAMETER SELECT", 45, 294, 475, 18, juce::Justification::centredLeft);
    g.drawText("SOFT KNOB", 565, 294, 145, 18, juce::Justification::centred);
    g.setColour(juce::Colour(0xff77796e));
    g.drawHorizontalLine(283, 38.0f, 722.0f);

    const int x0 = 52, y = 328, w = 42, h = 38, gap = 7;
    for (int i = 0; i < 10; ++i)
    {
        const auto x = x0 + i * (w + gap);
        g.setColour(juce::Colour(0xff30342f));
        g.fillRoundedRectangle((float)x, (float)y, (float)w, (float)h, 3.0f);
        g.setColour(juce::Colour(0xff666b60));
        g.drawRoundedRectangle((float)x, (float)y, (float)w, (float)h, 3.0f, 1.0f);
        g.setColour(juce::Colour(0xffe0dfd2));
        g.setFont(juce::Font(16.0f, juce::Font::bold));
        g.drawText(juce::String(i), x, y, w, h, juce::Justification::centred);
    }

    g.setColour(juce::Colour(0xff51564e));
    g.fillEllipse(590.0f, 135.0f, 88.0f, 88.0f);
    g.setColour(juce::Colour(0xffbdc1b3));
    g.fillEllipse(596.0f, 141.0f, 76.0f, 76.0f);
    g.setColour(juce::Colour(0xff242822));
    g.fillEllipse(630.0f, 146.0f, 8.0f, 34.0f);

    g.setColour(juce::Colour(0xff66685f));
    for (auto point : { juce::Point<float>(29, 29), juce::Point<float>(731, 29),
                        juce::Point<float>(29, 441), juce::Point<float>(731, 441) })
    {
        g.fillEllipse(point.x - 3.0f, point.y - 3.0f, 6.0f, 6.0f);
        g.setColour(juce::Colour(0xffc5c4b8));
        g.drawLine(point.x - 2.0f, point.y, point.x + 2.0f, point.y, 1.0f);
        g.setColour(juce::Colour(0xff66685f));
    }
}

void PCM70LegacyAudioProcessorEditor::resized()
{
}
