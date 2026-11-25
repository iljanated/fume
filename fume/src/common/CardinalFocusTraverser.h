#pragma once

#include <JuceHeader.h>
#include "UIContext.h"

namespace fumeUI
{
	class CardinalFocusTraverser : public juce::ComponentTraverser
	{
	public:
		CardinalFocusTraverser(fumeUI::UIContext& uiContext, juce::Component* rootComponent);
		juce::Component* getDefaultComponent(juce::Component* parentComponent) override;
		juce::Component* getNextComponent(juce::Component* current) override;
		juce::Component* getPreviousComponent(juce::Component* current) override;
		std::vector<juce::Component*> getAllComponents(juce::Component* parentComponent) override;
		juce::Component* getNextComponent(juce::Component* current, juce::Point<int> delta);
		bool keyPressed(const juce::KeyPress& key);

	private:
		juce::KeyboardFocusTraverser keyboardTraverser;
		juce::Component* rootComponent = nullptr;
		fumeUI::UIContext& uiContext;

		juce::Component* getCurrentComponent();

		juce::Point<int> getCenter(juce::Component* component, juce::Component* parentComponent) const;
	};
}
