#pragma once

#include <JuceHeader.h>
#include "MidiClipToolState.h"
#include "ManualLassoComponent.h"

namespace te = tracktion;

class MidiClipSelectToolState : public MidiClipToolState, public LassoSource<te::MidiNote*>, public	juce::ChangeListener
{
public:
	MidiClipSelectToolState(MidiEventsEditorComponent* component);

	~MidiClipSelectToolState() override;

	void onEnterState() override {}

	void onExitState() override {}

	void changeListenerCallback(ChangeBroadcaster* broadcaster) override;

	virtual void findLassoItemsInArea(Array<te::MidiNote*>& itemsFound, const Rectangle<int>& area) override;

	bool canHandle(const juce::KeyPress& key) override;
	
	bool keyPressed(const juce::KeyPress& key) override;

	bool keyStateChanged(bool isKeyDown) override;

	SelectedItemSet<te::MidiNote*>& getLassoSelection() override { return selectedItemSet; }


private:
	juce::Point<double> dragPosition;
	bool isLassoActive = false;
	SelectedItemSet<te::MidiNote*> selectedItemSet;
	ManualLassoComponent<te::MidiNote*> lassoComponent;

	void beginLasso(juce::ModifierKeys modifiers);

	void dragLasso(juce::Point<int> delta, juce::ModifierKeys modifiers);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiClipSelectToolState)
};

