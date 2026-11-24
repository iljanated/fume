#include "CardinalFocusTraverser.h"

#include <cmath>
#include <limits>

namespace fumeUI
{
	CardinalFocusTraverser::CardinalFocusTraverser(juce::Component* rootComponent)
		: rootComponent(rootComponent)
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
}
