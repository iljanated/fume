#pragma once
#include "EditContainerComponent.h"
#include "Utilities.h"

namespace te = tracktion;
using namespace tracktion::literals;

EditContainerComponent::EditContainerComponent(te::Engine& engine, te::Edit& edit, te::SelectionManager& selectionManager, InputManager& inputManager)
	: uiContext{ engine, edit, selectionManager, inputManager }
{
	uiContext.inputManager.add(this);

	addAndMakeVisible(trackListViewport);
	trackListViewport.setViewedComponent(&trackListComponent, false);
	trackListViewport.setScrollBarsShown(true, true, true, true);
	trackListComponent.setSize(2024, 1600);

	trackListViewport.setWantsKeyboardFocus(false);
	trackListViewport.getHorizontalScrollBar().setMouseClickGrabsKeyboardFocus(false);
	trackListViewport.getVerticalScrollBar().setMouseClickGrabsKeyboardFocus(false);

	addAndMakeVisible(transportComponent);

	addAndMakeVisible(clipEditorComponent);
	
	uiContext.edit.state.setProperty(FumeIDs::songLength, 1024, &uiContext.edit.getUndoManager());

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
	uiContext.inputManager.remove(this);
}

void EditContainerComponent::resized()
{
	auto localBounds = getLocalBounds();
	transportComponent.setBounds(localBounds.removeFromTop(30));
	trackListViewport.setBounds(localBounds.removeFromTop(400));
	clipEditorComponent.setBounds(localBounds);
}

void EditContainerComponent::onInputAction(InputManagerActionId actionId, int inputMask, bool isActive)
{
	if (actionId == InputManagerActionId::UNDO && isActive)
	{
		if (inputMask & static_cast<int>(InputManagerActionId::MOD1))
		{
			uiContext.edit.getUndoManager().redo();
		}
		else
		{
			uiContext.edit.getUndoManager().undo();
		}
	}
}

