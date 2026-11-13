#include "ExtendedMidiClip.h"

ExtendedMidiClip::ExtendedMidiClip(te::MidiClip* c)
	: clip(c){
	jassert(clip != nullptr);

	auto& tempoSequence = clip->edit.tempoSequence;
	auto startBeat = clip->getStartBeat();
	auto startTime = tempoSequence.beatsToTime(startBeat).inSeconds();

	zoomX.referTo(clip->state, FumeIDs::ZoomX, nullptr, 1.0f);
	zoomY.referTo(clip->state, FumeIDs::ZoomY, nullptr, 1.0f);
	scrollX.referTo(clip->state, FumeIDs::ScrollX, nullptr, startTime / fumeUI::midiEditorSecondsPerPixel);
	scrollY.referTo(clip->state, FumeIDs::ScrollY, nullptr, 79 * fumeUI::midiEditorNoteHeight);
	cursorX.referTo(clip->state, FumeIDs::CursorX, nullptr, startTime);
	cursorY.referTo(clip->state, FumeIDs::CursorY, nullptr, 79);
	noteLength.referTo(clip->state, FumeIDs::NoteLength, nullptr, 0.1);
	noteVelocity.referTo(clip->state, FumeIDs::NoteVelocity, nullptr, 100);
	cursorSnapLevel.referTo(clip->state, FumeIDs::CursorSnapLevel, nullptr, 7);
}
