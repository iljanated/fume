# Copilot Instructions

## Project Guidelines
- User expects analysis to be based on the latest workspace file versions and may point out when stale context is used.
- Directional keyboard focus navigation should use a `fumeUI::CardinalFocusTraverser` (derived from `juce::ComponentTraverser`) with configurable `WrappingType` and perpendicular-penalty scoring, integrated from `TransportComponent` key handling.

## Code Style
- Always use braces for if statements, even single-line branches.
- Prefer descriptive variable names over single-letter names (except for coordinates like x/y and indices like i).