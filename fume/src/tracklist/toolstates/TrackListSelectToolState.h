#pragma once

#include <JuceHeader.h>
#include "TrackListToolState.h"
#include "ManualLassoComponent.h"

namespace te = tracktion;

class TrackListSelectToolState : public TrackListToolState, public LassoSource<te::Clip*>, public	juce::ChangeListener
{
public:
	TrackListSelectToolState(TrackListComponent* component);

	~TrackListSelectToolState() override;

	void onEnterState() override {}

	void onExitState() override {}

	void changeListenerCallback(ChangeBroadcaster* broadcaster) override;

	virtual void findLassoItemsInArea(Array<te::Clip*>& itemsFound, const Rectangle<int>& area) override;

	bool canHandle(const juce::KeyPress& key) override;

	bool keyPressed(const juce::KeyPress& key) override;

	bool keyStateChanged(bool isKeyDown) override;

	SelectedItemSet<te::Clip*>& getLassoSelection() override { return selectedItemSet; }


private:
	juce::Point<double> dragPosition;
	bool isLassoActive = false;
	SelectedItemSet<te::Clip*> selectedItemSet;
	ManualLassoComponent<te::Clip*> lassoComponent;

	void beginLasso(juce::ModifierKeys modifiers);

	void dragLasso(juce::Point<int> delta, juce::ModifierKeys modifiers);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackListSelectToolState)
};

