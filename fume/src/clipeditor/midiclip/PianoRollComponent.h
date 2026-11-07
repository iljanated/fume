#pragma once

#include "UIMidiClipContext.h"

class PianoRollComponent : public juce::Component
{
public:
    PianoRollComponent(fumeUI::UIMidiClipContext& uiContext) : uiContext(uiContext) {}
    ~PianoRollComponent() override {}

    void paint(juce::Graphics& g) override
    {
        auto localBounds = getLocalBounds();

		auto noteHeight = uiContext.extendedMidiClip.getNoteHeight();

		g.fillAll(juce::Colours::grey);

		auto startY = -std::fmod(scrollOffset, noteHeight);

		auto noteIndex = static_cast<int>(127 - scrollOffset / noteHeight);

		for (auto y = startY; y < localBounds.getBottom(); y += noteHeight)
		{
			auto localNoteIndex = noteIndex % 12;

			if (std::ranges::find(blackNotes, localNoteIndex) != blackNotes.end())
			{
				g.setColour(juce::Colours::black);
			}
			else
			{
				g.setColour(juce::Colours::white);
			}

			g.fillRect(30, y, 20, fumeUI::midiEditorNoteHeight);

			g.setColour(juce::Colours::grey);
			g.drawRect(30, y, 20, fumeUI::midiEditorNoteHeight);

			if (localNoteIndex == 0)
			{
				g.setColour(juce::Colours::darkgrey);
				g.drawLine(0, y + noteHeight, 30, y + noteHeight);
				auto note = static_cast<int>(noteIndex / 12);
				auto rect = juce::Rectangle{ 0, static_cast<int>(y), 30, static_cast<int>(noteHeight) };
				auto label = "c-" + juce::String(note);
				g.drawText(label, rect, juce::Justification::centred, false);
			}

            noteIndex--;
		}
    }

    void resized() override
    {
    }

	void setScrollOffset(int offset)
	{
		scrollOffset = offset;
		repaint();
	}

private:
	fumeUI::UIMidiClipContext& uiContext;
	std::array<int, 5> blackNotes = { 1, 3, 6, 8, 10 };
	int scrollOffset = 0;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PianoRollComponent)
};