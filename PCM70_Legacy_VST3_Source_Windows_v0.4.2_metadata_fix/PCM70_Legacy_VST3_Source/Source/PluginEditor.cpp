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
    : AudioProcessorEditor(&p), processor(p)
{
    PCM70Diagnostics::log("PCM70 v1.0 editor constructor");
    setSize(760, 470);
    PCM70Diagnostics::log("GUI checkpoint 01: setSize complete");

    program.addItem("CONCERT HALL", 1);
    program.addItem("RICH PLATE", 2);
    program.addItem("WARM ROOM", 3);
    program.addItem("AMBIENCE", 4);
    program.setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xff182019));
    program.setColour(juce::ComboBox::textColourId, green);
    program.setColour(juce::ComboBox::outlineColourId, juce::Colour(0xff53644f));
    addAndMakeVisible(program);
    PCM70Diagnostics::log("GUI checkpoint 02: program component visible");
    programAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        processor.parameters, "program", program);
    PCM70Diagnostics::log("GUI checkpoint 03: program attachment complete");

    softKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    softKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    softKnob.setRotaryParameters(juce::MathConstants<float>::pi * 1.25f,
                                 juce::MathConstants<float>::pi * 2.75f, true);
    softKnob.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xffbdc1b3));
    softKnob.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff51564e));
    softKnob.setColour(juce::Slider::thumbColourId, juce::Colour(0xff242822));
    addAndMakeVisible(softKnob);
    softKnob.onValueChange = [this] { updateDisplay(); };
    PCM70Diagnostics::log("GUI checkpoint 04: soft knob setup complete");

    valueLabel.setJustificationType(juce::Justification::centred);
    valueLabel.setColour(juce::Label::textColourId, green);
    valueLabel.setFont(juce::Font("Consolas", 14.0f, juce::Font::bold));
    addAndMakeVisible(valueLabel);
    PCM70Diagnostics::log("GUI checkpoint 05: value label setup complete");

    for (int i = 0; i < 10; ++i)
    {
        numberButtons[(size_t)i] = std::make_unique<juce::TextButton>(juce::String(i));
        styleButton(*numberButtons[(size_t)i], juce::String(i));
        numberButtons[(size_t)i]->onClick = [this, i] { selectParameter(i); };
        addAndMakeVisible(*numberButtons[(size_t)i]);
        PCM70Diagnostics::log("GUI checkpoint button created: " + juce::String(i));
    }
    PCM70Diagnostics::log("GUI checkpoint 06: all number buttons complete");
    selectParameter(0);
    PCM70Diagnostics::log("GUI checkpoint 07: initial parameter selected");
    PCM70Diagnostics::log("PCM70 v1.0 editor ready");
}

void PCM70LegacyAudioProcessorEditor::styleButton(juce::TextButton& button, const juce::String& text)
{
    button.setButtonText(text);
    button.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff30342f));
    button.setColour(juce::TextButton::buttonOnColourId, juce::Colour(0xff59644f));
    button.setColour(juce::TextButton::textColourOffId, juce::Colour(0xffe0dfd2));
    button.setColour(juce::TextButton::textColourOnId, green);
    button.setClickingTogglesState(false);
}

void PCM70LegacyAudioProcessorEditor::selectParameter(int index)
{
    selectedParameter = juce::jlimit(0, 9, index);
    parameterAttachment.reset();
    softKnob.setEnabled(false);
    parameterAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor.parameters, parameterIds[(size_t)selectedParameter], softKnob);
    softKnob.setEnabled(true);
    for (int i = 0; i < 10; ++i)
        numberButtons[(size_t)i]->setToggleState(i == selectedParameter, juce::dontSendNotification);
    updateDisplay();
    repaint();
}

void PCM70LegacyAudioProcessorEditor::updateDisplay()
{
    const auto id = parameterIds[(size_t)selectedParameter];
    const auto* parameter = processor.parameters.getParameter(id);
    const auto* value = processor.parameters.getRawParameterValue(id);
    if (parameter != nullptr && value != nullptr)
        valueLabel.setText(parameterNames[(size_t)selectedParameter] + "    " +
                           parameter->getText(parameter->getValue(), 12),
                           juce::dontSendNotification);
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
    g.setColour(juce::Colour(0xff8b2118));
    g.setFont(juce::Font(13.0f, juce::Font::bold));
    g.drawText("GUI TEST 1.0", 565, 58, 153, 20, juce::Justification::centredRight);

    // Recessed green LCD window
    g.setColour(juce::Colour(0xff101810));
    g.fillRoundedRectangle(juce::Rectangle<float>(45, 100, 475, 174), 4.0f);
    g.setColour(juce::Colour(0xff566b52));
    g.drawRoundedRectangle(juce::Rectangle<float>(45, 100, 475, 174), 4.0f, 2.0f);
    g.setColour(green.withAlpha(0.75f));
    g.setFont(juce::Font("Consolas", 11.0f, juce::Font::bold));
    g.drawText("PROGRAM / EDIT", 62, 112, 200, 18, juce::Justification::left);
    g.drawText("PARAMETER", 62, 207, 110, 18, juce::Justification::left);

    g.setColour(inkColour);
    g.setFont(juce::Font(10.0f, juce::Font::bold));
    g.drawText("PARAMETER SELECT", 45, 294, 475, 18, juce::Justification::centredLeft);
    g.drawText("SOFT KNOB", 565, 294, 145, 18, juce::Justification::centred);
    g.setColour(juce::Colour(0xff77796e));
    g.drawHorizontalLine(283, 38.0f, 722.0f);

    // Screws and understated hardware details
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
    program.setBounds(285, 145, 190, 28);
    valueLabel.setBounds(62, 226, 440, 26);
    softKnob.setBounds(578, 132, 112, 112);
    const int x0 = 52, y = 328, w = 42, h = 38, gap = 7;
    for (int i = 0; i < 10; ++i)
        numberButtons[(size_t)i]->setBounds(x0 + i * (w + gap), y, w, h);
}
