#pragma once

#include  <JuceHeader.h>

class CarrouselComponent : public juce::Component
{
public:
	CarrouselComponent()
	{
	}

	void resized() override
	{
		for (auto* component : components)
		{
			component->setBounds(getLocalBounds());
		}
	}

	void addComponent(juce::Component* component)
	{
		components.push_back(component);
		addAndMakeVisible(component);
		component->setBounds(getLocalBounds());
		component->setVisible(false);
	}

	void showNextComponent()
	{
		int nextIndex = (currentComponentIndex + 1) % components.size();
		showComponent(nextIndex);
	}

	void showPreviousComponent()
	{
		int prevIndex = (currentComponentIndex - 1 + components.size()) % components.size();
		showComponent(prevIndex);
	}

	void showComponent(int index)
	{
		if (index != currentComponentIndex)
		{
			if (currentComponentIndex >= 0 && currentComponentIndex < components.size())
			{
				components[currentComponentIndex]->setVisible(false);
			}
		}
		if (index >= 0 && index < components.size())
		{
			currentComponentIndex = index;
			components[currentComponentIndex]->setVisible(true);
			juce::MessageManager::callAsync([this] { components[currentComponentIndex]->grabKeyboardFocus(); });
		}
	}
private:
	std::vector<juce::Component*> components;
	int currentComponentIndex = -1;
};