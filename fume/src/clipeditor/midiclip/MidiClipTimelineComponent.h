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

		updateSnapType();
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

		auto secondsPerPixel = extendedMidiClip.getSecondsPerPixel();
		auto& tempoSequence = uiContext.extendedEdit.getTempoSequence();
		
		auto leftTime = te::TimePosition::fromSeconds(offset * secondsPerPixel);

		auto startTime = snapType.roundTimeUp(leftTime, tempoSequence);
		auto endTime = leftTime + te::TimeDuration::fromSeconds(localBounds.getWidth() * secondsPerPixel);

		auto time = startTime;

		auto timeOffset = te::TimeDuration::fromSeconds(0.0001);

		juce::Path gridPath;

		while (time < endTime)
		{
			auto x = (time.inSeconds()) / secondsPerPixel - offset;

			gridPath.startNewSubPath(x, 0);
			gridPath.lineTo(x, localBounds.getHeight());

			auto barBeats = tempoSequence.timeToBarsBeats(time);

			auto text = fumeUI::beatToString(barBeats, false);

			g.drawText(text, x + 4, 0, 40, 20, juce::Justification::left, false);
			
			time = snapType.roundTimeUp(time + timeOffset, tempoSequence);
		}

		g.setColour(juce::Colours::darkgrey);
		g.strokePath(gridPath, juce::PathStrokeType(1.0f));
	}

	void setOffset(int newOffset) {
		offset = newOffset;
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
	int offset = 0;
	te::TimecodeSnapType snapType{ te::TimecodeType::barsBeats, 1 }; 

	void updateSnapType()
	{
		auto& extendedMidiClip = uiContext.extendedMidiClip;
		auto secondsPerPixel = extendedMidiClip.getSecondsPerPixel();
		auto leftSeconds = te::TimePosition::fromSeconds(offset * secondsPerPixel);
		te::TempoSequence& tempoSequence = uiContext.extendedEdit.getEdit().tempoSequence;
		bool isTripletOverride = false;
		auto displayFormat = uiContext.extendedEdit.getEdit().getTimecodeFormat();
		snapType = fumeHelpers::getBestSnapType(displayFormat, fumeUI::midiEditorHeaderMinimumPixelsPerSnap, tempoSequence.getTempoAt(leftSeconds), te::TimeDuration::fromSeconds(secondsPerPixel), isTripletOverride);

	}

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiClipTimelineComponent)
};