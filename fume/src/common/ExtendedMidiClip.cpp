#include "ExtendedMidiClip.h"

ExtendedMidiClip::ExtendedMidiClip(te::MidiClip* c)
	: clip(c),
	  cursorQuantisation(c->state.getOrCreateChildWithName(FumeIDs::CursorQuantisation, c->getUndoManager()), c->getUndoManager())
{
	jassert(clip != nullptr);
	zoomX.referTo(clip->state, FumeIDs::ZoomX, nullptr, 1.0f);
	zoomY.referTo(clip->state, FumeIDs::ZoomY, nullptr, 1.0f);
	scrollX.referTo(clip->state, FumeIDs::ScrollX, nullptr, 0);
	scrollY.referTo(clip->state, FumeIDs::ScrollY, nullptr, 64 * fumeUI::midiEditorNoteHeight);
	cursorX.referTo(clip->state, FumeIDs::CursorX, nullptr, 0);
	cursorY.referTo(clip->state, FumeIDs::CursorY, nullptr, 64);
	noteLength.referTo(clip->state, FumeIDs::NoteLength, nullptr, 0.1);
	noteVelocity.referTo(clip->state, FumeIDs::NoteVelocity, nullptr, 100);

	if (cursorQuantisation.getType(false) == cursorQuantisation.getAvailableQuantiseTypes(false)[0])
	{
		cursorQuantisation.setType(cursorQuantisation.getAvailableQuantiseTypes(false)[4]);
	}		
	DBG("ExtendedMidiClip::ExtendedMidiClip: cursorQuantisation.getType(false) = " << cursorQuantisation.getType(false));
}
