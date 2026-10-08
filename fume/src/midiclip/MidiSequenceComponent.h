#pragma once
#include <JuceHeader.h>

class MidiSequenceComponent : public juce::Component, private juce::ValueTree::Listener
{
public:
    MidiSequenceComponent();
    ~MidiSequenceComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
	void visibilityChanged() override;  

    void setMidiClip(te::MidiClip* newClip);

private:
    juce::ValueTree clipState;

    void valueTreeParentChanged(juce::ValueTree& treeWhoseParentHasChanged) override;
    void valueTreePropertyChanged(juce::ValueTree&, const juce::Identifier&) override { repaint(); }
    void valueTreeChildAdded(juce::ValueTree&, juce::ValueTree&) override { repaint(); }
    void valueTreeChildRemoved(juce::ValueTree&, juce::ValueTree&, int) override { repaint(); }
    void valueTreeChildOrderChanged(juce::ValueTree&, int, int) override { repaint(); }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiSequenceComponent)
};
