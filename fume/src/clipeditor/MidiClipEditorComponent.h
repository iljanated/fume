#pragma once

#include <JuceHeader.h>
#include "Utilities.h"
#include "ClipEditorImplementationComponent.h"

class MidiClipEditorComponent : public ClipEditorImplementationComponent,
	public fumeUI::Grid,
    private juce::ValueTree::Listener,
    private juce::AsyncUpdater
{
	public:
    MidiClipEditorComponent(te::MidiClip* clip, fumeUI::UIContext& context);
	~MidiClipEditorComponent();
	void paint(juce::Graphics& g) override;
    te::Clip* getClip() override;
	void resized() override;


    /**
	 * @name Overrides from fumeUI::Grid
     */
    //@{
    juce::Point<double> getGridSize() const override;
	juce::Point<double> getUnitSize() const override { return { FUME_MIDI_EDITOR_BEAT_WIDTH, FUME_MIDI_EDITOR_NOTE_HEIGHT }; }
	juce::Point<int> getCellSize() const override { return { 1, 1 }; }
	int getCellGroupSize() const override { return 4; }
    double getSnappingInterval() const override { return 1.0; }
    //@}

private:
    te::MidiClip* midiClip;
    juce::ValueTree state;
    fumeUI::UIContext& uiContext;

private:
    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property) override;
    void valueTreeChildAdded(juce::ValueTree& parentTree, juce::ValueTree& childWhichHasBeenAdded) override;
    void valueTreeChildRemoved(juce::ValueTree& parentTree, juce::ValueTree& childWhichHasBeenRemoved, int index) override;
    void valueTreeChildOrderChanged(juce::ValueTree& parentTree, int oldIndex, int newIndex) override;
    void valueTreeParentChanged(juce::ValueTree& treeWhoseParentHasChanged) override;

    void handleAsyncUpdate() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiClipEditorComponent)
};
