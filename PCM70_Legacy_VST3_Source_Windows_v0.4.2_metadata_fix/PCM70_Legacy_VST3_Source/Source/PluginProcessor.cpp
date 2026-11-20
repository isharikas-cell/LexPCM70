#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <algorithm>
#include <cmath>

namespace
{
constexpr float pi = 3.14159265358979323846f;
}

PCM70LegacyAudioProcessor::PCM70LegacyAudioProcessor()
    : AudioProcessor(BusesProperties()
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      parameters(*this, nullptr, "PARAMETERS", createParameterLayout())
{}

juce::AudioProcessorValueTreeState::ParameterLayout PCM70LegacyAudioProcessor::createParameterLayout()
{
    using APF = juce::AudioParameterFloat;
    using APC = juce::AudioParameterChoice;
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> p;
    p.push_back(std::make_unique<APC>("program", "Program",
        juce::StringArray{"Concert Hall", "Rich Plate", "Warm Room", "Ambience"}, 0));
    p.push_back(std::make_unique<APF>("decay", "Decay", juce::NormalisableRange<float>(0.4f, 18.0f, 0.01f, 0.45f), 4.8f, " s"));
    p.push_back(std::make_unique<APF>("size", "Size", juce::NormalisableRange<float>(0.25f, 2.0f, 0.001f), 1.0f));
    p.push_back(std::make_unique<APF>("predelay", "Predelay", juce::NormalisableRange<float>(0.0f, 180.0f, 0.1f), 24.0f, " ms"));
    p.push_back(std::make_unique<APF>("diffusion", "Diffusion", juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 78.0f, "%"));
    p.push_back(std::make_unique<APF>("damping", "HF Damping", juce::NormalisableRange<float>(1000.0f, 18000.0f, 1.0f, 0.35f), 7200.0f, " Hz"));
    p.push_back(std::make_unique<APF>("modulation", "Modulation", juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 18.0f, "%"));
    p.push_back(std::make_unique<APF>("width", "Width", juce::NormalisableRange<float>(0.0f, 150.0f, 0.1f), 100.0f, "%"));
    p.push_back(std::make_unique<APF>("mix", "Mix", juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 25.0f, "%"));
    p.push_back(std::make_unique<APF>("input", "Input", juce::NormalisableRange<float>(-18.0f, 12.0f, 0.1f), 0.0f, " dB"));
    p.push_back(std::make_unique<APF>("output", "Output", juce::NormalisableRange<float>(-18.0f, 12.0f, 0.1f), 0.0f, " dB"));
    return { p.begin(), p.end() };
}

void PCM70LegacyAudioProcessor::DelayLine::prepare(int samples)
{
    data.assign((size_t)juce::jmax(8, samples), 0.0f);
    write = 0;
}
float PCM70LegacyAudioProcessor::DelayLine::read(float delaySamples) const
{
    if (data.empty()) return 0.0f;
    const float maxDelay = (float)data.size() - 2.0f;
    delaySamples = juce::jlimit(1.0f, maxDelay, delaySamples);
    float pos = (float)write - delaySamples;
    while (pos < 0.0f) pos += (float)data.size();
    const int i0 = (int)pos % (int)data.size();
    const int i1 = (i0 + 1) % (int)data.size();
    const float frac = pos - std::floor(pos);
    return data[(size_t)i0] + frac * (data[(size_t)i1] - data[(size_t)i0]);
}
void PCM70LegacyAudioProcessor::DelayLine::push(float x)
{
    if (data.empty()) return;
    data[(size_t)write] = x;
    if (++write >= (int)data.size()) write = 0;
}
void PCM70LegacyAudioProcessor::DelayLine::clear()
{
    std::fill(data.begin(), data.end(), 0.0f);
    write = 0;
}
float PCM70LegacyAudioProcessor::onePole(float current, float input, float coefficient)
{
    return current + coefficient * (input - current);
}

void PCM70LegacyAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    sr = sampleRate;
    mixSmooth.reset(sr, 0.025);
    inputGainSmooth.reset(sr, 0.025);
    outputGainSmooth.reset(sr, 0.025);
    mixSmooth.setCurrentAndTargetValue(parameters.getRawParameterValue("mix")->load() * 0.01f);
    inputGainSmooth.setCurrentAndTargetValue(juce::Decibels::decibelsToGain(parameters.getRawParameterValue("input")->load()));
    outputGainSmooth.setCurrentAndTargetValue(juce::Decibels::decibelsToGain(parameters.getRawParameterValue("output")->load()));
    const float initialControls[7] = {
        parameters.getRawParameterValue("decay")->load(),
        parameters.getRawParameterValue("size")->load(),
        parameters.getRawParameterValue("predelay")->load(),
        parameters.getRawParameterValue("diffusion")->load(),
        parameters.getRawParameterValue("damping")->load(),
        parameters.getRawParameterValue("modulation")->load(),
        parameters.getRawParameterValue("width")->load()
    };
    for (size_t i = 0; i < controlSmooth.size(); ++i)
    {
        controlSmooth[i].reset(sr, 0.035);
        controlSmooth[i].setCurrentAndTargetValue(initialControls[i]);
    }
    const int maxDelay = (int)(sr * 3.0) + samplesPerBlock + 32;
    for (auto& ch : state)
    {
        for (auto& d : ch.comb) d.prepare(maxDelay);
        for (auto& d : ch.diffuser) d.prepare(maxDelay);
        ch.early.prepare(maxDelay);
        ch.dampingState.fill(0.0f);
        ch.phase = 0.0f;
        ch.crossState = 0.0f;
    }
}
void PCM70LegacyAudioProcessor::releaseResources() {}
bool PCM70LegacyAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    return layouts.getMainInputChannelSet() == juce::AudioChannelSet::stereo()
        && layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void PCM70LegacyAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    if (buffer.getNumChannels() < 2) return;

    const int mode = (int)parameters.getRawParameterValue("program")->load();
    mixSmooth.setTargetValue(parameters.getRawParameterValue("mix")->load() * 0.01f);
    inputGainSmooth.setTargetValue(juce::Decibels::decibelsToGain(parameters.getRawParameterValue("input")->load()));
    outputGainSmooth.setTargetValue(juce::Decibels::decibelsToGain(parameters.getRawParameterValue("output")->load()));
    const float controlTargets[7] = {
        parameters.getRawParameterValue("decay")->load(),
        parameters.getRawParameterValue("size")->load(),
        parameters.getRawParameterValue("predelay")->load(),
        parameters.getRawParameterValue("diffusion")->load(),
        parameters.getRawParameterValue("damping")->load(),
        parameters.getRawParameterValue("modulation")->load(),
        parameters.getRawParameterValue("width")->load()
    };
    for (size_t i = 0; i < controlSmooth.size(); ++i)
        controlSmooth[i].setTargetValue(controlTargets[i]);

    // Mode voicings deliberately alter the network's time geometry and density.
    const float modeSize = mode == 1 ? 0.86f : (mode == 2 ? 0.68f : (mode == 3 ? 0.48f : 1.0f));
    const float modeDecay = mode == 1 ? 1.08f : (mode == 2 ? 0.78f : (mode == 3 ? 0.42f : 1.0f));
    const float modeDiffusion = mode == 1 ? 0.96f : (mode == 2 ? 0.82f : (mode == 3 ? 0.55f : 1.0f));
    // Four mutually-prime delay lengths reduce obvious periodic ringing.
    const float baseMs[4] = { 29.7f, 37.1f, 41.1f, 53.3f };
    const float diffuserMs[2] = { 5.3f, 7.9f };

    auto* left = buffer.getWritePointer(0);
    auto* right = buffer.getWritePointer(1);
    for (int i = 0; i < buffer.getNumSamples(); ++i)
    {
        const float mix = mixSmooth.getNextValue();
        const float inGain = inputGainSmooth.getNextValue();
        const float outGain = outputGainSmooth.getNextValue();
        const float dryGain = 1.0f - mix;
        const float decay = controlSmooth[0].getNextValue();
        const float size = controlSmooth[1].getNextValue();
        const float predelayMs = controlSmooth[2].getNextValue();
        const float diffusion = controlSmooth[3].getNextValue() * 0.01f;
        const float dampingHz = controlSmooth[4].getNextValue();
        const float modulation = controlSmooth[5].getNextValue() * 0.01f;
        const float width = controlSmooth[6].getNextValue() * 0.01f;
        const float effectiveSize = size * modeSize;
        const float rt60 = juce::jmax(0.15f, decay * modeDecay);
        const float dampCoeff = 1.0f - std::exp(-2.0f * pi * dampingHz / (float)sr);
        const float preDelay = predelayMs * 0.001f * (float)sr;
        const float in[2] = { left[i] * inGain, right[i] * inGain };
        float wet[2] = { 0.0f, 0.0f };
        const float crossFromLeft = state[0].crossState;
        const float crossFromRight = state[1].crossState;

        for (int c = 0; c < 2; ++c)
        {
            auto& s = state[(size_t)c];
            const float channelSign = c == 0 ? 1.0f : 1.013f;
            s.phase += 2.0f * pi * (0.13f + 0.37f * modulation) / (float)sr;
            if (s.phase >= 2.0f * pi) s.phase -= 2.0f * pi;
            const float modSamples = std::sin(s.phase) * modulation * 2.2f * (float)sr / 1000.0f;

            const float earlyInput = in[c];
            s.early.push(earlyInput);
            float early = 0.0f;
            const float earlyTapMs[4] = { 11.0f, 19.3f, 31.7f, 47.1f };
            const float earlyWeights[4] = { 0.42f, 0.31f, 0.22f, 0.14f };
            for (int t = 0; t < 4; ++t)
                early += s.early.read(preDelay + earlyTapMs[t] * 0.001f * (float)sr) * earlyWeights[t];

            // A small, one-sample-delayed crossfeed couples the stereo tanks while
            // keeping the feedback matrix bounded and avoiding channel-order bias.
            const float crossFeed = c == 0 ? crossFromRight : crossFromLeft;
            float networkInput = (earlyInput * 0.48f + early * 0.44f + crossFeed * 0.08f);
            const float g = 0.18f + 0.72f * diffusion * modeDiffusion;
            for (int ap = 0; ap < 2; ++ap)
            {
                const float d = s.diffuser[(size_t)ap].read(diffuserMs[ap] * 0.001f * (float)sr * effectiveSize);
                const float y = d - g * networkInput;
                s.diffuser[(size_t)ap].push(networkInput + g * y);
                networkInput = y;
            }

            float sum = 0.0f;
            for (int k = 0; k < 4; ++k)
            {
                const float delaySamples = baseMs[k] * 0.001f * (float)sr * effectiveSize * channelSign
                                         + (k % 2 == 0 ? modSamples : -0.73f * modSamples);
                const float delayed = s.comb[(size_t)k].read(delaySamples);
                s.dampingState[(size_t)k] = onePole(s.dampingState[(size_t)k], delayed, dampCoeff);

                // RT60 feedback: amplitude falls by 60 dB over the requested decay time.
                const float delaySeconds = baseMs[k] * 0.001f * effectiveSize * channelSign;
                const float feedback = juce::jlimit(0.0f, 0.985f,
                    std::pow(0.001f, delaySeconds / rt60));
                const float tapGain[4] = { 0.34f, 0.29f, 0.23f, 0.20f };
                // Feed the tank strongly enough to build an audible late field.
                // The previous 0.25 injection followed by 0.19 output scaling
                // made the wet return excessively quiet at normal input levels.
                s.comb[(size_t)k].push(networkInput * 0.40f + s.dampingState[(size_t)k] * feedback);
                sum += delayed * tapGain[k];
            }

            // Keep early reflections controlled while bringing the diffuse tail
            // up to a practical level; the Mix control remains the wet/dry blend.
            wet[c] = early * 0.22f + sum * 0.34f;
            s.crossState = sum * 0.30f + early * 0.10f;
        }

        const float mid = 0.5f * (wet[0] + wet[1]);
        const float side = 0.5f * (wet[0] - wet[1]) * width;
        const float wetL = mid + side;
        const float wetR = mid - side;
        left[i] = (in[0] * dryGain + wetL * mix) * outGain;
        right[i] = (in[1] * dryGain + wetR * mix) * outGain;
    }
}

juce::AudioProcessorEditor* PCM70LegacyAudioProcessor::createEditor()
{
    return new PCM70LegacyAudioProcessorEditor(*this);
}
void PCM70LegacyAudioProcessor::getStateInformation(juce::MemoryBlock& dest)
{
    auto copy = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(copy.createXml());
    copyXmlToBinary(*xml, dest);
}
void PCM70LegacyAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml && xml->hasTagName(parameters.state.getType()))
        parameters.replaceState(juce::ValueTree::fromXml(*xml));
}
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PCM70LegacyAudioProcessor();
}
