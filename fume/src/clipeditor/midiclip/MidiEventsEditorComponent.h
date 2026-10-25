#pragma once

#include <JuceHeader.h>
#include "Utilities.h"
#include "ClipEditorComponent.h"
#include "ExtendedMidiClip.h"
#include "SelectionBoxComponent.h"
#include "ToolState.h"
#include "GridEditor.h"

class MidiEventsEditorComponent
    : public GridEditor<MidiEventsEditorComponent>,
    private juce::ValueTree::Listener,
    private juce::AsyncUpdater,
    public juce::ChangeListener
{
public:
    MidiEventsEditorComponent(te::MidiClip* clip, fumeUI::UIContext& context, juce::Viewport& v);
    ~MidiEventsEditorComponent();
    void paint(juce::Graphics& g) override;
    void resized() override;

	void selectItemsInBox(const juce::Rectangle<double>& selectionBox) override;
	void changeListenerCallback(juce::ChangeBroadcaster* source) override;
	te::SelectedMidiEvents& getSelectedEvents() { return *selectedEvents; }
    te::SelectedMidiEvents& getSelectedEvents(bool selectCursorIfEmpty) { 
        if (selectCursorIfEmpty && selectedEvents->getSelectedNotes().isEmpty())
        {
            auto cursorPosition = cursorComponent.getCursorPosition();
            selectItemsInBox({cursorPosition.getX(), cursorPosition.getY(), 0, 0});
        }
        
        return getSelectedEvents();
    }
	ExtendedMidiClip& getMidiClip() { return midiClip; }

    /**
     * @name Overrides from fumeUI::Grid
     */
     //@{
    juce::Point<double> getGridSize() const override;
    juce::Point<double> getUnitSize() const override { return { fumeUI::midiEditorBeatWidth, static_cast<double>(fumeUI::midiEditorNoteHeight) }; }
    juce::Point<double> getCellSize() const override { return { 0.25, 1 }; }
    int getCellGroupSize() const override { return 4; }
    double getSnappingInterval() const override { return 0.25; }
    //@}

	te::SelectedMidiEvents& clearSelectedEvents();

private:
    ExtendedMidiClip midiClip;
    juce::ValueTree state;
	std::unique_ptr<te::SelectedMidiEvents> selectedEvents { std::make_unique<te::SelectedMidiEvents>(midiClip.getClip()) };
    
    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property) override;
    void valueTreeChildAdded(juce::ValueTree& parentTree, juce::ValueTree& childWhichHasBeenAdded) override;
    void valueTreeChildRemoved(juce::ValueTree& parentTree, juce::ValueTree& childWhichHasBeenRemoved, int index) override;
    void valueTreeChildOrderChanged(juce::ValueTree& parentTree, int oldIndex, int newIndex) override;
    void valueTreeParentChanged(juce::ValueTree& treeWhoseParentHasChanged) override;

    void handleAsyncUpdate() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiEventsEditorComponent)
};
