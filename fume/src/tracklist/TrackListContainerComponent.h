#pragma once

#include "ManualViewport.h"
#include "TrackHeadersComponent.h"
#include "TrackListComponent.h"

class TrackListContainerComponent : public Component, private juce::ComponentListener
{
public:
	TrackListContainerComponent(fumeUI::UIContext& context);
	~TrackListContainerComponent() override;
	void paint(juce::Graphics& g) override;
	void resized() override;
	void focusGained(FocusChangeType cause) override { trackListComponent.grabKeyboardFocus(); }
	void focusLost(FocusChangeType cause) override {}

private:
	void componentMovedOrResized(juce::Component& component, bool wasMoved, bool wasResized) override;

	TrackHeadersComponent trackHeadersComponent;
	ManualViewport trackListViewport;
	TrackListComponent trackListComponent;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackListContainerComponent)
};