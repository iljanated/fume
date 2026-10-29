#pragma once

#include "ExtendedEdit.h"

ExtendedEdit::ExtendedEdit(te::Edit& e, juce::UndoManager* um) : edit(e)
{
    zoomX.referTo(edit.state, FumeIDs::ZoomX, nullptr, 1.0f);
    zoomY.referTo(edit.state, FumeIDs::ZoomY, nullptr, 1.0f);
    scrollX.referTo(edit.state, FumeIDs::ScrollX, nullptr, 0);
    scrollY.referTo(edit.state, FumeIDs::ScrollY, nullptr, 0);
}
