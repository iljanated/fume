#pragma once

#include <JuceHeader.h>
#include "Utilities.h"

class TrackHeaderComponent : public juce::Component
{
public:
	TrackHeaderComponent(te::AudioTrack& track, fumeUI::UIContext& context) : track(track), uiContext(context) {}
	~TrackHeaderComponent() override = default;
	void paint(juce::Graphics& g) override;
	void resized() override
	{
	}
private:
	fumeUI::UIContext& uiContext;
    te::AudioTrack& track;
};

class TrackHeadersComponent : public juce::Component,
    private te::ValueTreeObjectList<Helpers::AsyncValueTreeItem<TrackHeaderComponent>>
{
public:
	TrackHeadersComponent(fumeUI::UIContext& context);
    ~TrackHeadersComponent() override;

    void paint(juce::Graphics& g) override;

    void resized() override;

	void setTrackOffset(int offset);

private:
    Helpers::AsyncResizer asyncResizer{ *this };
	fumeUI::UIContext& uiContext;
    int trackOffset = 0;

    bool isSuitableType(const juce::ValueTree& v) const override;
    Helpers::AsyncValueTreeItem<TrackHeaderComponent>* createNewObject(const juce::ValueTree& v) override;
    void deleteObject(Helpers::AsyncValueTreeItem<TrackHeaderComponent>* obj) override;
    void newObjectAdded(Helpers::AsyncValueTreeItem<TrackHeaderComponent>*) override;
    void objectRemoved(Helpers::AsyncValueTreeItem<TrackHeaderComponent>*) override;
    void objectOrderChanged() override;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackHeadersComponent)
};