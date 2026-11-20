#pragma once
#include <JuceHeader.h>

namespace PCM70Diagnostics
{
    inline void log(const juce::String& message)
    {
        static juce::CriticalSection lock;
        const juce::ScopedLock guard(lock);
        auto folder = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
                          .getChildFile("PCM70Legacy");
        if (!folder.exists() && !folder.createDirectory())
            return;

        auto file = folder.getChildFile("diagnostics.log");
        juce::FileOutputStream stream(file, 1024);
        if (!stream.openedOk())
            return;

        stream.setPosition(stream.getTotalLength());
        stream.writeText(juce::Time::getCurrentTime().toISO8601(true)
                             + " | " + message + juce::newLine, false, false, nullptr);
        stream.flush();
    }
}
