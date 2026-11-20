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
    PCM70Diagnostics::log("LCD READOUT GUI 0.4.17: constructor entered");

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

    // Soft Knob starts on Decay; keys 0 and 1 select Decay and Size.
    softKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    softKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    softKnob.setRotaryParameters(juce::MathConstants<float>::pi * 1.2f,
                                 juce::MathConstants<float>::pi * 2.8f, true);
    softKnob.setColour(juce::Slider::rotarySliderFillColourId, green);
    softKnob.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff51564e));
    softKnob.setColour(juce::Slider::thumbColourId, juce::Colour(0xffe0dfd2));
    softKnob.setTooltip("SOFT KNOB — Decay time");
    addAndMakeVisible(softKnob);
    activeKnobAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor.parameters, "decay", softKnob);
    decaySelectButton.setButtonText("0");
    sizeSelectButton.setButtonText("1");
    predelaySelectButton.setButtonText("2");
    diffusionSelectButton.setButtonText("3");
    dampingSelectButton.setButtonText("4");
    modulationSelectButton.setButtonText("5");
    widthSelectButton.setButtonText("6");
    mixSelectButton.setButtonText("7");
    inputSelectButton.setButtonText("8");
    outputSelectButton.setButtonText("9");
    for (auto* button : { &decaySelectButton, &sizeSelectButton, &predelaySelectButton, &diffusionSelectButton, &dampingSelectButton, &modulationSelectButton, &widthSelectButton, &mixSelectButton, &inputSelectButton, &outputSelectButton })
    {
        button->setColour(juce::TextButton::buttonColourId, juce::Colour(0xff30342f));
        button->setColour(juce::TextButton::textColourOffId, juce::Colour(0xffe0dfd2));
        button->setColour(juce::TextButton::buttonOnColourId, juce::Colour(0xff4b5945));
        button->setColour(juce::TextButton::textColourOnId, green);
        addAndMakeVisible(*button);
    }
    decaySelectButton.setTooltip("Select DECAY for Soft Knob");
    sizeSelectButton.setTooltip("Select SIZE for Soft Knob");
    predelaySelectButton.setTooltip("Select PREDELAY for Soft Knob");
    diffusionSelectButton.setTooltip("Select DIFFUSION for Soft Knob");
    dampingSelectButton.setTooltip("Select DAMPING for Soft Knob");
    modulationSelectButton.setTooltip("Select MODULATION for Soft Knob");
    widthSelectButton.setTooltip("Select WIDTH for Soft Knob");
    mixSelectButton.setTooltip("Select MIX for Soft Knob");
    inputSelectButton.setTooltip("Select INPUT level for Soft Knob");
    outputSelectButton.setTooltip("Select OUTPUT level for Soft Knob");
    decaySelectButton.onClick = [this] { if (secondBank) selectParameter("modRate", "MOD RATE", "HZ"); else selectParameter("decay", "DECAY", "SEC"); };
    sizeSelectButton.onClick = [this] { if (secondBank) selectParameter("earlyLevel", "EARLY LEVEL", "%"); else selectParameter("size", "SIZE", "X"); };
    predelaySelectButton.onClick = [this] { if (secondBank) selectParameter("lateLevel", "LATE LEVEL", "%"); else selectParameter("predelay", "PREDELAY", "MS"); };
    diffusionSelectButton.onClick = [this] { if (secondBank) selectParameter("crossfeed", "CROSSFEED", "%"); else selectParameter("diffusion", "DIFFUSION", "%"); };
    dampingSelectButton.onClick = [this] { selectParameter("damping", "DAMPING", "HZ"); };
    modulationSelectButton.onClick = [this] { selectParameter("modulation", "MODULATION", "%"); };
    widthSelectButton.onClick = [this] { selectParameter("width", "WIDTH", "%"); };
    mixSelectButton.onClick = [this] { selectParameter("mix", "MIX", "%"); };
    inputSelectButton.onClick = [this] { selectParameter("input", "INPUT", "DB"); };
    outputSelectButton.onClick = [this] { selectParameter("output", "OUTPUT", "DB"); };
    bankButton.setButtonText("BANK A");
    bankButton.setTooltip("Switch parameter bank A / B");
    bankButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff30342f));
    bankButton.setColour(juce::TextButton::textColourOffId, green);
    bankButton.onClick = [this]
    {
        secondBank = !secondBank;
        bankButton.setButtonText(secondBank ? "BANK B" : "BANK A");
        repaint(45, 100, 475, 174);
    };
    addAndMakeVisible(bankButton);
    softKnob.onValueChange = [this] { repaint(55, 225, 430, 35); };

    setSize(760, 470);
    programSelector.setBounds(60, 143, 440, 34);
    softKnob.setBounds(584, 128, 100, 100);
    decaySelectButton.setBounds(52, 328, 42, 38);
    sizeSelectButton.setBounds(101, 328, 42, 38);
    predelaySelectButton.setBounds(150, 328, 42, 38);
    diffusionSelectButton.setBounds(199, 328, 42, 38);
    dampingSelectButton.setBounds(248, 328, 42, 38);
    modulationSelectButton.setBounds(297, 328, 42, 38);
    widthSelectButton.setBounds(346, 328, 42, 38);
    mixSelectButton.setBounds(395, 328, 42, 38);
    inputSelectButton.setBounds(444, 328, 42, 38);
    outputSelectButton.setBounds(493, 328, 42, 38);
    bankButton.setBounds(410, 108, 92, 24);
    PCM70Diagnostics::log("LCD READOUT GUI 0.4.17: setSize and controls setup returned");
    PCM70Diagnostics::log("LCD READOUT GUI 0.4.17: editor ready");
}

void PCM70LegacyAudioProcessorEditor::selectParameter(const juce::String& parameterId, const juce::String& displayName, const juce::String& unit)
{
    activeKnobAttachment.reset();
    activeKnobAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor.parameters, parameterId, softKnob);
    selectedParameterName = displayName;
    selectedParameterUnit = unit;
    softKnob.setTooltip("SOFT KNOB — " + displayName);
    repaint(55, 225, 430, 35);
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
    g.drawText(secondBank ? "PARAMETER BANK B" : "PARAMETER BANK A", 275, 112, 225, 18, juce::Justification::right);
    g.setFont(juce::Font("Consolas", 11.0f, juce::Font::bold));
    g.drawText("PARAMETER", 62, 207, 180, 18, juce::Justification::left);
    g.drawText("EDIT VALUE", 330, 207, 150, 18, juce::Justification::right);
    g.setColour(juce::Colour(0xff566b52));
    g.drawHorizontalLine(226, 62.0f, 482.0f);
    g.setColour(green);
    g.setFont(juce::Font("Consolas", 15.0f, juce::Font::bold));
    g.drawText(selectedParameterName, 62, 234, 205, 24, juce::Justification::left);

    const auto parameterValue = softKnob.getValue();
    juce::String valueText;
    if (selectedParameterUnit == "MS" || selectedParameterUnit == "HZ" || selectedParameterUnit == "%")
        valueText = juce::String(juce::roundToInt(parameterValue));
    else if (selectedParameterUnit == "DB")
        valueText = (parameterValue > 0.0001 ? "+" : "") + juce::String(parameterValue, 1);
    else
        valueText = juce::String(parameterValue, 2);
    g.drawText(valueText + " " + selectedParameterUnit, 270, 234, 212, 24, juce::Justification::right);

    g.setColour(inkColour);
    g.setFont(juce::Font(10.0f, juce::Font::bold));
    g.drawText("PARAMETER SELECT", 45, 294, 475, 18, juce::Justification::centredLeft);
    g.drawText("SOFT KNOB", 565, 294, 145, 18, juce::Justification::centred);
    g.setColour(juce::Colour(0xff77796e));
    g.drawHorizontalLine(283, 38.0f, 722.0f);
    g.setColour(juce::Colour(0xffd8d7ca));
    g.drawHorizontalLine(92, 32.0f, 728.0f);
    g.drawHorizontalLine(383, 32.0f, 728.0f);

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
