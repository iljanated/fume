#pragma once

#include <JuceHeader.h>

namespace te = tracktion;

class SelectableClipClass : public te::SelectableClass {
public:

    SelectableClipClass() {}
    ~SelectableClipClass() {}

    void deleteSelected(const DeleteSelectedParams& params) override;
};