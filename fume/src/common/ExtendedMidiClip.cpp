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
	noteLength.referTo(clip->state, FumeIDs::NoteLength, nullptr, 0.12);
}
