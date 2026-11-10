#pragma once

#include <JuceHeader.h>
#include "UIMidiClipContext.h"
#include "ToolState.h"
#include "ExtendedMidiClip.h"
#include "MidiEventsEditorCursorComponent.h"

class MidiEventsEditorComponent
    : public juce::Component,
    private juce::ValueTree::Listener,
    public juce::ChangeListener
{
public:
    MidiEventsEditorComponent(fumeUI::UIMidiClipContext& context, juce::Viewport& v);
    ~MidiEventsEditorComponent();
    void paint(juce::Graphics& g) override;
    void resized() override;

	void changeListenerCallback(juce::ChangeBroadcaster* source) override;

	bool keyPressed(const juce::KeyPress& key) override
	{
		if (toolStateManager.keyPressed(key))
		{
			return true;
		}

		if (cursorComponent.keyPressed(key))
		{
			return true;
		}
		return false;
	}

	bool keyStateChanged(bool isKeyDown) override
	{
		if (toolStateManager.keyStateChanged(isKeyDown))
		{
			return true;
		}
		return false;
	}

	te::SelectedMidiEvents& getSelectedEvents() { return *selectedEvents; }
	te::SelectedMidiEvents& getSelectedEvents(bool selectCursorIfEmpty) {
		if (selectCursorIfEmpty && selectedEvents->getSelectedNotes().isEmpty())
		{
			//auto cursorPosition = cursorComponent.getCursorPosition();
			//selectItemsInBox({ cursorPosition.getX(), cursorPosition.getY(), 0, 0 });
		}

		return getSelectedEvents();
	}

	void findLassoItemsInArea(Array<te::MidiNote*>& itemsFound, const Rectangle<int>& area);
	void lassoSelectionChanged(SelectedItemSet<te::MidiNote*>& newSelection);

	fumeUI::UIMidiClipContext& getUIContext() { return uiContext; }
	void showCursor(bool visible) { cursorComponent.setVisible(visible); }

	te::SelectedMidiEvents& clearSelectedEvents();
    
private:
	juce::Viewport& viewport;
	Helpers::AsyncResizer asyncResizer{ *this };
	fumeUI::UIMidiClipContext& uiContext;
	juce::ValueTree state;
	ToolStateManager<MidiEventsEditorComponent> toolStateManager{ uiContext.inputManager, FumeIDs::MidiClipSelectToolState };
	std::unique_ptr<te::SelectedMidiEvents> selectedEvents;
	MidiEventsEditorCursorComponent cursorComponent{ uiContext };
    
	void activateToolState(ToolState* newState);

    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property) override;
    void valueTreeChildAdded(juce::ValueTree& parentTree, juce::ValueTree& childWhichHasBeenAdded) override;
    void valueTreeChildRemoved(juce::ValueTree& parentTree, juce::ValueTree& childWhichHasBeenRemoved, int index) override;
    void valueTreeChildOrderChanged(juce::ValueTree& parentTree, int oldIndex, int newIndex) override;
    void valueTreeParentChanged(juce::ValueTree& treeWhoseParentHasChanged) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiEventsEditorComponent)
};
