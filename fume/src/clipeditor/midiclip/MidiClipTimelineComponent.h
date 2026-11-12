#pragma once

#include <JuceHeader.h>
#include "UIMidiClipContext.h"
#include "Utilities.h"

class MidiClipTimelineComponent : public juce::Component, public juce::ValueTree::Listener
{
public:
	MidiClipTimelineComponent(fumeUI::UIMidiClipContext& context) : uiContext(context)
	{
		state = uiContext.extendedMidiClip.getState();
		state.addListener(this);
	}

	~MidiClipTimelineComponent() override
	{
		state.removeListener(this);
	}

	void paint(juce::Graphics& g) override
	{
		auto& extendedMidiClip = uiContext.extendedMidiClip;
		auto localBounds = getLocalBounds();

		g.fillAll(juce::Colours::lightgrey);

		auto beatsPerPixel = extendedMidiClip.getBeatsPerPixel();

		auto leftBeat = te::BeatPosition::fromBeats(offset * beatsPerPixel);

		auto startBeat = quantisationType.roundBeatUp(leftBeat);
		auto endBeat = leftBeat + te::BeatDuration::fromBeats(localBounds.getWidth() * beatsPerPixel);

		auto beat = startBeat;

		auto beatOffset = te::BeatDuration::fromBeats(0.0001);

		juce::Path gridPath;

		while (beat < endBeat)
		{
			auto x = (beat.inBeats()) / beatsPerPixel - offset;

			gridPath.startNewSubPath(x, 0);
			gridPath.lineTo(x, localBounds.getHeight());

			auto text = fumeUI::beatToString(beat, 4, false);

			g.drawText(text, x + 4, 0, 40, 20, juce::Justification::left, false);
			
			beat = quantisationType.roundBeatUp(beat + beatOffset);
		}

		g.setColour(juce::Colours::darkgrey);
		g.strokePath(gridPath, juce::PathStrokeType(1.0f));
	}

	void setOffset(int newOffset) {
		offset = newOffset;
		updateSnapType();
		repaint();
	}

	void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property) override
	{
		if (property == FumeIDs::ZoomX)
		{
			updateSnapType();
			repaint();
		}
	}

private:
	juce::ValueTree state;
	fumeUI::UIMidiClipContext& uiContext;
	BeatQuantisationType quantisationType{ te::BeatDuration::fromBeats(0.125) };
	int offset = 0;

	void updateSnapType()
	{
		quantisationType = fumeHelpers::getBestBeatQuantisationTypeType(fumeUI::timelineHeaderMinimumPixelsPerSnap, uiContext.extendedMidiClip.getBeatsPerPixel());
	}

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiClipTimelineComponent)
};