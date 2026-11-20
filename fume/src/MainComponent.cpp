#pragma once
#include "MainComponent.h"
#include "Utilities.h"

namespace te = tracktion;
using namespace tracktion::literals;

MainComponent::MainComponent()
	: engine("Fume")
{
	setSize(1280, 720);

	// 1. Haal de JUCE AudioDeviceManager op uit de Tracktion Engine
	auto& deviceManager = engine.getDeviceManager().deviceManager;

	// Initialiseer de geluidskaart met standaardinstellingen (2 inputs, 2 outputs)
	deviceManager.initialiseWithDefaultDevices(0, 2);

	addAndMakeVisible(optionsComponent);
	optionsComponent.setVisible(false);

	editManager.addListener(this);
	editManager.setAutoSaveEnabled(true);
	editManager.createOrLoadNewEdit(juce::File::getSpecialLocation(juce::File::SpecialLocationType::userDocumentsDirectory).getChildFile("demo_edit.fum"));
}

MainComponent::~MainComponent()
{
}

void MainComponent::paint(juce::Graphics& g)
{
	g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void MainComponent::resized()
{
	auto localBounds = getLocalBounds();
	optionsComponent.setBounds(localBounds);
	if (editContainerComponent != nullptr)
	{
		editContainerComponent->setBounds(localBounds);
	}
}

bool MainComponent::keyPressed(const juce::KeyPress& key)
{
	auto keyCode = key.getKeyCode();
	auto modifiers = key.getModifiers();
	if (inputManager.validate(keyCode, modifiers, InputAction::Options))
	{
		if (optionsComponent.isVisible())
		{
			optionsComponent.setVisible(false);
			if (editContainerComponent != nullptr)
			{
				editContainerComponent->setVisible(true);
				editContainerComponent->grabKeyboardFocus();
			}
		}
		else
		{
			if (editContainerComponent != nullptr)
			{
				editContainerComponent->setVisible(false);
			}
			optionsComponent.setVisible(true);
			optionsComponent.grabKeyboardFocus();
		}
		return true;
	}
	if (inputManager.validate(keyCode, modifiers, InputAction::Save))
	{
		editManager.saveEdit();
		return true;
	}

	return false;
}

void MainComponent::onEditUnloaded()
{
	selectionManager.deselectAll();
	editContainerComponent = nullptr;
}

void MainComponent::onEditLoaded(te::Edit* newEdit)
{
	optionsComponent.setVisible(false);
	edit = newEdit;
	editContainerComponent = std::make_unique<EditContainerComponent>(engine, *edit, selectionManager, inputManager);
	addAndMakeVisible(editContainerComponent.get());
	resized();
}

void MainComponent::systemRequestedToQuit()
{
	int counter = 0;
	if (editManager.isSaveInProgress() && counter < 5)
	{
		DBG("Waiting for save to complete before quitting...");
		juce::Thread::sleep(1000);
		counter++;
	}

	jassert(editManager.isSaveInProgress() == false);

}
