#pragma once

#include "UIContext.h"
#include "ExtendedMidiClip.h"

namespace te = tracktion;

namespace fumeUI
{
	struct UIMidiClipContext : public UIContext
	{
		UIMidiClipContext(UIContext& context, ExtendedMidiClip& clip)
			: UIContext{ context.engine, context.extendedEdit, context.selectionManager, context.inputManager }
			, extendedMidiClip(clip)
		{
		}

		ExtendedMidiClip& extendedMidiClip;
	};
}