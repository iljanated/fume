#pragma once

#include "Utilities.h"

namespace fumeUI
{
    const double timelineBeatWidth = 20;
    const int timelineTrackHeight = 30;
    const int timelineTrackHeadWidth = 100;
    const int timelineTimelineHeight = 20;
    const double midiEditorBeatWidth = 80;
    const int midiEditorNoteHeight = 10;
    const int midiEditorTimelineHeight = 20;
    const int midiEditorPianoRollWidth = 50;
    const int scrollPadding = 10;
    const int inputRefreshHz = 60;
    const int inputRepeatDelayFrames = 30;
    const int inputRepeatFrames = 2;
	const float zoomStepSize = 0.1f;

    double roundToNearestPowerOfBase(double x, double base) {
        // 1. Guard clauses for invalid inputs
        if (x <= 0.0) return 0.0;
        if (base <= 1.0) return x; // Base must be greater than 1

        // 2. Calculate log in the target base using the change-of-base formula
        double log_base_x = std::log(x) / std::log(base);

        // 3. Find the two bounding exponents
        double exp_lower = std::floor(log_base_x);
        double exp_upper = exp_lower + 1.0;

        // 4. Calculate the actual bounding values
        double lower_bound = std::pow(base, exp_lower);
        double upper_bound = std::pow(base, exp_upper);

        // 5. Return whichever bound is arithmetically closer
        if ((x - lower_bound) < (upper_bound - x)) {
            return lower_bound;
        }
        return upper_bound;
    }


    /** Draw the grid. */
    void drawGrid(juce::Graphics& g, juce::Rectangle<int> bounds, fumeUI::Grid& grid, juce::Colour lineColour, juce::Colour rectColour, bool blackKeys)
    {
        auto gridSize = grid.getGridSize();
        auto unitSize = grid.getUnitSize();
        
		double zoomX = 1.0;
		double zoomY = 1.0;

        auto unitSizeX = unitSize.x * zoomX;
        auto unitSizeY = unitSize.y * zoomY;

		auto minX = std::clamp(bounds.getX(), 0, static_cast<int>(gridSize.x * unitSizeX));
        auto maxX = std::clamp(bounds.getX() + bounds.getWidth(), 0, static_cast<int>(gridSize.x * unitSizeX));

		auto minY = std::clamp(static_cast<int>(bounds.getY() - std::fmod(bounds.getY(), unitSizeY)), 0, static_cast<int>(gridSize.y * unitSizeY));
		auto maxY = std::clamp(bounds.getY() + bounds.getHeight(), 0, static_cast<int>(gridSize.y * unitSizeY));

        auto baseColour = juce::Colours::lightgrey;

        g.setColour(baseColour);
        g.fillRect(bounds);

		if (blackKeys)
        {
            g.setColour(baseColour.darker(0.1f));
            
            for (float y = minY; y < maxY; y += unitSizeY)
            {
                int noteNumber = (127 - static_cast<int>(y / unitSizeY)) % 12;
                if (noteNumber == 1 || noteNumber == 3 || noteNumber == 6 || noteNumber == 8 || noteNumber == 10)
                {
                    g.fillRect(bounds.getX(), (int)y, maxX, (int)unitSizeY);
                }


            }
        }

        // vert lines

        auto stepSize = zoomX * unitSizeX;

        g.setColour(baseColour.darker(0.2f));
        juce::Path gridPath;

		for (auto x = minX + unitSizeX - std::fmod(minX, unitSizeX); x < maxX; x += stepSize)
		{
			gridPath.startNewSubPath(x, minY);
			gridPath.lineTo(x, maxY);
		}
        
        // horizontal lines

        for (auto y = minY + unitSizeY - std::fmod(minY, unitSizeY); y < maxY; y += unitSizeY)
        {
            gridPath.startNewSubPath(minX, y);
            gridPath.lineTo(maxX, y);
        }
        g.strokePath(gridPath, juce::PathStrokeType(1.0f));
    }
}