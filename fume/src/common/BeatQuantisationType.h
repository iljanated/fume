#pragma once

#include <JuceHeader.h>

namespace te = tracktion;

class BeatQuantisationType
{
public:
	BeatQuantisationType(te::BeatDuration type) : type(type) {}
	~BeatQuantisationType() = default;

	te::BeatPosition roundBeatUp(te::BeatPosition beat) const
	{
		auto beatInBeats = beat.inBeats();
		auto quantisedBeatInBeats = std::ceil(beatInBeats / type.inBeats()) * type.inBeats();
		return te::BeatPosition::fromBeats(quantisedBeatInBeats);
	}

	te::BeatPosition roundBeatDown(te::BeatPosition beat) const
	{
		auto beatInBeats = beat.inBeats();
		auto quantisedBeatInBeats = std::floor(beatInBeats / type.inBeats()) * type.inBeats();
		return te::BeatPosition::fromBeats(quantisedBeatInBeats);
	}

	te::BeatDuration getType() const { return type; }

private:
	te::BeatDuration type;
};