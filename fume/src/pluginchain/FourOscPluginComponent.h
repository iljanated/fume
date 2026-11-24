#pragma once

#include <JuceHeader.h>
#include "UIContext.h"
#include "InputComponents.h"

namespace te = tracktion;

class OscComponent : public juce::Component
{
public:
	OscComponent(fumeUI::UIContext& context)
		: uiContext(context)
	{
		waveShapeSlider.setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
		addAndMakeVisible(waveShapeSlider);
		waveShapeLabel.setJustificationType(juce::Justification::centred);
		addAndMakeVisible(waveShapeLabel);

		pwmSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
		pwmSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
		addAndMakeVisible(pwmSlider);
		pwmLabel.setJustificationType(juce::Justification::centred);
		addAndMakeVisible(pwmLabel);

		levelSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
		levelSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
		addAndMakeVisible(levelSlider);
		levelLabel.setJustificationType(juce::Justification::centred);
		addAndMakeVisible(levelLabel);

		panSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
		panSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
		addAndMakeVisible(panSlider);
		panLabel.setJustificationType(juce::Justification::centred);
		addAndMakeVisible(panLabel);

		coarseTuneSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
		coarseTuneSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
		addAndMakeVisible(coarseTuneSlider);
		coarseTuneLabel.setJustificationType(juce::Justification::centred);
		addAndMakeVisible(coarseTuneLabel);

		fineTuneSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
		fineTuneSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
		addAndMakeVisible(fineTuneSlider);
		fineTuneLabel.setJustificationType(juce::Justification::centred);
		addAndMakeVisible(fineTuneLabel);

		unisonSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
		unisonSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
		addAndMakeVisible(unisonSlider);
		unisonLabel.setJustificationType(juce::Justification::centred);
		addAndMakeVisible(unisonLabel);

		detuneSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
		detuneSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
		addAndMakeVisible(detuneSlider);
		detuneLabel.setJustificationType(juce::Justification::centred);
		addAndMakeVisible(detuneLabel);

		spreadSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
		spreadSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
		addAndMakeVisible(spreadSlider);
		spreadLabel.setJustificationType(juce::Justification::centred);
		addAndMakeVisible(spreadLabel);
	}
	
	~OscComponent() override = default;
	
	void paint(juce::Graphics& g) override
	{
	}
	
	void resized() override
	{
		auto localBounds = getLocalBounds();
		auto width = localBounds.getWidth() / 9;
		auto labelHeight = 24;
		auto sliderHeight = localBounds.getHeight() - labelHeight;

		auto waveShapeRect = localBounds.removeFromLeft(width);
		waveShapeSlider.setBounds(waveShapeRect.removeFromTop(sliderHeight));
		waveShapeLabel.setBounds(waveShapeRect);

		auto pwmRect = localBounds.removeFromLeft(width);
		pwmSlider.setBounds(pwmRect.removeFromTop(sliderHeight));
		pwmLabel.setBounds(pwmRect);

		auto levelRect = localBounds.removeFromLeft(width);
		levelSlider.setBounds(levelRect.removeFromTop(sliderHeight));
		levelLabel.setBounds(levelRect);

		auto panRect = localBounds.removeFromLeft(width);
		panSlider.setBounds(panRect.removeFromTop(sliderHeight));
		panLabel.setBounds(panRect);

		auto coarseTuneRect = localBounds.removeFromLeft(width);
		coarseTuneSlider.setBounds(coarseTuneRect.removeFromTop(sliderHeight));
		coarseTuneLabel.setBounds(coarseTuneRect);

		auto fineTuneRect = localBounds.removeFromLeft(width);
		fineTuneSlider.setBounds(fineTuneRect.removeFromTop(sliderHeight));
		fineTuneLabel.setBounds(fineTuneRect);

		auto unisonRect = localBounds.removeFromLeft(width);
		unisonSlider.setBounds(unisonRect.removeFromTop(sliderHeight));
		unisonLabel.setBounds(unisonRect);

		auto detuneRect = localBounds.removeFromLeft(width);
		detuneSlider.setBounds(detuneRect.removeFromTop(sliderHeight));
		detuneLabel.setBounds(detuneRect);

		auto spreadRect = localBounds.removeFromLeft(width);
		spreadSlider.setBounds(spreadRect.removeFromTop(sliderHeight));
		spreadLabel.setBounds(spreadRect);
	}

	fumeUI::KeyboardSlider& getWaveShapeSlider() { return waveShapeSlider; }
	fumeUI::KeyboardSlider& getPwmSlider() { return pwmSlider; }
	fumeUI::KeyboardSlider& getLevelSlider() { return levelSlider; }
	fumeUI::KeyboardSlider& getPanSlider() { return panSlider; }
	fumeUI::KeyboardSlider& getCoarseTuneSlider() { return coarseTuneSlider; }
	fumeUI::KeyboardSlider& getFineTuneSlider() { return fineTuneSlider; }
	fumeUI::KeyboardSlider& getUnisonSlider() { return unisonSlider; }
	fumeUI::KeyboardSlider& getDetuneSlider() { return detuneSlider; }
	fumeUI::KeyboardSlider& getSpreadSlider() { return spreadSlider; }

private:
	fumeUI::UIContext& uiContext;
	fumeUI::KeyboardSlider waveShapeSlider{ "Wave Shape", &uiContext.extendedEdit.getUndoManager() };
	fumeUI::KeyboardSlider pwmSlider{ "PWM", &uiContext.extendedEdit.getUndoManager() };
	fumeUI::KeyboardSlider levelSlider{ "Level", &uiContext.extendedEdit.getUndoManager() };
	fumeUI::KeyboardSlider panSlider{ "Pan", &uiContext.extendedEdit.getUndoManager() };
	fumeUI::KeyboardSlider coarseTuneSlider{ "Tune", &uiContext.extendedEdit.getUndoManager() };
	fumeUI::KeyboardSlider fineTuneSlider{ "Fine", &uiContext.extendedEdit.getUndoManager() };
	fumeUI::KeyboardSlider unisonSlider{ "Unison", &uiContext.extendedEdit.getUndoManager() };
	fumeUI::KeyboardSlider detuneSlider{ "Detune", &uiContext.extendedEdit.getUndoManager() };
	fumeUI::KeyboardSlider spreadSlider{ "Spread", &uiContext.extendedEdit.getUndoManager() };

	juce::Label waveShapeLabel{ {}, "Shape" };
	juce::Label pwmLabel{ {}, "PWM" };
	juce::Label levelLabel{ {}, "Level" };
	juce::Label panLabel{ {}, "Pan" };
	juce::Label coarseTuneLabel{ {}, "Tune" };
	juce::Label fineTuneLabel{ {}, "Fine" };
	juce::Label unisonLabel{ {}, "Unison" };
	juce::Label detuneLabel{ {}, "Detune" };
	juce::Label spreadLabel{ {}, "Spread" };

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OscComponent)
};

class FourOscPluginComponent : public juce::Component
{
public:
	FourOscPluginComponent(fumeUI::UIContext& context, te::FourOscPlugin* plugin)
		: uiContext(context), plugin(plugin)
	{
		addAndMakeVisible(osc1Component);
		addAndMakeVisible(osc2Component);
		addAndMakeVisible(osc3Component);
		addAndMakeVisible(osc4Component);
		addAndMakeVisible(ampADSRComponent);

		setSize(700, 300);
	}

	~FourOscPluginComponent() override
	{
	}

	void paint(juce::Graphics& g) override
	{
		g.fillAll(juce::Colours::black);
		g.setColour(juce::Colours::white);
		g.drawText("FourOscPluginComponent", getLocalBounds(), juce::Justification::centred, true);
	}

	void resized() override
	{
		auto localBounds = juce::Rectangle(0, 0, getWidth(), getHeight());

		auto oscHeight = localBounds.getHeight() / 4;
		auto oscRect = localBounds.removeFromLeft(300);
		osc1Component.setBounds(oscRect.removeFromTop(oscHeight));
		osc2Component.setBounds(oscRect.removeFromTop(oscHeight));
		osc3Component.setBounds(oscRect.removeFromTop(oscHeight));
		osc4Component.setBounds(oscRect.removeFromTop(oscHeight));

		ampADSRComponent.setBounds(localBounds.removeFromTop(200));
	}

private:
	fumeUI::UIContext& uiContext;
	te::FourOscPlugin* plugin;

	fumeUI::ADSRComponent ampADSRComponent{ uiContext };
	OscComponent osc1Component{ uiContext };
	OscComponent osc2Component{ uiContext };
	OscComponent osc3Component{ uiContext };
	OscComponent osc4Component{ uiContext };


	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FourOscPluginComponent)
};