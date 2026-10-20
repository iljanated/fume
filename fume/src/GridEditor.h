#pragma once

#include <JuceHeader.h>
#include "Utilities.h"
#include "CursorComponent.h"
#include "SelectionBoxComponent.h"
#include "ToolState.h"

template <typename T>
class GridEditor
	: public juce::Component,
	public juce::ComponentListener,
	public SelectionBoxSource,
	public fumeUI::Grid
{
public:
	GridEditor(fumeUI::UIContext& context, juce::Viewport& v)
		: uiContext(context),
		viewport(v)
	{
		addChildComponent(selectionBoxComponent);
		selectionBoxComponent.setAlwaysOnTop(true);
		selectionBoxComponent.addComponentListener(this);

		addAndMakeVisible(cursorComponent);
		cursorComponent.setAlwaysOnTop(true);
		cursorComponent.addComponentListener(this);

		setWantsKeyboardFocus(true);
	}
	virtual ~GridEditor() = default;

	bool keyPressed(const juce::KeyPress& key)
	{
		if (toolStateManager.keyPressed(key))
		{
			return true;
		}

		if (selectionBoxComponent.getIsSelecting() && selectionBoxComponent.keyPressed(key))
		{
			return true;
		}

		if (cursorComponent.keyPressed(key))
		{
			return true;
		}
		return false;
	}

	bool keyStateChanged(bool isKeyDown)
	{
		if (toolStateManager.keyStateChanged(isKeyDown))
		{
			return true;
		}
		return false;
	}

	void componentMovedOrResized(juce::Component& component, bool wasMoved, bool wasResized)
	{
		if (&component == &cursorComponent)
		{
			auto x = cursorComponent.getX();
			auto y = cursorComponent.getY();

			juce::Rectangle<int> viewArea = viewport.getViewArea();
			int currentScrollX = viewArea.getX();
			int currentScrollY = viewArea.getY();

			int viewWidth = viewArea.getWidth();
			int viewHeight = viewArea.getHeight();

			int maxX = currentScrollX + viewWidth;
			int maxY = currentScrollY + viewHeight;

			int targetScrollX = currentScrollX;
			int targetScrollY = currentScrollY;

			if (x < currentScrollX + FUME_SCROLL_PADDING)
			{
				targetScrollX = x - FUME_SCROLL_PADDING;
			}
			else if (x > maxX - FUME_SCROLL_PADDING)
			{
				targetScrollX = x - viewWidth + FUME_SCROLL_PADDING;
			}

			if (y < currentScrollY + FUME_SCROLL_PADDING)
			{
				targetScrollY = y - FUME_SCROLL_PADDING;
			}
			else if (y + getUnitSize().getY() > maxY - FUME_SCROLL_PADDING)
			{
				targetScrollY = y + getUnitSize().getY() - viewHeight + FUME_SCROLL_PADDING;
			}

			if (targetScrollX != currentScrollX || targetScrollY != currentScrollY)
			{
				viewport.setViewPosition(targetScrollX, targetScrollY);
			}
		}
	}

	CursorComponent& getCursorComponent() { return cursorComponent; }
	SelectionBoxComponent& getSelectionBoxComponent() { return selectionBoxComponent; }

	fumeUI::UIContext& getUIContext() { return uiContext; }



protected:
	fumeUI::UIContext& uiContext;
	juce::Viewport& viewport;
	CursorComponent cursorComponent{ *this, 2, uiContext.inputManager };
	SelectionBoxComponent selectionBoxComponent{ *this, 1, this, uiContext.inputManager };
	ToolStateManager<T> toolStateManager{ uiContext.inputManager };


	void resizeViewport()
	{
		auto gridSize = getGridSize();
		auto width = gridSize.getX() * getUnitSize().getX();
		auto height = gridSize.getY() * getUnitSize().getY() + FUME_SCROLL_PADDING;
		setBounds(0, 0, width, height);

	}


	JUCE_DECLARE_NON_COPYABLE(GridEditor)
};