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

        const auto file = folder.getChildFile("diagnostics.log");
        const auto line = juce::Time::getCurrentTime().toISO8601(true)
                        + " | " + message + juce::newLine;
        // appendText opens, writes and closes the file for each event.
        // This avoids retaining a stream or relying on a buffered stream's lifetime.
        file.appendText(line, false, "\n");
    }
}
