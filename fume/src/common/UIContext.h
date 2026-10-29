#pragma once

#include "Utilities.h"
#include "ExtendedEdit.h"
#include "InputManager.h"

namespace te = tracktion;

namespace fumeUI
{
	struct UIContext
	{
		te::Engine& engine;
		ExtendedEdit& extendedEdit;
		te::SelectionManager& selectionManager;
		InputManager& inputManager;
	};
}