
/* Lasso component based on juce lasso component but to be controlled directly with coordinates instead of mouse events  */

#include <JuceHeader.h>

template <class SelectableItemType>
class ManualLassoComponent : public Component
{
public:
    //==============================================================================
    /** Creates a Lasso component. */
    ManualLassoComponent() = default;
	~ManualLassoComponent() override = default;

    //==============================================================================
    void beginLasso(juce::Point<int> position, juce::ModifierKeys modifiers, LassoSource<SelectableItemType>* lassoSource)
    {
        jassert(source == nullptr);  // this suggests that you didn't call endLasso() after the last drag...
        jassert(lassoSource != nullptr); // the source can't be null!
        jassert(getParentComponent() != nullptr);  // you need to add this to a parent component for it to work!

        source = lassoSource;

        if (lassoSource != nullptr)
            originalSelection = lassoSource->getLassoSelection().getItemArray();

        setSize(0, 0);
        dragStartPos = position;
        selectionRectangle = juce::Rectangle<int>(dragStartPos, dragStartPos);
		dragLasso(position, modifiers);
    }

    /** Call this in your mouseDrag event, to update the lasso's position.

        This must be repeatedly calling when the mouse is dragged, after you've
        first initialised the lasso with beginLasso().

        This method takes into account the modifier keys that are being held down, so
        if shift is pressed, then the lassoed items will be added to any that were
        previously selected; if ctrl or command is pressed, then they will be xor'ed
        with previously selected items.

        @see beginLasso, endLasso
    */
    void dragLasso(juce::Point<int> position, juce::ModifierKeys modifiers)
    {
        if (source != nullptr)
        {
            selectionRectangle = juce::Rectangle<int>(dragStartPos, position);
            auto drawRectangle = selectionRectangle.expanded(margin, margin);
			drawRectangle.setHeight(rowHeight + drawRectangle.getHeight());
			setBounds(drawRectangle);
            setVisible(true);

            Array<SelectableItemType> itemsInLasso;
            source->findLassoItemsInArea(itemsInLasso, selectionRectangle);

            if (modifiers.isShiftDown())
            {
                itemsInLasso.removeValuesIn(originalSelection); //  to avoid duplicates
                itemsInLasso.addArray(originalSelection);
            }
            else if (modifiers.isCommandDown() || modifiers.isAltDown())
            {
                auto originalMinusNew = originalSelection;
                originalMinusNew.removeValuesIn(itemsInLasso);

                itemsInLasso.removeValuesIn(originalSelection);
                itemsInLasso.addArray(originalMinusNew);
            }

            source->getLassoSelection() = SelectedItemSet<SelectableItemType>(itemsInLasso);
        }
    }

    /** Call this in your mouseUp event, after the lasso has been dragged.
        @see beginLasso, dragLasso
    */
    void endLasso()
    {
        source = nullptr;
        originalSelection.clear();
        setVisible(false);
    }

    //==============================================================================
    /** A set of colour IDs to use to change the colour of various aspects of the label.

        These constants can be used either via the Component::setColour(), or LookAndFeel::setColour()
        methods.

        Note that you can also use the constants from TextEditor::ColourIds to change the
        colour of the text editor that is opened when a label is editable.

        @see Component::setColour, Component::findColour, LookAndFeel::setColour, LookAndFeel::findColour
    */
    enum ColourIds
    {
        lassoFillColourId = 0x1000440, /**< The colour to fill the lasso rectangle with. */
        lassoOutlineColourId = 0x1000441, /**< The colour to draw the outline with. */
    };

    //==============================================================================
    /** @internal */
    void paint(Graphics& g) override
    {
        getLookAndFeel().drawLasso(g, *this);
    }

    /** @internal */
    bool hitTest(int, int) override { return false; }

    void setRowHeight(int height) { rowHeight = height; }
    int getRowHeight() const { return rowHeight; }

    void setMargin(int m) { margin = m; }
    int getMargin() const { return margin; }
private:
    //==============================================================================
    Array<SelectableItemType> originalSelection;
    LassoSource<SelectableItemType>* source = nullptr;
    Point<int> dragStartPos;
    int rowHeight = 0;
    int margin = 0;
    juce::Rectangle<int> selectionRectangle;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ManualLassoComponent)
};