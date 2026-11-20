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
		uiContext.extendedEdit.getTransport().state.addListener(this);

		tempoSequenceState = uiContext.extendedEdit.getTempoSequence().getState();
		tempoSequenceState.addListener(this);

		loopPoint1.referTo(uiContext.extendedEdit.getTransport().state, te::IDs::loopPoint1, nullptr);
		loopPoint2.referTo(uiContext.extendedEdit.getTransport().state, te::IDs::loopPoint2, nullptr);

		updateSnapType();
	}

	~MidiClipTimelineComponent() override
	{
		tempoSequenceState.removeListener(this);
		uiContext.extendedEdit.getTransport().state.removeListener(this);
		state.removeListener(this);
	}

	void paint(juce::Graphics& g) override
	{
		auto& extendedMidiClip = uiContext.extendedMidiClip;
		auto localBounds = getLocalBounds();
		auto localHeight = localBounds.getHeight();

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
			gridPath.lineTo(x, localHeight);

			auto barBeats = tempoSequence.timeToBarsBeats(time);

			auto text = fumeUI::beatToString(barBeats, false);

			g.drawText(text, x + 4, 0, 40, 20, juce::Justification::left, false);
			
			time = snapType.roundTimeUp(time + timeOffset, tempoSequence);
		}

		g.setColour(juce::Colours::darkgrey);
		g.strokePath(gridPath, juce::PathStrokeType(1.0f));

		// draw loop points

		auto loopStart = std::min(loopPoint1.get(), loopPoint2.get());
		auto loopEnd = std::max(loopPoint1.get(), loopPoint2.get());

		if (loopStart < endTime.inSeconds() && loopEnd > leftTime.inSeconds())
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

		if (property == te::IDs::loopPoint1 || property == te::IDs::loopPoint2)
		{
			repaint();
		}

		if (property == te::IDs::bpm)
		{
			repaint();
		}
	}

private:
	juce::ValueTree state;
	juce::ValueTree tempoSequenceState;
	fumeUI::UIMidiClipContext& uiContext;
	int offset = 0;
	te::TimecodeSnapType snapType{ te::TimecodeType::barsBeats, 1 };
	juce::CachedValue<double> loopPoint1;
	juce::CachedValue<double> loopPoint2;

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