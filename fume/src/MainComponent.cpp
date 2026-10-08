#pragma once
#include "MainComponent.h"
#include "Utilities.h"

namespace te = tracktion;
using namespace tracktion::literals;

MainComponent::MainComponent()
	: engine("Fume")
{
	setSize(500, 450);

	// 1. Haal de JUCE AudioDeviceManager op uit de Tracktion Engine
	auto& deviceManager = engine.getDeviceManager().deviceManager;

	// Initialiseer de geluidskaart met standaardinstellingen (2 inputs, 2 outputs)
	deviceManager.initialiseWithDefaultDevices(0, 2);

	loadFile();
}

MainComponent::~MainComponent()
{
	removeKeyListener(&inputManager);
}

void MainComponent::parentHierarchyChanged()
{
	if (auto* peer = getPeer())
	{
		peer->getComponent().removeKeyListener(&inputManager);
		peer->getComponent().addKeyListener(&inputManager);
	}
}

void MainComponent::modifierKeysChanged(const ModifierKeys& modifiers)
{
	inputManager.modifierKeysChanged(modifiers.getRawFlags());
}

void MainComponent::paint(juce::Graphics& g)
{
	g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void MainComponent::resized()
{
	auto localBounds = getLocalBounds();
	if (editContainerComponent != nullptr)
	{
		editContainerComponent->setBounds(localBounds);
	}
}

void MainComponent::loadFile()
{
	editContainerComponent = nullptr;
	edit = nullptr;

	juce::File userFolder = juce::File::getSpecialLocation(juce::File::SpecialLocationType::userDocumentsDirectory);
	juce::File editFile = userFolder.getChildFile("demo_edit.fum");

	if (editFile.existsAsFile())
	{
		edit = te::loadEditFromFile(engine, editFile);
	}
	else
	{
		edit = te::createEmptyEdit(engine, editFile);
	}

	editContainerComponent = std::make_unique<EditContainerComponent>(*edit, inputManager);
	addAndMakeVisible(editContainerComponent.get());
	resized();
}	
