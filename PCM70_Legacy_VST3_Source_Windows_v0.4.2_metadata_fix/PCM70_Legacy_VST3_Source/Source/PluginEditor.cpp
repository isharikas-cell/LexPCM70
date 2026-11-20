#include "PluginEditor.h"

PCM70LegacyAudioProcessorEditor::Knob::Knob(juce::AudioProcessorValueTreeState& state,
                                             const juce::String& id, const juce::String& text)
{
    label.setText(text, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    label.setColour(juce::Label::textColourId, juce::Colour(0xffd8c9a6));
    label.setFont(juce::Font(12.0f, juce::Font::bold));
    addAndMakeVisible(label);
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 66, 18);
    slider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xffc18b45));
    slider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff45413a));
    slider.setColour(juce::Slider::thumbColourId, juce::Colour(0xffead9b5));
    slider.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xffeee4d0));
    slider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xff252523));
    addAndMakeVisible(slider);
    attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(state, id, slider);
}
void PCM70LegacyAudioProcessorEditor::Knob::resized()
{
    label.setBounds(0, 0, getWidth(), 20);
    slider.setBounds(0, 18, getWidth(), getHeight() - 18);
}

PCM70LegacyAudioProcessorEditor::PCM70LegacyAudioProcessorEditor(PCM70LegacyAudioProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    setSize(760, 470);
    title.setText("PCM70 LEGACY", juce::dontSendNotification);
    title.setFont(juce::Font(30.0f, juce::Font::bold));
    title.setColour(juce::Label::textColourId, juce::Colour(0xffe4d1a7));
    title.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(title);
    subtitle.setText("STEREO REVERB  /  VINTAGE DIGITAL SPACE", juce::dontSendNotification);
    subtitle.setFont(juce::Font(11.0f));
    subtitle.setColour(juce::Label::textColourId, juce::Colour(0xff9b927f));
    subtitle.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(subtitle);

    program.addItem("CONCERT HALL", 1);
    program.addItem("RICH PLATE", 2);
    program.addItem("WARM ROOM", 3);
    program.addItem("AMBIENCE", 4);
    program.setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xff292825));
    program.setColour(juce::ComboBox::textColourId, juce::Colour(0xffe8dcc3));
    addAndMakeVisible(program);
    programAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        processor.parameters, "program", program);

    const std::array<std::pair<const char*, const char*>, 10> defs {{
        {"decay","DECAY"}, {"size","SIZE"}, {"predelay","PREDELAY"}, {"diffusion","DIFFUSION"},
        {"damping","HF DAMP"}, {"modulation","MODULATION"}, {"width","WIDTH"},
        {"mix","MIX"}, {"input","INPUT"}, {"output","OUTPUT"}
    }};
    for (size_t i = 0; i < defs.size(); ++i)
    {
        knobs[i] = std::make_unique<Knob>(processor.parameters, defs[i].first, defs[i].second);
        addAndMakeVisible(*knobs[i]);
    }
}
void PCM70LegacyAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff171817));
    g.setColour(juce::Colour(0xff35332d));
    g.fillRoundedRectangle(getLocalBounds().toFloat().reduced(10.0f), 10.0f);
    g.setColour(juce::Colour(0xff75664b));
    g.drawRoundedRectangle(getLocalBounds().toFloat().reduced(10.0f), 10.0f, 1.2f);
    g.setColour(juce::Colour(0xff24231f));
    g.fillRoundedRectangle(juce::Rectangle<float>(24, 104, 712, 330), 7.0f);
    g.setColour(juce::Colour(0xff6c5b3f));
    g.drawHorizontalLine(91, 25.0f, 735.0f);
}
void PCM70LegacyAudioProcessorEditor::resized()
{
    title.setBounds(24, 18, 712, 42);
    subtitle.setBounds(24, 58, 712, 22);
    program.setBounds(285, 108, 190, 28);
    const int left = 32, top = 158, w = 136, h = 128, gap = 5;
    for (int i = 0; i < 7; ++i)
        knobs[(size_t)i]->setBounds(left + i * (w + gap), top, w, h);
    for (int i = 7; i < 10; ++i)
        knobs[(size_t)i]->setBounds(left + (i - 7) * (w + gap), top + 132, w, h);
}
