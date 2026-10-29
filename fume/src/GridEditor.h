#pragma once

#include <JuceHeader.h>
#include "UIContext.h"
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
	GridEditor(fumeUI::UIContext& context, juce::Viewport& v, juce::Identifier defaultToolStateIdentifier)
		: uiContext(context),
		viewport(v),
		toolStateManager(context.inputManager, defaultToolStateIdentifier)
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

	void focusGained(FocusChangeType cause) override
	{
		cursorComponent.setVisible(true);
		centerCursorInView();
	}

	void focusLost(FocusChangeType cause) override
	{
		cursorComponent.setVisible(false);
	}

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

			if (x < currentScrollX + fumeUI::scrollPadding)
			{
				targetScrollX = x - fumeUI::scrollPadding;
			}
			else if (x > maxX - fumeUI::scrollPadding)
			{
				targetScrollX = x - viewWidth + fumeUI::scrollPadding;
			}

			if (y < currentScrollY + fumeUI::scrollPadding)
			{
				targetScrollY = y - fumeUI::scrollPadding;
			}
			else if (y + getUnitSize().getY() > maxY - fumeUI::scrollPadding)
			{
				targetScrollY = y + getUnitSize().getY() - viewHeight + fumeUI::scrollPadding;
			}

			if (targetScrollX != currentScrollX || targetScrollY != currentScrollY)
			{
				viewport.setViewPosition(targetScrollX, targetScrollY);
			}
		}
	}

	/** Moves the cursor to the visible area */
	void centerCursorInView()
	{
		juce::Rectangle<int> viewArea = viewport.getViewArea().reduced(fumeUI::scrollPadding);

		// get cursor position in pixels
		auto cursorViewPosition = cursorComponent.getBounds().getCentre();

		if (viewArea.contains(cursorViewPosition))
		{
			return; // cursor is already in view
		}

		auto targetViewPosition = viewArea.getCentre();
		auto x = targetViewPosition.x / getUnitSize().getX();
		x = x - std::fmod(x, getSnappingInterval());

		auto y = std::floor(targetViewPosition.y / getUnitSize().getY());
		cursorComponent.setCursorPosition({ x, y });
	}

	CursorComponent& getCursorComponent() { return cursorComponent; }
	SelectionBoxComponent& getSelectionBoxComponent() { return selectionBoxComponent; }

	fumeUI::UIContext& getUIContext() { return uiContext; }



protected:
	fumeUI::UIContext& uiContext;
	juce::Viewport& viewport;
	CursorComponent cursorComponent{ *this, 2, uiContext.inputManager };
	SelectionBoxComponent selectionBoxComponent{ *this, 1, this, uiContext.inputManager };
	ToolStateManager<T> toolStateManager;


	void resizeViewport()
	{
		auto gridSize = getGridSize();
		auto width = gridSize.getX() * getUnitSize().getX();
		auto height = gridSize.getY() * getUnitSize().getY() + fumeUI::scrollPadding;
		setBounds(0, 0, width, height);

	}


	JUCE_DECLARE_NON_COPYABLE(GridEditor)
};