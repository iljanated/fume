#pragma once
#include <JuceHeader.h>
#include "Utilities.h"
#include "ClipComponent.h"

namespace te = tracktion;

class TrackComponent : public juce::Component,
    private te::ValueTreeObjectList<Helpers::AsyncValueTreeItem<ClipComponent>>
{
public:
    //==============================================================================
    TrackComponent(te::AudioTrack& t);

    ~TrackComponent() override;

    //==============================================================================
    void paint(juce::Graphics& g) override;

    void resized() override;

    te::AudioTrack& getTrack();

private:
    //==============================================================================
    te::AudioTrack& track;

    Helpers::AsyncResizer asyncResizer{ *this };

    bool isSuitableType(const juce::ValueTree& v) const override;

    Helpers::AsyncValueTreeItem<ClipComponent>* createNewObject(const juce::ValueTree& v) override;

    void deleteObject(Helpers::AsyncValueTreeItem<ClipComponent>* obj) override;

    void newObjectAdded(Helpers::AsyncValueTreeItem<ClipComponent>*) override;
    void objectRemoved(Helpers::AsyncValueTreeItem<ClipComponent>*) override;
    void objectOrderChanged() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackComponent)
};
