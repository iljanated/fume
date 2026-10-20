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

    /**
     * @name Overrides from fumeUI::Grid
     */
     //@{
    juce::Point<double> getGridSize() const override;
    juce::Point<double> getUnitSize() const override { return { FUME_MIDI_EDITOR_BEAT_WIDTH, FUME_MIDI_EDITOR_NOTE_HEIGHT }; }
    juce::Point<double> getCellSize() const override { return { 0.25, 1 }; }
    int getCellGroupSize() const override { return 4; }
    double getSnappingInterval() const override { return 0.25; }
    //@}

private:
    ExtendedMidiClip midiClip;
    juce::ValueTree state;
	te::SelectedMidiEvents selectedEvents { midiClip.getClip() };
    
    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property) override;
    void valueTreeChildAdded(juce::ValueTree& parentTree, juce::ValueTree& childWhichHasBeenAdded) override;
    void valueTreeChildRemoved(juce::ValueTree& parentTree, juce::ValueTree& childWhichHasBeenRemoved, int index) override;
    void valueTreeChildOrderChanged(juce::ValueTree& parentTree, int oldIndex, int newIndex) override;
    void valueTreeParentChanged(juce::ValueTree& treeWhoseParentHasChanged) override;

    void handleAsyncUpdate() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiEventsEditorComponent)
};
