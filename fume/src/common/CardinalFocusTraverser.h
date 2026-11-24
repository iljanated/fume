#pragma once

#include <JuceHeader.h>

namespace fumeUI
{
	class CardinalFocusTraverser : public juce::ComponentTraverser
	{
	public:
		CardinalFocusTraverser(juce::Component* rootComponent);
		juce::Component* getDefaultComponent(juce::Component* parentComponent) override;
		juce::Component* getNextComponent(juce::Component* current) override;
		juce::Component* getPreviousComponent(juce::Component* current) override;
		std::vector<juce::Component*> getAllComponents(juce::Component* parentComponent) override;
		juce::Component* getNextComponent(juce::Component* current, juce::Point<int> delta);

	private:
		juce::KeyboardFocusTraverser keyboardTraverser;
		juce::Component* rootComponent = nullptr;

		juce::Point<int> getCenter(juce::Component* component, juce::Component* parentComponent) const
		{
			auto localCentre = component->getLocalBounds().getCentre();
			return parentComponent->getLocalPoint(component, localCentre);
		}
	};
}
