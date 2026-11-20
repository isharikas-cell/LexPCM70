#include "PluginEditor.h"
#include "DiagnosticLog.h"

namespace
{
const juce::Colour background(0xff101317), panel(0xff20252b), blue(0xff4d83b5);
const juce::Colour lcd(0xff91dc68), text(0xffe3e6e8);
const char* ids[] = { "decay","size","predelay","diffusion","damping","modulation","width","mix","input","output","modRate","earlyLevel","lateLevel","crossfeed" };
const char* names[] = { "DECAY","SIZE","PREDELAY","DIFFUSION","DAMPING","MODULATION","WIDTH","MIX","INPUT","OUTPUT","MOD RATE","EARLY LEVEL","LATE LEVEL","CROSSFEED" };
}
PCM70LegacyAudioProcessorEditor::PCM70LegacyAudioProcessorEditor(PCM70LegacyAudioProcessor& p)
    : juce::AudioProcessorEditor(&p), processor(p)
{
    PCM70Diagnostics::log("HYBRID PCM70 GUI C 0.4.20: constructor entered");
    programSelector.addItem("CONCERT HALL",1); programSelector.addItem("RICH PLATE",2);
    programSelector.addItem("WARM ROOM",3); programSelector.addItem("AMBIENCE",4);
    programSelector.setColour(juce::ComboBox::backgroundColourId,juce::Colour(0xff101820));
    programSelector.setColour(juce::ComboBox::textColourId,lcd);
    programSelector.setColour(juce::ComboBox::outlineColourId,blue);
    programSelector.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(programSelector);
    programAttachment=std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(processor.parameters,"program",programSelector);
    for(int i=0;i<parameterCount;++i)
    {
        auto& s=controls[(size_t)i];
        s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        s.setTextBoxStyle(juce::Slider::TextBoxBelow,false,62,19);
        s.setColour(juce::Slider::rotarySliderFillColourId,blue);
        s.setColour(juce::Slider::rotarySliderOutlineColourId,juce::Colour(0xff65717c));
        s.setColour(juce::Slider::thumbColourId,juce::Colour(0xffe5e8eb));
        s.setColour(juce::Slider::textBoxTextColourId,text);
        s.setColour(juce::Slider::textBoxBackgroundColourId,juce::Colour(0xff171b20));
        s.setColour(juce::Slider::textBoxOutlineColourId,juce::Colour(0xff3b4650));
        s.setTooltip(names[i]); addAndMakeVisible(s);
        labels[(size_t)i].setText(names[i],juce::dontSendNotification);
        labels[(size_t)i].setJustificationType(juce::Justification::centred);
        labels[(size_t)i].setColour(juce::Label::textColourId,text);
        labels[(size_t)i].setFont(juce::Font("Arial",10.5f,juce::Font::bold));
        addAndMakeVisible(labels[(size_t)i]);
        attachments[(size_t)i]=std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.parameters,ids[i],s);
    }
    setResizable(true,true); setResizeLimits(900,430,1500,650); setSize(1120,500);
    PCM70Diagnostics::log("HYBRID PCM70 GUI C 0.4.20: editor ready");
}
void PCM70LegacyAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(background);
    auto frame=getLocalBounds().toFloat().reduced(8.0f);
    g.setColour(juce::Colour(0xff07090b)); g.fillRoundedRectangle(frame,9.0f);
    g.setColour(blue); g.drawRoundedRectangle(frame,9.0f,1.4f);
    auto rack=juce::Rectangle<float>(18,18,(float)getWidth()-36,166);
    g.setGradientFill(juce::ColourGradient(juce::Colour(0xff30363d),rack.getX(),rack.getY(),juce::Colour(0xff11161c),rack.getX(),rack.getBottom(),false));
    g.fillRoundedRectangle(rack,5.0f); g.setColour(blue); g.drawRoundedRectangle(rack.reduced(1),5,1);
    g.setColour(juce::Colour(0xff080b0e)); g.fillRoundedRectangle(205,38,280,104,4);
    g.setColour(juce::Colour(0xff405d37)); g.fillRoundedRectangle(211,44,268,92,2);
    g.setColour(lcd); g.setFont(juce::Font("Consolas",17,juce::Font::bold));
    g.drawText("LEXICON  PCM 70",224,53,240,25,juce::Justification::centredLeft);
    g.setFont(juce::Font("Consolas",13,juce::Font::bold));
    g.drawText("DIGITAL REVERBERATOR",224,83,240,20,juce::Justification::centredLeft);
    g.drawText("PROGRAM  /  EDIT",224,108,240,18,juce::Justification::centredLeft);
    g.setColour(text); g.setFont(juce::Font("Arial",10,juce::Font::bold));
    g.drawText("INPUT",48,42,105,16,juce::Justification::centred);
    g.drawText("HEADROOM",48,132,105,15,juce::Justification::centred);
    g.drawText("PCM70",getWidth()-143,45,105,25,juce::Justification::centred);
    g.setColour(blue); g.drawText("LEGACY",getWidth()-143,68,105,18,juce::Justification::centred);
    for(int i=0;i<10;++i)
    {
        const int col=i%5,row=i/5;
        juce::Rectangle<float> key(535.0f+col*47.0f,54.0f+row*43.0f,37.0f,32.0f);
        g.setColour(juce::Colour(0xff080b0e)); g.fillRoundedRectangle(key,4);
        g.setColour(juce::Colour(0xff68727c)); g.drawRoundedRectangle(key,4,0.8f);
        g.setColour(text); g.setFont(juce::Font("Arial",12,juce::Font::bold));
        g.drawText(juce::String((i+1)%10),key.toNearestInt(),juce::Justification::centred);
    }
    const char* keys[]={"PGM","RCV","LOAD","BYP"};
    for(int i=0;i<4;++i)
    {
        juce::Rectangle<float> key((float)getWidth()-330.0f+(i%2)*67.0f,53.0f+(i/2)*43.0f,54,32);
        g.setColour(juce::Colour(0xff111820)); g.fillRoundedRectangle(key,4);
        g.setColour(blue); g.drawRoundedRectangle(key,4,0.8f);
        g.setColour(text); g.setFont(juce::Font("Arial",10,juce::Font::bold));
        g.drawText(keys[i],key.toNearestInt(),juce::Justification::centred);
    }
    g.setColour(blue); g.drawHorizontalLine(196,22,(float)getWidth()-22);
    g.setColour(text); g.setFont(juce::Font("Arial",13,juce::Font::bold));
    g.drawText("PARAMETER EDIT",30,203,250,22,juce::Justification::centredLeft);
    g.setFont(juce::Font("Arial",10,juce::Font::plain));
    g.drawText("14 CONTROLS  |  REAL-TIME DSP",getWidth()-300,205,270,18,juce::Justification::centredRight);
    auto lower=juce::Rectangle<float>(20,232,(float)getWidth()-40,(float)getHeight()-250);
    g.setColour(panel); g.fillRoundedRectangle(lower,5);
    g.setColour(juce::Colour(0xff414c56)); g.drawRoundedRectangle(lower,5,1);
    for(int x=1;x<3;++x)
    {
        const float split=20.0f+((float)getWidth()-40.0f)*(float)x/3.0f;
        g.setColour(juce::Colour(0xff3c4852)); g.drawVerticalLine((int)split,247,(float)getHeight()-30);
    }
}
void PCM70LegacyAudioProcessorEditor::resized()
{
    const int margin=28, available=getWidth()-margin*2, cell=available/parameterCount;
    const int knobSize=juce::jmin(58,cell-5), knobY=274;
    for(int i=0;i<parameterCount;++i)
    {
        const int x=margin+i*cell;
        labels[(size_t)i].setBounds(x,253,cell,18);
        controls[(size_t)i].setBounds(x+(cell-knobSize)/2,knobY,knobSize,juce::jmax(120,getHeight()-knobY-25));
    }
    programSelector.setBounds(218,119,254,20);
}
