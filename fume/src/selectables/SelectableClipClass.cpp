#pragma once

#include "SelectableClipClass.h"

namespace te = tracktion;

void SelectableClipClass::deleteSelected(const DeleteSelectedParams& params)
{
    // Unpack the SelectableList from the parameter structure
    for (auto* element : params.items)
    {
        if (auto* clip = dynamic_cast<te::Clip*>(element))
        {
            clip->removeFromParent();
        }
    }
}

te::SelectableClass::ClassInstance<SelectableClipClass, te::Clip> selectableClipClass;