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
    PCM70Diagnostics::log("HARDWARE FACEPLATE GUI 0.4.19: constructor entered");

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
    const juce::Colour steelTop(0xffc4c4b9), steelBottom(0xff85877e);
    const juce::Colour steelEdge(0xffe0dfd2), darkEdge(0xff555a52);
    const juce::Colour lcdBlack(0xff071008), lcdGreen(0xff9cf58f);

    g.fillAll(juce::Colour(0xff10120f));
    auto outer = getLocalBounds().toFloat().reduced(7.0f);
    g.setGradientFill(juce::ColourGradient(juce::Colour(0xff42463f), outer.getX(), outer.getY(),
                                           juce::Colour(0xff171a16), outer.getRight(), outer.getBottom(), false));
    g.fillRoundedRectangle(outer, 10.0f);
    g.setColour(juce::Colour(0xff73786d));
    g.drawRoundedRectangle(outer, 10.0f, 2.0f);

    auto face = juce::Rectangle<float>(20.0f, 20.0f, 720.0f, 430.0f);
    g.setGradientFill(juce::ColourGradient(steelTop, 0.0f, face.getY(), steelBottom, 0.0f, face.getBottom(), false));
    g.fillRoundedRectangle(face, 5.0f);
    g.setColour(steelEdge);
    g.drawRoundedRectangle(face.reduced(1.5f), 4.0f, 1.2f);
    // Fine horizontal brushing gives the faceplate a restrained anodised-metal texture.
    g.setColour(juce::Colour(0x18ffffff));
    for (int y = 24; y < 447; y += 4)
        g.drawHorizontalLine(y, 25.0f, 735.0f);
    g.setColour(juce::Colour(0x18000000));
    for (int y = 26; y < 447; y += 8)
        g.drawHorizontalLine(y, 25.0f, 735.0f);

    g.setColour(inkColour);
    g.setFont(juce::Font("Arial", 23.0f, juce::Font::bold));
    g.drawText("LEXICON", 38, 29, 190, 28, juce::Justification::centredLeft);
    g.setFont(juce::Font("Arial", 11.5f, juce::Font::bold));
    g.drawText("PCM 70   DIGITAL EFFECTS PROCESSOR", 40, 57, 390, 18, juce::Justification::centredLeft);
    g.setFont(juce::Font("Arial", 10.0f, juce::Font::bold));
    g.drawText("STEREO DIGITAL REVERBERATOR", 480, 39, 238, 16, juce::Justification::centredRight);
    g.drawText("LEGACY SERIES", 580, 57, 138, 15, juce::Justification::centredRight);

    // Recessed, backlit LCD with a dark bezel and luminous green lettering.
    const juce::Rectangle<float> bezel(39.0f, 94.0f, 488.0f, 188.0f);
    g.setColour(juce::Colour(0xff343a32));
    g.fillRoundedRectangle(bezel.expanded(3.0f), 5.0f);
    g.setColour(juce::Colour(0xff0b0e0a));
    g.fillRoundedRectangle(bezel, 3.0f);
    g.setGradientFill(juce::ColourGradient(juce::Colour(0xff142319), lcdBlack,
                                           bezel.getX(), bezel.getY(), bezel.getRight(), bezel.getBottom(), false));
    g.fillRoundedRectangle(juce::Rectangle<float>(45, 100, 476, 176), 2.0f);
    g.setColour(juce::Colour(0xff536e4e));
    g.drawRoundedRectangle(juce::Rectangle<float>(45, 100, 476, 176), 2.0f, 1.0f);
    g.saveState();
    g.reduceClipRegion(juce::Rectangle<int>(47, 102, 472, 172));
    g.setColour(juce::Colour(0x0c9cff8f));
    for (int y = 103; y < 274; y += 3)
        g.drawHorizontalLine(y, 47.0f, 519.0f);
    g.restoreState();

    g.setColour(lcdGreen.withAlpha(0.88f));
    g.setFont(juce::Font("Consolas", 10.5f, juce::Font::bold));
    g.drawText("PROGRAM / EDIT", 62, 110, 190, 17, juce::Justification::left);
    g.drawText(secondBank ? "BANK B  /  EXTENDED" : "BANK A  /  MAIN", 278, 110, 225, 17, juce::Justification::right);
    g.setColour(juce::Colour(0xff526f4d));
    g.drawHorizontalLine(133, 62.0f, 503.0f);
    g.setColour(lcdGreen);
    g.setFont(juce::Font("Consolas", 12.0f, juce::Font::bold));
    g.drawText("PARAMETER", 62, 202, 180, 18, juce::Justification::left);
    g.drawText("EDIT VALUE", 330, 202, 170, 18, juce::Justification::right);
    g.setColour(juce::Colour(0xff526f4d));
    g.drawHorizontalLine(226, 62.0f, 503.0f);
    g.setColour(lcdGreen);
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
    g.setFont(juce::Font("Consolas", 15.0f, juce::Font::bold));
    g.drawText(valueText + " " + selectedParameterUnit, 270, 234, 230, 24, juce::Justification::right);

    // Engraved legends above the ten physical parameter keys; legends follow the active bank.
    g.setColour(inkColour);
    g.setFont(juce::Font("Arial", 9.0f, juce::Font::bold));
    g.drawText("PARAMETER SELECT", 45, 294, 475, 16, juce::Justification::centredLeft);
    g.drawText("SOFT KNOB", 565, 294, 145, 16, juce::Justification::centred);
    const char* bankALabels[10] = { "DECAY", "SIZE", "PRE", "DIFF", "DAMP", "MOD", "WIDTH", "MIX", "INPUT", "OUTPUT" };
    const char* bankBLabels[10] = { "RATE", "EARLY", "LATE", "XFEED", "DAMP", "MOD", "WIDTH", "MIX", "INPUT", "OUTPUT" };
    const int x0 = 52, y = 328, w = 42, h = 38, gap = 7;
    for (int i = 0; i < 10; ++i)
    {
        const int x = x0 + i * (w + gap);
        g.setColour(juce::Colour(0xff343831));
        g.drawText(secondBank ? bankBLabels[i] : bankALabels[i], x - 3, 308, w + 6, 15, juce::Justification::centred);
        // Recess and bevel around each button face.
        g.setColour(juce::Colour(0xff555a51));
        g.fillRoundedRectangle((float)x - 1.0f, (float)y - 1.0f, (float)w + 2.0f, (float)h + 2.0f, 3.5f);
        g.setGradientFill(juce::ColourGradient(juce::Colour(0xff555b51), juce::Colour(0xff20241f),
                                               (float)x, (float)y, (float)x, (float)(y + h), false));
        g.fillRoundedRectangle((float)x, (float)y, (float)w, (float)h, 3.0f);
        g.setColour(juce::Colour(0xff858a7d));
        g.drawRoundedRectangle((float)x + 0.5f, (float)y + 0.5f, (float)w - 1.0f, (float)h - 1.0f, 3.0f, 0.8f);
    }
    g.setColour(juce::Colour(0xff73786e));
    g.drawHorizontalLine(283, 38.0f, 722.0f);
    g.setColour(juce::Colour(0xffe0dfd2));
    g.drawHorizontalLine(92, 32.0f, 728.0f);
    g.drawHorizontalLine(383, 32.0f, 728.0f);

    // Knob escutcheon and tick marks sit behind the live JUCE rotary control.
    g.setColour(juce::Colour(0xff4b5048));
    g.fillEllipse(580.0f, 124.0f, 108.0f, 108.0f);
    g.setColour(juce::Colour(0xffd1d2c7));
    g.drawEllipse(584.0f, 128.0f, 100.0f, 100.0f, 1.5f);
    for (int i = 0; i <= 10; ++i)
    {
        const float a = juce::MathConstants<float>::pi * (1.2f + 1.6f * (float)i / 10.0f);
        const float cx = 634.0f, cy = 178.0f;
        const float r1 = (i % 5 == 0) ? 54.0f : 56.0f;
        const float r2 = 60.0f;
        g.setColour(i % 5 == 0 ? inkColour : darkEdge);
        g.drawLine(cx + std::cos(a) * r1, cy + std::sin(a) * r1,
                   cx + std::cos(a) * r2, cy + std::sin(a) * r2, i % 5 == 0 ? 1.8f : 1.0f);
    }

    // Four visible captive faceplate screws.
    g.setColour(juce::Colour(0xff66685f));
    for (auto point : { juce::Point<float>(29, 29), juce::Point<float>(731, 29),
                        juce::Point<float>(29, 441), juce::Point<float>(731, 441) })
    {
        g.fillEllipse(point.x - 3.5f, point.y - 3.5f, 7.0f, 7.0f);
        g.setColour(juce::Colour(0xffd0cfc2));
        g.drawLine(point.x - 2.0f, point.y - 1.0f, point.x + 2.0f, point.y + 1.0f, 1.0f);
        g.setColour(juce::Colour(0xff66685f));
    }
}

void PCM70LegacyAudioProcessorEditor::resized()
{
}
