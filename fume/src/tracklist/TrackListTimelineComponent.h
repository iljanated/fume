#pragma once

#include <JuceHeader.h>
#include "UIContext.h"

class TrackListTimelineComponent : public juce::Component, public juce::ValueTree::Listener
{
public:
	TrackListTimelineComponent(fumeUI::UIContext& context) : uiContext(context)
	{
		context.extendedEdit.getState().addListener(this);
	}

	~TrackListTimelineComponent() override
	{
		uiContext.extendedEdit.getState().removeListener(this);
	}

	void paint(juce::Graphics& g) override
	{
		auto& extendedEdit = uiContext.extendedEdit;
		auto localBounds = getLocalBounds();

		g.fillAll(juce::Colours::lightgrey);

		auto secondsPerPixel = extendedEdit.getSecondsPerPixel();

		auto leftSeconds = te::TimePosition::fromSeconds(offset * secondsPerPixel);

		te::TempoSequence& tempoSequence = extendedEdit.getTempoSequence();
		
		auto startTime = snapType.roundTimeUp(leftSeconds, tempoSequence);
		auto endTime = te::TimePosition::fromSeconds((offset + localBounds.getWidth()) * secondsPerPixel);

		auto time = startTime;
		
		auto timeOffset = te::TimeDuration::fromSeconds(0.0001);

		juce::Path gridPath;
		
		while (time < endTime)
		{
			auto x = time.inSeconds() / secondsPerPixel - offset;
			
			gridPath.startNewSubPath(x, 0);
			gridPath.lineTo(x, localBounds.getHeight());

			auto barBeats = tempoSequence.timeToBarsBeats(time);
			if (barBeats.beats.inBeats() == 0)
			{
				g.drawText(std::to_string(static_cast<int>(barBeats.getTotalBars())), x + 4, 0, 30, 20, juce::Justification::left);
			}

			time = snapType.roundTimeUp(time + timeOffset, tempoSequence);
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
	fumeUI::UIContext& uiContext;
	te::engine::TimecodeSnapType snapType{ te::engine::TimecodeType::barsBeats, 1 };
	int offset = 0;

	void updateSnapType()
	{
		auto& extendedEdit = uiContext.extendedEdit;
		auto secondsPerPixel = extendedEdit.getSecondsPerPixel();
		auto leftSeconds = te::TimePosition::fromSeconds(offset * secondsPerPixel);

		bool isTripletOverride = false;
		auto displayFormat = extendedEdit.getEdit().getTimecodeFormat();
		snapType = fumeHelpers::getBestSnapType(
			displayFormat, 
			fumeUI::timelineHeaderMinimumPixelsPerSnap, 
			extendedEdit.getTempoSequence().getTempoAt(leftSeconds), 
			te::TimeDuration::fromSeconds(secondsPerPixel), isTripletOverride
		);
	}

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackListTimelineComponent)
};