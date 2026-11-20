#pragma once
#include <JuceHeader.h>
#include <array>
#include <vector>

class PCM70LegacyAudioProcessor : public juce::AudioProcessor
{
public:
    PCM70LegacyAudioProcessor();
    ~PCM70LegacyAudioProcessor() override = default;
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return "PCM70 Legacy"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 30.0; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return "Default"; }
    void changeProgramName(int, const juce::String&) override {}
    void getStateInformation(juce::MemoryBlock&) override;
    void setStateInformation(const void*, int) override;

    juce::AudioProcessorValueTreeState parameters;

private:
    struct DelayLine
    {
        std::vector<float> data;
        int write = 0;
        void prepare(int samples);
        float read(float delaySamples) const;
        void push(float x);
        void clear();
    };
    struct ChannelState
    {
        std::array<DelayLine, 4> comb;
        std::array<DelayLine, 2> diffuser;
        DelayLine early;
        std::array<float, 4> dampingState {};
        float phase = 0.0f;
        float crossState = 0.0f;
    };
    std::array<ChannelState, 2> state;
    double sr = 44100.0;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> mixSmooth, inputGainSmooth, outputGainSmooth;
    // decay, size, predelay, diffusion, damping, modulation, width
    std::array<juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear>, 7> controlSmooth;
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    static float onePole(float current, float input, float coefficient);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PCM70LegacyAudioProcessor)
};
