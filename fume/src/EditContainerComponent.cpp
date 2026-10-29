#pragma once
#include "EditContainerComponent.h"
#include "Utilities.h"
#include "ExtendedEdit.h"
namespace te = tracktion;
using namespace tracktion::literals;

EditContainerComponent::EditContainerComponent(te::Engine& engine, te::Edit& edit, te::SelectionManager& selectionManager, InputManager& inputManager)
	: extendedEdit{ edit, &edit.getUndoManager() },  uiContext {engine, extendedEdit, selectionManager, inputManager }
{
	addAndMakeVisible(trackListContainerComponent);

	addAndMakeVisible(transportComponent);

	addAndMakeVisible(clipEditorComponent);
	
	edit.state.setProperty(FumeIDs::SongLength, 1024, &edit.getUndoManager());

	EngineHelpers::getOrInsertAudioTrackAt(edit, 16);

	
	auto synthPlugin = dynamic_cast<te::FourOscPlugin*> (edit.getPluginCache().createNewPlugin(te::FourOscPlugin::xmlTypeName, {}).get());
	auto track = te::getAudioTracks(edit)[4];
	track->pluginList.insertPlugin(synthPlugin, 0, nullptr);

	te::TempoSequence& tempoSequence = edit.tempoSequence;

	auto midiClip = track->insertMIDIClip(tempoSequence.toTime({ 0_bp, 16_bp }), nullptr);

	auto& seq = midiClip->getSequence();
	seq.addNote(48, 0_bp, 4_bd, 127, 0, nullptr);
	seq.addNote(41, 4_bp, 4_bd, 100, 0, nullptr);
	seq.addNote(59, 8_bp, 4_bd, 110, 0, nullptr);
	seq.addNote(38, 12_bp, 4_bd, 100, 0, nullptr);

	auto track2 = te::getAudioTracks(edit)[3];


	auto midiClip2 = track2->insertMIDIClip(tempoSequence.toTime({ 16_bp, 32_bp }), nullptr);

	auto& seq2 = midiClip2->getSequence();
	seq2.addNote(66, 0_bp, 4_bd, 127, 0, nullptr);
	seq2.addNote(41, 4_bp, 4_bd, 100, 0, nullptr);
	seq2.addNote(59, 8_bp, 4_bd, 110, 0, nullptr);
	seq2.addNote(38, 12_bp, 4_bd, 100, 0, nullptr);
	

	edit.getUndoManager().clearUndoHistory();

	// Start de transport direct
	edit.getTransport().play(false);
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
	auto& edit = extendedEdit.getEdit();
	auto keyCode = key.getKeyCode();
	auto modifiers = key.getModifiers();
	auto& inputManager = uiContext.inputManager;
	if (inputManager.validate(keyCode, modifiers, InputAction::Undo))
	{
		edit.getUndoManager().undo();
		return true;
	}
	else if (inputManager.validate(keyCode, modifiers, InputAction::Redo))
	{
		edit.getUndoManager().redo();
		return true;
	}
	else if (inputManager.validate(keyCode, modifiers, InputAction::Play))
	{
		if (edit.getTransport().isPlaying())
		{
			edit.getTransport().stop(false, false);
		}
		else
		{
			edit.getTransport().play(false);
		}
		return true;
	}
	else if (inputManager.validate(keyCode, modifiers, InputAction::PlayFromStart))
	{
		edit.getTransport().playFromStart(false);
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
