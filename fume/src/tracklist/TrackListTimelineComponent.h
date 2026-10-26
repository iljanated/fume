#pragma once

#include <JuceHeader.h>
#include "Utilities.h"

class TrackListTimelineComponent : public juce::Component
{
public:
	TrackListTimelineComponent(fumeUI::UIContext& context) : uiContext(context)
	{
	}

	~TrackListTimelineComponent() override = default;

	void paint(juce::Graphics& g) override
	{
		auto localBounds = getLocalBounds();

		g.fillAll(juce::Colours::lightgrey);

		float zoomX = uiContext.edit.state.getProperty(FumeIDs::ZoomX, 1.0f);
		
		auto secondsPerPixel = fumeUI::timelineSecondsPerPixel / zoomX;

		auto leftSeconds = te::TimePosition::fromSeconds(offset * secondsPerPixel);

		te::TempoSequence& tempoSequence = uiContext.edit.tempoSequence;
		bool isTripletOverride = false;
		auto displayFormat = uiContext.edit.getTimecodeFormat();
		auto snapType = fumeHelpers::getBestSnapType(displayFormat, fumeUI::timelineHeaderMinimumPixelsPerSnap, tempoSequence.getTempoAt(leftSeconds), te::TimeDuration::fromSeconds(secondsPerPixel), isTripletOverride);

		auto startTime = snapType.roundTimeUp(leftSeconds, tempoSequence);
		auto endTime = te::TimePosition::fromSeconds((offset + localBounds.getWidth()) * secondsPerPixel);

		auto time = startTime;
		
		juce::Path gridPath;
		
		g.setColour(juce::Colours::darkgrey);
		DBG("TimelineComponent::paint: offset = " << offset << ", leftSeconds = " << leftSeconds.inSeconds() << ", startTime = " << startTime.inSeconds() << ", endTime = " << endTime.inSeconds());
		while (time < endTime)
		{
			auto x = time.inSeconds() / secondsPerPixel - offset;
			
			
			gridPath.startNewSubPath(x, 0);
			gridPath.lineTo(x, localBounds.getHeight());

			//auto beatNumber = static_cast<int>((-offset + x) / unitX);
			//g.drawText(std::to_string(beatNumber), x + 2, 2, 30, 20, juce::Justification::left);

			time = snapType.roundTimeUp(time + te::TimeDuration::fromSeconds(0.0001), tempoSequence);
		}

		g.strokePath(gridPath, juce::PathStrokeType(1.0f));
	}

	void setOffset(int newOffset) { offset = newOffset; repaint(); }

private:
	fumeUI::UIContext& uiContext;
	int offset = 0;
};