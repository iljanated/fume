#pragma once
#include <JuceHeader.h>
#include "TrackComponent.h"
#include "Utilities.h"
#include "ToolState.h"
#include "CursorComponent.h"
#include "SelectionBoxComponent.h"

namespace te = tracktion;

class TrackListComponent : public juce::Component,
    private te::ValueTreeObjectList<Helpers::AsyncValueTreeItem<TrackComponent>>,
	private juce::Timer,
    public juce::ChangeListener,
    public juce::ComponentListener,
	public SelectionBoxSource,
	public fumeUI::Grid
{
public:
    //==============================================================================
    TrackListComponent(fumeUI::UIContext& context, juce::Viewport& v);

    ~TrackListComponent() override;

    //==============================================================================
    void paint(juce::Graphics& g) override;

    // callbacks

    void resized() override;
    void focusGained(FocusChangeType cause) override;
	void focusLost(FocusChangeType cause) override;
	bool keyPressed(const juce::KeyPress& key) override;
	bool keyStateChanged(bool isKeyDown) override;
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;
    void componentMovedOrResized(Component& component, bool wasMoved, bool wasResized) override;
	void selectItemsInBox(const juce::Rectangle<double>& selectionBox) override;

    // tracks
	int getTrackCount() const { return objects.size(); }
	te::AudioTrack* getTrackAt(int index) const;

	CursorComponent& getCursorComponent() { return cursorComponent; }
	SelectionBoxComponent& getSelectionBoxComponent() { return selectionBoxComponent; }

	fumeUI::UIContext& getUIContext() { return uiContext; }
    
    /**
	 * @name Overrides from fumeUI::Grid
     */
    //@{
    juce::Point<double> getGridSize() const override;
	juce::Point<double> getUnitSize() const override { return { FUME_TIMELINE_BEAT_WIDTH, FUME_TIMELINE_TRACK_HEIGHT }; }
	juce::Point<int> getCellSize() const override { return { 4, 1 }; }
	int getCellGroupSize() const override { return 4; }
    double getSnappingInterval() const override { return 1.0; }
    //@}

private:
    //==============================================================================
	fumeUI::UIContext& uiContext;
    Viewport& viewport;
    DrawableRectangle currentPositionMarker;
	CursorComponent cursorComponent{ *this, 2, uiContext.inputManager };
	SelectionBoxComponent selectionBoxComponent{ *this, 1, this, uiContext.inputManager };
	ToolStateManager<TrackListComponent> toolStateManager{ uiContext.inputManager };
    Helpers::AsyncResizer asyncResizer{ *this };

	void activateToolState(ToolState<TrackListComponent>* newState);

    bool isSuitableType(const juce::ValueTree& v) const override;
    Helpers::AsyncValueTreeItem<TrackComponent>* createNewObject(const juce::ValueTree& v) override;
    void deleteObject(Helpers::AsyncValueTreeItem<TrackComponent>* obj) override;
    void newObjectAdded(Helpers::AsyncValueTreeItem<TrackComponent>*) override;
    void objectRemoved(Helpers::AsyncValueTreeItem<TrackComponent>*) override;
    void objectOrderChanged() override;

    void timerCallback() override;

	void updateCurrentPositionMarker();

    void resizeViewport();

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackListComponent)
};
