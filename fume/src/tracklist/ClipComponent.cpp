#pragma once
#include "ClipComponent.h"

namespace te = tracktion;

ClipComponent::ClipComponent()
{
	setWantsKeyboardFocus(false);
	setMouseClickGrabsKeyboardFocus(false);

}

ClipComponent::~ClipComponent()
{}