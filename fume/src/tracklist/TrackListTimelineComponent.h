#pragma once

#include <JuceHeader.h>
#include "UIContext.h"

class TrackListTimelineComponent : public juce::Component, public juce::ValueTree::Listener
{
public:
	TrackListTimelineComponent(fumeUI::UIContext& context) : uiContext(context)
	{
		tempoSequenceState = uiContext.extendedEdit.getTempoSequence().getState();

		uiContext.extendedEdit.getState().addListener(this);
		uiContext.extendedEdit.getTransport().state.addListener(this);
		tempoSequenceState.addListener(this);

		loopPoint1.referTo(uiContext.extendedEdit.getTransport().state, te::IDs::loopPoint1, nullptr);
		loopPoint2.referTo(uiContext.extendedEdit.getTransport().state, te::IDs::loopPoint2, nullptr);

		updateSnapType();
	}

	~TrackListTimelineComponent() override
	{
		tempoSequenceState.removeListener(this);
		uiContext.extendedEdit.getTransport().state.removeListener(this);
		uiContext.extendedEdit.getState().removeListener(this);
	}

	void paint(juce::Graphics& g) override
	{
		auto& extendedEdit = uiContext.extendedEdit;
		auto localBounds = getLocalBounds();

		auto localHeight = localBounds.getHeight();

		g.fillAll(juce::Colours::lightgrey);

		auto secondsPerPixel = extendedEdit.getSecondsPerPixel();

		auto leftSeconds = te::TimePosition::fromSeconds(offset * secondsPerPixel);
		auto rightSeconds = te::TimePosition::fromSeconds((offset + localBounds.getWidth()) * secondsPerPixel);

		// draw loop points

		auto loopStart = std::min(loopPoint1.get(), loopPoint2.get());
		auto loopEnd = std::max(loopPoint1.get(), loopPoint2.get());

		if (loopStart < rightSeconds.inSeconds() && loopEnd > leftSeconds.inSeconds())
		{
			auto loopStartX = (loopStart / secondsPerPixel) - offset;
			auto loopEndX = (loopEnd / secondsPerPixel) - offset;
			g.setColour(juce::Colours::green.withAlpha(0.2f));
			g.fillRect(loopStartX, 0, loopEndX - loopStartX, localHeight);
		
			g.setColour(juce::Colours::darkgrey);
			
			auto leftPath = juce::Path();
			leftPath.addTriangle(
					juce::Point<float>(loopStartX, 0),
					juce::Point<float>(loopStartX + 10, localHeight * 0.5f),
					juce::Point<float>(loopStartX, localHeight)
				);

			auto rightPath = juce::Path();
			rightPath.addTriangle(
				juce::Point<float>(loopEndX, 0),
				juce::Point<float>(loopEndX - 10, localHeight * 0.5f),
				juce::Point<float>(loopEndX, localHeight)
			);
			
			g.fillPath(leftPath);
			g.fillPath(rightPath);
		}

		// draw grid lines

		
		te::TempoSequence& tempoSequence = extendedEdit.getTempoSequence();
		
		auto startTime = snapType.roundTimeUp(leftSeconds, tempoSequence);
		
		auto time = startTime;
		
		auto timeOffset = te::TimeDuration::fromSeconds(0.0001);

		juce::Path gridPath;

		g.setColour(juce::Colours::darkgrey);
		
		while (time < rightSeconds)
		{
			auto x = time.inSeconds() / secondsPerPixel - offset;
			
			gridPath.startNewSubPath(x, 0);
			gridPath.lineTo(x, localHeight);

			auto barBeats = tempoSequence.timeToBarsBeats(time);
			g.drawText(std::to_string(static_cast<int>(barBeats.getTotalBars())), x + 4, 0, 30, 20, juce::Justification::left);
			
			time = snapType.roundTimeUp(time + timeOffset, tempoSequence);
		}

		g.setColour(juce::Colours::darkgrey);
		g.strokePath(gridPath, juce::PathStrokeType(1.0f));
	}

	void resized() override
	{
		updateSnapType();
	}

	void setOffset(int newOffset) { 
		offset = newOffset;
		updateSnapType();
		repaint(); 
	}

	void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property) override
	{
		// edit

		if (property == FumeIDs::ZoomX)
		{
			updateSnapType();
			repaint();
		}

		// transport

		if (property == te::IDs::loopPoint1 || property == te::IDs::loopPoint2)
		{
			repaint();
		}

		// tempo sequence
		if (property == te::IDs::bpm)
		{
			repaint();
		}
	}

private:
	fumeUI::UIContext& uiContext;
	te::engine::TimecodeSnapType snapType{ te::engine::TimecodeType::barsBeats, 1 };
	int offset = 0;
	juce::ValueTree tempoSequenceState;
	juce::CachedValue<double> loopPoint1;
	juce::CachedValue<double> loopPoint2;

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