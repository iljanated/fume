#include "CardinalFocusTraverser.h"

#include <cmath>
#include <limits>

namespace fumeUI
{
	CardinalFocusTraverser::CardinalFocusTraverser(WrappingType wrappingTypeToUse, double perpendicularPenaltyToUse)
		: wrappingType(wrappingTypeToUse), perpendicularPenalty(perpendicularPenaltyToUse)
	{
	}

	void CardinalFocusTraverser::setWrappingType(WrappingType newWrappingType)
	{
		wrappingType = newWrappingType;
	}

	CardinalFocusTraverser::WrappingType CardinalFocusTraverser::getWrappingType() const
	{
		return wrappingType;
	}

	void CardinalFocusTraverser::setPerpendicularPenalty(double newPerpendicularPenalty)
	{
		perpendicularPenalty = newPerpendicularPenalty;
	}

	double CardinalFocusTraverser::getPerpendicularPenalty() const
	{
		return perpendicularPenalty;
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
		if (current == nullptr)
		{
			return nullptr;
		}

		delta.x = juce::jlimit(-1, 1, delta.x);
		delta.y = juce::jlimit(-1, 1, delta.y);

		if (delta == juce::Point<int>())
		{
			return current;
		}

		auto* scopeRoot = current->getParentComponent();
		if (scopeRoot == nullptr)
		{
			scopeRoot = current->findFocusContainer();
		}

		if (scopeRoot == nullptr)
		{
			return nullptr;
		}

		auto allComponents = getAllComponents(scopeRoot);
		std::vector<juce::Component*> candidateComponents;
		candidateComponents.reserve(allComponents.size());

		for (auto* candidateComponent : allComponents)
		{
			if (candidateComponent != nullptr && candidateComponent->getParentComponent() == scopeRoot)
			{
				candidateComponents.push_back(candidateComponent);
			}
		}

		if (candidateComponents.empty())
		{
			return nullptr;
		}

		const auto currentCentre = current->getScreenBounds().getCentre();

		if (auto* bestDirectional = findBestCandidate(currentCentre, delta, candidateComponents, true))
		{
			return bestDirectional;
		}

		if (wrappingType == WrappingType::NoWrap)
		{
			return nullptr;
		}

		if (wrappingType == WrappingType::SnakeWrapping)
		{
			if (auto* snakeWrapped = getSnakeWrappedComponent(current, delta, candidateComponents))
			{
				return snakeWrapped;
			}
		}

		return getWrappedComponent(current, delta, candidateComponents);
	}

	juce::Component* CardinalFocusTraverser::getWrappedComponent(juce::Component* current,
										 juce::Point<int> delta,
										 const std::vector<juce::Component*>& components) const
	{
		const auto currentCentre = current->getScreenBounds().getCentre();
		juce::Component* bestComponent = nullptr;
		double bestPerpendicularOffset = std::numeric_limits<double>::max();
		double bestParallelDistance = std::numeric_limits<double>::max();

		auto shouldSelectCandidate = [&bestPerpendicularOffset, &bestParallelDistance](double perpendicularOffset, double parallelDistance)
		{
			if (perpendicularOffset < bestPerpendicularOffset)
			{
				bestPerpendicularOffset = perpendicularOffset;
				bestParallelDistance = parallelDistance;
				return true;
			}

			if (perpendicularOffset == bestPerpendicularOffset && parallelDistance < bestParallelDistance)
			{
				bestParallelDistance = parallelDistance;
				return true;
			}

			return false;
		};

		if (delta.x != 0)
		{
			int edgeX = (delta.x > 0) ? std::numeric_limits<int>::max() : std::numeric_limits<int>::min();

			for (auto* candidateComponent : components)
			{
				if (candidateComponent == nullptr || candidateComponent == current)
				{
					continue;
				}

				auto centreX = candidateComponent->getScreenBounds().getCentreX();
				edgeX = (delta.x > 0) ? std::min(edgeX, centreX) : std::max(edgeX, centreX);
			}

			for (auto* candidateComponent : components)
			{
				if (candidateComponent == nullptr || candidateComponent == current)
				{
					continue;
				}

				auto candidateCentre = candidateComponent->getScreenBounds().getCentre();
				if (candidateCentre.x != edgeX)
				{
					continue;
				}

				double perpendicularOffset = std::abs(static_cast<double>(candidateCentre.y - currentCentre.y));
				double parallelDistance = std::abs(static_cast<double>(candidateCentre.x - currentCentre.x));

				if (shouldSelectCandidate(perpendicularOffset, parallelDistance))
				{
					bestComponent = candidateComponent;
				}
			}
		}
		else if (delta.y != 0)
		{
			int edgeY = (delta.y > 0) ? std::numeric_limits<int>::max() : std::numeric_limits<int>::min();

			for (auto* candidateComponent : components)
			{
				if (candidateComponent == nullptr || candidateComponent == current)
				{
					continue;
				}

				auto centreY = candidateComponent->getScreenBounds().getCentreY();
				edgeY = (delta.y > 0) ? std::min(edgeY, centreY) : std::max(edgeY, centreY);
			}

			for (auto* candidateComponent : components)
			{
				if (candidateComponent == nullptr || candidateComponent == current)
				{
					continue;
				}

				auto candidateCentre = candidateComponent->getScreenBounds().getCentre();
				if (candidateCentre.y != edgeY)
				{
					continue;
				}

				double perpendicularOffset = std::abs(static_cast<double>(candidateCentre.x - currentCentre.x));
				double parallelDistance = std::abs(static_cast<double>(candidateCentre.y - currentCentre.y));

				if (shouldSelectCandidate(perpendicularOffset, parallelDistance))
				{
					bestComponent = candidateComponent;
				}
			}
		}

		return bestComponent;
	}

	juce::Component* CardinalFocusTraverser::getSnakeWrappedComponent(juce::Component* current,
										  juce::Point<int> delta,
										  const std::vector<juce::Component*>& components) const
	{
		if (delta.y != 0)
		{
			return getWrappedComponent(current, delta, components);
		}

		const auto currentCentre = current->getScreenBounds().getCentre();
		double bestRowOffset = std::numeric_limits<double>::max();
		std::vector<juce::Component*> rowCandidates;

		for (auto* candidateComponent : components)
		{
			if (candidateComponent == current)
			{
				continue;
			}

			auto candidateCentre = candidateComponent->getScreenBounds().getCentre();
			double rowOffset = static_cast<double>(candidateCentre.y - currentCentre.y);

			if ((delta.x > 0 && rowOffset <= 0.0) || (delta.x < 0 && rowOffset >= 0.0))
			{
				continue;
			}

			double absoluteRowOffset = std::abs(rowOffset);
			if (absoluteRowOffset < bestRowOffset)
			{
				bestRowOffset = absoluteRowOffset;
				rowCandidates.clear();
				rowCandidates.push_back(candidateComponent);
			}
			else if (absoluteRowOffset == bestRowOffset)
			{
				rowCandidates.push_back(candidateComponent);
			}
		}

		if (rowCandidates.empty())
		{
			return getWrappedComponent(current, delta, components);
		}

		juce::Point<int> horizontalDirection { delta.x > 0 ? 1 : -1, 0 };
		return findBestCandidate(currentCentre, horizontalDirection, rowCandidates, false);
	}

	juce::Component* CardinalFocusTraverser::findBestCandidate(juce::Point<int> origin,
										   juce::Point<int> delta,
										   const std::vector<juce::Component*>& components,
										   bool directionalOnly) const
	{
		auto axisX = static_cast<double>(delta.x);
		auto axisY = static_cast<double>(delta.y);
		auto axisLengthSquared = (axisX * axisX) + (axisY * axisY);

		if (axisLengthSquared <= 0.0)
		{
			return nullptr;
		}

		juce::Component* bestCandidate = nullptr;
		double bestScore = std::numeric_limits<double>::max();

		for (auto* candidateComponent : components)
		{
			if (candidateComponent == nullptr || candidateComponent->getScreenBounds().getCentre() == origin)
			{
				continue;
			}

			auto candidateCentre = candidateComponent->getScreenBounds().getCentre();
			double offsetX = static_cast<double>(candidateCentre.x - origin.x);
			double offsetY = static_cast<double>(candidateCentre.y - origin.y);

			double parallelDot = (offsetX * axisX) + (offsetY * axisY);
			if (directionalOnly && parallelDot <= 0.0)
			{
				continue;
			}

			double parallelDistance = std::abs(parallelDot) / axisLengthSquared;
			double perpendicularDistance = std::abs((offsetX * axisY) - (offsetY * axisX)) / axisLengthSquared;
			double candidateScore = parallelDistance + (perpendicularPenalty * perpendicularDistance);

			if (candidateScore < bestScore)
			{
				bestScore = candidateScore;
				bestCandidate = candidateComponent;
			}
		}

		return bestCandidate;
	}
}
