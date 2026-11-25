#include "CardinalFocusTraverser.h"

#include <cmath>
#include <limits>

namespace fumeUI
{
	CardinalFocusTraverser::CardinalFocusTraverser(fumeUI::UIContext& uiContext, juce::Component* rootComponent)
		: uiContext(uiContext), rootComponent(rootComponent)
	{
	}

	juce::Component* CardinalFocusTraverser::getDefaultComponent(juce::Component* parentComponent)
	{
		return keyboardTraverser.getDefaultComponent(parentComponent);
	}

	juce::Component* CardinalFocusTraverser::getNextComponent(juce::Component* current)
	{
		return getNextComponent(current, { 1, 0 });
	}

	juce::Component* CardinalFocusTraverser::getPreviousComponent(juce::Component* current)
	{
		return getNextComponent(current, { -1, 0 });
	}

	std::vector<juce::Component*> CardinalFocusTraverser::getAllComponents(juce::Component* parentComponent)
	{
		return keyboardTraverser.getAllComponents(parentComponent);
	}

	juce::Component* CardinalFocusTraverser::getNextComponent(juce::Component* current, juce::Point<int> delta)
	{

		// return default component if current is nullptr
		if (current == nullptr)
		{
			return getDefaultComponent(current);
		}

		// return current if delta is zero
		delta.x = juce::jlimit(-1, 1, delta.x);
		delta.y = juce::jlimit(-1, 1, delta.y);

		if (delta == juce::Point<int>())
		{
			return current;
		}

		auto allComponents = getAllComponents(rootComponent);
		
		auto cost = std::numeric_limits<int>::max();
		auto bestCandidate = current;
		auto center = getCenter(bestCandidate, rootComponent);

		DBG(allComponents.size());

		for (auto* candidate : allComponents)
		{
			if (candidate == current)
			{
				continue;
			}

			auto candidateCenter = getCenter(candidate, rootComponent);

			auto candidateDelta = candidateCenter - center;

			if (delta.x > 0 && candidateDelta.x <= 0)
			{
				continue;
			}
			else if (delta.x < 0 && candidateDelta.x >= 0)
			{
				continue;
			}
			else if (delta.y > 0 && candidateDelta.y <= 0)
			{
				continue;
			}
			else if (delta.y < 0 && candidateDelta.y >= 0)
			{
				continue;
			}

			if (delta.x == 0)
			{
				candidateDelta.x = candidateDelta.x * 3;
			}

			if (delta.y == 0)
			{
				candidateDelta.y = candidateDelta.y * 3;
			}

			auto candidateCost = std::abs(candidateDelta.x) + std::abs(candidateDelta.y);

			if (candidateCost < cost)
			{
				cost = candidateCost;
				bestCandidate = candidate;
			}
		}

		return  bestCandidate;
	}

	bool CardinalFocusTraverser::keyPressed(const juce::KeyPress& key)
	{
		auto keyCode = key.getKeyCode();
		auto modifiers = key.getModifiers();
		auto& inputManager = uiContext.inputManager;
		
		if (inputManager.validate(keyCode, modifiers, InputAction::MoveLeft))
		{
			auto current = getCurrentComponent();
			auto next = getNextComponent(current, { -1, 0 });

			if (next != nullptr)
			{
				next->grabKeyboardFocus();
			}
			return true;
		}
		if (inputManager.validate(keyCode, modifiers, InputAction::MoveRight))
		{
			auto current = getCurrentComponent();
			auto next = getNextComponent(current, { 1, 0 });

			if (next != nullptr)
			{
				next->grabKeyboardFocus();
			}
			return true;
		}
		if (inputManager.validate(keyCode, modifiers, InputAction::MoveUp))
		{
			auto current = getCurrentComponent();
			auto next = getNextComponent(current, { 0, -1 });

			if (next != nullptr)
			{
				next->grabKeyboardFocus();
			}
			return true;
		}
		if (inputManager.validate(keyCode, modifiers, InputAction::MoveDown))
		{
			auto current = getCurrentComponent();
			auto next = getNextComponent(current, { 0, 1 });

			if (next != nullptr)
			{
				next->grabKeyboardFocus();
			}
			return true;
		}
		return false;
	}

	juce::Component* CardinalFocusTraverser::getCurrentComponent()
	{
		auto* current = juce::Component::getCurrentlyFocusedComponent();

		if (current == nullptr || (!rootComponent->isParentOf(current) && current != rootComponent))
		{
			current = getDefaultComponent(rootComponent);
		}
		return current;
	}

	juce::Point<int> CardinalFocusTraverser::getCenter(juce::Component* component, juce::Component* parentComponent) const
	{
		auto localCentre = component->getLocalBounds().getCentre();
		return parentComponent->getLocalPoint(component, localCentre);
	}
}
