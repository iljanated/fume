#pragma once

#include <JuceHeader.h>

namespace fumeUI
{
	class CardinalFocusTraverser : public juce::ComponentTraverser
	{
	public:
		enum class WrappingType
		{
			NoWrap,
			Wrapping,
			SnakeWrapping
		};

		explicit CardinalFocusTraverser(WrappingType wrappingType = WrappingType::NoWrap,
								   double perpendicularPenalty = 2.5);

		void setWrappingType(WrappingType newWrappingType);
		WrappingType getWrappingType() const;

		void setPerpendicularPenalty(double newPerpendicularPenalty);
		double getPerpendicularPenalty() const;

		juce::Component* getDefaultComponent(juce::Component* parentComponent) override;
		juce::Component* getNextComponent(juce::Component* current) override;
		juce::Component* getPreviousComponent(juce::Component* current) override;
		std::vector<juce::Component*> getAllComponents(juce::Component* parentComponent) override;

		juce::Component* getNextComponent(juce::Component* current, juce::Point<int> delta);

	private:
		juce::Component* getWrappedComponent(juce::Component* current, juce::Point<int> delta, const std::vector<juce::Component*>& components) const;
		juce::Component* getSnakeWrappedComponent(juce::Component* current, juce::Point<int> delta, const std::vector<juce::Component*>& components) const;
		juce::Component* findBestCandidate(juce::Point<int> origin,
							  juce::Point<int> delta,
							  const std::vector<juce::Component*>& components,
							  bool directionalOnly) const;

		WrappingType wrappingType = WrappingType::NoWrap;
		double perpendicularPenalty = 2.5;
		juce::KeyboardFocusTraverser keyboardTraverser;
	};
}
