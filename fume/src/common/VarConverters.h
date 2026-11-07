#pragma once

#include <JuceHeader.h>

namespace te = tracktion;

namespace juce
{
    template <>
    struct VariantConverter<te::TimecodeSnapType>
    {
        static te::TimecodeSnapType fromVar(const var& v)
        {
            if (v.isVoid())
                return {};

            const auto text = v.toString();
            const auto sep = text.indexOfChar(':');

            if (sep < 0)
                return {};

            const auto typeValue = text.substring(0, sep).getIntValue();
            const auto level = text.substring(sep + 1).getIntValue();

            auto type = te::TimecodeType::millisecs;

            switch (typeValue)
            {
                case (int)te::TimecodeType::barsBeats: type = te::TimecodeType::barsBeats; break;
                case (int)te::TimecodeType::fps24: type = te::TimecodeType::fps24; break;
                case (int)te::TimecodeType::fps25: type = te::TimecodeType::fps25; break;
                case (int)te::TimecodeType::fps30: type = te::TimecodeType::fps30; break;
                default: break;
            }

            return { type, level };
        }

        static var toVar(const te::TimecodeSnapType& snapType)
        {
            return juce::String((int)snapType.type) + ":" + juce::String(snapType.level);
        }
    };
}