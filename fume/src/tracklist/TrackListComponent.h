#pragma once
#include <JuceHeader.h>
#include "TrackComponent.h"
#include "UIContext.h"
#include "ToolState.h"
#include "TrackListCursorComponent.h"

namespace te = tracktion;

class TrackListComponent : public juce::Component,
    private te::ValueTreeObjectList<Helpers::AsyncValueTreeItem<TrackComponent>>,
	private juce::Timer,
    public juce::ChangeListener
{
public:
    //==============================================================================
    TrackListComponent(fumeUI::UIContext& context, juce::Viewport& v);

    ~TrackListComponent() override;

    //==============================================================================
    void paint(juce::Graphics& g) override;

    // callbacks

    void resized() override;
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;
    void focusGained(FocusChangeType cause) override;
    void focusLost(FocusChangeType cause) override;
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

    // tracks
	int getTrackCount() const { return objects.size(); }
	te::AudioTrack* getTrackAt(int index) const;

    void findLassoItemsInArea(Array<te::Clip*>& itemsFound, const Rectangle<int>& area);
	void lassoSelectionChanged(SelectedItemSet<te::Clip*>& newSelection);

	fumeUI::UIContext& getUIContext() { return uiContext; }
	TrackListCursorComponent& getCursorComponent() { return cursorComponent; }
	te::engine::TimecodeSnapType& getSnapType() { return snapType; }
private:
    //==============================================================================
	juce::Viewport& viewport;
    fumeUI::UIContext& uiContext;
	DrawableRectangle currentPositionMarker;
	Helpers::AsyncResizer asyncResizer{ *this };
	te::engine::TimecodeSnapType snapType{ te::engine::TimecodeType::barsBeats, 1 };
	ToolStateManager<TrackListComponent> toolStateManager{ uiContext.inputManager, FumeIDs::TrackListSelectToolState};
	TrackListCursorComponent cursorComponent{ uiContext, *this };

	void activateToolState(ToolState* newState);

    bool isSuitableType(const juce::ValueTree& v) const override;
    Helpers::AsyncValueTreeItem<TrackComponent>* createNewObject(const juce::ValueTree& v) override;
    void deleteObject(Helpers::AsyncValueTreeItem<TrackComponent>* obj) override;
    void newObjectAdded(Helpers::AsyncValueTreeItem<TrackComponent>*) override;
    void objectRemoved(Helpers::AsyncValueTreeItem<TrackComponent>*) override;
    void objectOrderChanged() override;

    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
        const juce::Identifier& property) override;

    void timerCallback() override;

	void updateCurrentPositionMarker();
	void updateSnapType();
	void updateBounds();

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackListComponent)
};
