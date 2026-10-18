#pragma once
#include "EditContainerComponent.h"
#include "Utilities.h"

namespace te = tracktion;
using namespace tracktion::literals;

EditContainerComponent::EditContainerComponent(te::Engine& engine, te::Edit& edit, te::SelectionManager& selectionManager, InputManager& inputManager)
	: uiContext{ engine, edit, selectionManager, inputManager }
{
	addAndMakeVisible(trackListContainerComponent);

	addAndMakeVisible(transportComponent);

	addAndMakeVisible(clipEditorComponent);
	
	uiContext.edit.state.setProperty(FumeIDs::SongLength, 1024, &uiContext.edit.getUndoManager());

	EngineHelpers::getOrInsertAudioTrackAt(uiContext.edit, 16);

	
	auto synthPlugin = dynamic_cast<te::FourOscPlugin*> (uiContext.edit.getPluginCache().createNewPlugin(te::FourOscPlugin::xmlTypeName, {}).get());
	auto track = te::getAudioTracks(uiContext.edit)[4];
	track->pluginList.insertPlugin(synthPlugin, 0, nullptr);

	te::TempoSequence& tempoSequence = uiContext.edit.tempoSequence;

	auto midiClip = track->insertMIDIClip(tempoSequence.toTime({ 0_bp, 16_bp }), nullptr);

	auto& seq = midiClip->getSequence();
	seq.addNote(48, 0_bp, 4_bd, 127, 0, nullptr);
	seq.addNote(41, 4_bp, 4_bd, 100, 0, nullptr);
	seq.addNote(59, 8_bp, 4_bd, 110, 0, nullptr);
	seq.addNote(38, 12_bp, 4_bd, 100, 0, nullptr);

	auto track2 = te::getAudioTracks(uiContext.edit)[3];


	auto midiClip2 = track2->insertMIDIClip(tempoSequence.toTime({ 16_bp, 32_bp }), nullptr);

	auto& seq2 = midiClip2->getSequence();
	seq2.addNote(66, 0_bp, 4_bd, 127, 0, nullptr);
	seq2.addNote(41, 4_bp, 4_bd, 100, 0, nullptr);
	seq2.addNote(59, 8_bp, 4_bd, 110, 0, nullptr);
	seq2.addNote(38, 12_bp, 4_bd, 100, 0, nullptr);
	

	uiContext.edit.getUndoManager().clearUndoHistory();

	// Start de transport direct
	uiContext.edit.getTransport().play(false);
}

EditContainerComponent::~EditContainerComponent()
{
}

void EditContainerComponent::visibilityChanged()
{
	if (isShowing())
	{
		trackListContainerComponent.grabKeyboardFocus();
	}
}	

void EditContainerComponent::resized()
{
	auto localBounds = getLocalBounds();
	transportComponent.setBounds(localBounds.removeFromTop(30));
	trackListContainerComponent.setBounds(localBounds.removeFromTop(400));
	clipEditorComponent.setBounds(localBounds);
}

bool EditContainerComponent::keyPressed(const juce::KeyPress& key)
{
	auto keyCode = key.getKeyCode();
	auto modifiers = key.getModifiers();
	auto& inputManager = uiContext.inputManager;
	if (inputManager.validate(keyCode, modifiers, InputAction::Undo))
	{
		uiContext.edit.getUndoManager().undo();
		return true;
	}
	else if (inputManager.validate(keyCode, modifiers, InputAction::Redo))
	{
		uiContext.edit.getUndoManager().redo();
		return true;
	}
	else if (inputManager.validate(keyCode, modifiers, InputAction::Play))
	{
		if (uiContext.edit.getTransport().isPlaying())
		{
			uiContext.edit.getTransport().stop(false, false);
		}
		else
		{
			uiContext.edit.getTransport().play(false);
		}
		return true;
	}
	else if (inputManager.validate(keyCode, modifiers, InputAction::PlayFromStart))
	{
		uiContext.edit.getTransport().playFromStart(false);
		return true;
	}
	else if (inputManager.validate(keyCode, modifiers, InputAction::NextLeft))
	{
		clipEditorComponent.grabKeyboardFocus();
		return true;
	}
	else if (inputManager.validate(keyCode, modifiers, InputAction::NextRight))
	{
		trackListContainerComponent.grabKeyboardFocus();
		return true;
	}
	DBG("HOME");
	return false;
}
