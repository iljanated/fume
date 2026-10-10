#pragma once
#include <JuceHeader.h>
#include "TrackComponent.h"
#include "Utilities.h"
#include "ToolState.h"

namespace te = tracktion;

class TrackListComponent : public juce::Component,
    public InputManagerListener,
    private te::ValueTreeObjectList<Helpers::AsyncValueTreeItem<TrackComponent>>,
    private juce::Timer,
    public juce::ChangeListener,
	public juce::ComponentListener,
    public ToolStateListener
{
public:
    //==============================================================================
    TrackListComponent(fumeUI::UIContext& context, juce::Viewport& v);

    ~TrackListComponent() override;

    //==============================================================================
    void paint(juce::Graphics& g) override;

    void resized() override;

    void onInputAction(InputManagerActionId actionId, int inputMask, bool isActive) override;

    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

	juce::DrawableRectangle& getDrawableCursor() { return drawableCursor; }
	juce::Point<double>& getCursor() { return cursor; }
	juce::Point<double>& getCursorAnchor() { return cursorAnchor; }
	int getTrackCount() const { return objects.size(); }
	te::AudioTrack* getTrackAt(int index) const;

    void componentMovedOrResized(Component& component, bool wasMoved, bool wasResized) override;

	void onToolStateFinished() override;
private:
    //==============================================================================
	fumeUI::UIContext& uiContext;
    Viewport& viewport;
    DrawableRectangle currentPositionMarker;
    DrawableRectangle drawableCursor;
    juce::Point<double> cursor;
    juce::Point<double> cursorAnchor;
    RectangleList<float> gridBackgroundsLight;
    RectangleList<float> gridBackgroundsDark;
	Path gridLines;
	std::unique_ptr<ToolState<TrackListComponent>> selectToolState;
    std::unique_ptr<ToolState<TrackListComponent>> writeToolState;
    std::unique_ptr<ToolState<TrackListComponent>> deleteToolState;
    ToolState<TrackListComponent>* currentToolState{ nullptr };

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

	void updatePaths();

    void resizeViewport();

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackListComponent)
};
