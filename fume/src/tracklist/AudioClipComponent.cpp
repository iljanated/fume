#pragma once
#include "AudioClipComponent.h"

namespace te = tracktion;

AudioClipComponent::AudioClipComponent(te::WaveAudioClip& c)
    : clip(c)
{}

AudioClipComponent::~AudioClipComponent()
{}

te::Clip& AudioClipComponent::getClip()
{
	return clip;
}
