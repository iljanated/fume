#pragma once

#include <JuceHeader.h>

class PianoRollComponent : public juce::Component
{
public:
    PianoRollComponent() {}
    ~PianoRollComponent() override {}

    void paint(juce::Graphics& g) override
    {
        auto localBounds = getLocalBounds();

		g.fillAll(juce::Colours::grey);

		auto startY = -scrollOffset % FUME_MIDI_EDITOR_NOTE_HEIGHT;

		auto noteIndex = 127 - scrollOffset / FUME_MIDI_EDITOR_NOTE_HEIGHT;

		for (auto y = startY; y < localBounds.getBottom(); y += FUME_MIDI_EDITOR_NOTE_HEIGHT)
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

			g.fillRect(30, y, 20, FUME_MIDI_EDITOR_NOTE_HEIGHT);

			g.setColour(juce::Colours::grey);
			g.drawRect(30, y, 20, FUME_MIDI_EDITOR_NOTE_HEIGHT);

			if (localNoteIndex == 0)
			{
				g.setColour(juce::Colours::darkgrey);
				g.drawLine(0, y + FUME_MIDI_EDITOR_NOTE_HEIGHT, 30, y + FUME_MIDI_EDITOR_NOTE_HEIGHT);
				auto note = static_cast<int>(noteIndex / 12);
				auto rect = juce::Rectangle{ 0, y, 30, FUME_MIDI_EDITOR_NOTE_HEIGHT };
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
	std::array<int, 5> blackNotes = { 1, 3, 6, 8, 10 };
	int scrollOffset = 0;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PianoRollComponent)
};