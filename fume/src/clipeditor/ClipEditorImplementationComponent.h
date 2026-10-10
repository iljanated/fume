#pragma once

#include <JuceHeader.h>

namespace te = tracktion;

class ClipEditorImplementationComponent : public juce::Component
{
public:
	virtual te::Clip* getClip() = 0;
};