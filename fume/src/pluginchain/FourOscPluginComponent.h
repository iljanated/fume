#pragma once

#include <JuceHeader.h>
#include "UIContext.h"
#include "InputComponents.h"

namespace te = tracktion;

class OscComponent : public juce::Component
{
public:
	OscComponent(fumeUI::UIContext& context, te::FourOscPlugin::OscParams* params)
		: uiContext(context), params(params)
	{
		waveShapeSlider.setRange(0.0, 5.0, 1.0);
		waveShapeSlider.setValue(params->waveShapeValue.get(), juce::dontSendNotification);
		waveShapeSlider.getValueObject().referTo(params->waveShapeValue.getPropertyAsValue());

		waveShapeSlider.setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
		addAndMakeVisible(waveShapeLabel);

		pwmSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
		pwmSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
		addAndMakeVisible(pwmLabel);

		levelSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
		levelSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
		addAndMakeVisible(levelLabel);

		panSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
		panSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
		addAndMakeVisible(panLabel);

		coarseTuneSlider.setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
		addAndMakeVisible(coarseTuneLabel);

		fineTuneSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
		fineTuneSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
		addAndMakeVisible(fineTuneLabel);

		unisonSlider.setRange(1.0, 8.0, 1.0);
		// force initialisation of voicesValue
		params->voicesValue = juce::jlimit(1, 8, params->voicesValue.get());
		unisonSlider.setValue(params->voicesValue.get(), juce::dontSendNotification);
		unisonSlider.getValueObject().referTo(params->voicesValue.getPropertyAsValue());

		unisonSlider.setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
		addAndMakeVisible(unisonLabel);

		detuneSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
		detuneSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
		addAndMakeVisible(detuneLabel);

		spreadSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
		spreadSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
		addAndMakeVisible(spreadLabel);
	}
	
	~OscComponent() override = default;
	
	void paint(juce::Graphics& g) override
	{
	}
	
	void resized() override
	{
		auto localBounds = getLocalBounds();
		
		juce::FlexBox flexBox;
		flexBox.flexDirection = juce::FlexBox::Direction::row;

		flexBox.items.add(juce::FlexItem(waveShapeLabel).withFlex(1.0f).withMargin(1.0f));
		flexBox.items.add(juce::FlexItem(pwmLabel).withFlex(1.0f).withMargin(1.0f));
		flexBox.items.add(juce::FlexItem(levelLabel).withFlex(1.0f).withMargin(1.0f));
		flexBox.items.add(juce::FlexItem(panLabel).withFlex(1.0f).withMargin(1.0f));
		flexBox.items.add(juce::FlexItem(coarseTuneLabel).withFlex(1.0f).withMargin(1.0f));
		flexBox.items.add(juce::FlexItem(fineTuneLabel).withFlex(1.0f).withMargin(1.0f));
		flexBox.items.add(juce::FlexItem(unisonLabel).withFlex(1.0f).withMargin(1.0f));
		flexBox.items.add(juce::FlexItem(detuneLabel).withFlex(1.0f).withMargin(1.0f));
		flexBox.items.add(juce::FlexItem(spreadLabel).withFlex(1.0f).withMargin(1.0f));
		
		flexBox.performLayout(localBounds);
	}

private:
	fumeUI::UIContext& uiContext;
	te::FourOscPlugin::OscParams* params;
	fumeUI::OptionsSlider waveShapeSlider{ "Wave Shape", &uiContext.extendedEdit.getUndoManager(), { "None", "Sine", "Triangle", "Saw", "Square" } };
	fumeUI::KeyboardSlider pwmSlider{ "PWM", &uiContext.extendedEdit.getUndoManager() };
	fumeUI::KeyboardSlider levelSlider{ "Level", &uiContext.extendedEdit.getUndoManager() };
	fumeUI::KeyboardSlider panSlider{ "Pan", &uiContext.extendedEdit.getUndoManager() };
	fumeUI::KeyboardSlider coarseTuneSlider{ "Tune", &uiContext.extendedEdit.getUndoManager() };
	fumeUI::KeyboardSlider fineTuneSlider{ "Fine", &uiContext.extendedEdit.getUndoManager() };
	fumeUI::KeyboardSlider unisonSlider{ "Unison", &uiContext.extendedEdit.getUndoManager() };
	fumeUI::KeyboardSlider detuneSlider{ "Detune", &uiContext.extendedEdit.getUndoManager() };
	fumeUI::KeyboardSlider spreadSlider{ "Spread", &uiContext.extendedEdit.getUndoManager() };

	fumeUI::LabeledSliderContainer waveShapeLabel{ waveShapeSlider, "Shape" };
	fumeUI::LabeledSliderContainer pwmLabel{ pwmSlider, "PWM" };
	fumeUI::LabeledSliderContainer levelLabel{ levelSlider, "Level" };
	fumeUI::LabeledSliderContainer panLabel{ panSlider, "Pan" };
	fumeUI::LabeledSliderContainer coarseTuneLabel{ coarseTuneSlider, "Tune" };
	fumeUI::LabeledSliderContainer fineTuneLabel{ fineTuneSlider, "Fine" };
	fumeUI::LabeledSliderContainer unisonLabel{ unisonSlider, "Unison" };
	fumeUI::LabeledSliderContainer detuneLabel{ detuneSlider, "Detune" };
	fumeUI::LabeledSliderContainer spreadLabel{ spreadSlider, "Spread" };

	fumeUI::ParameterSliderAttachment pwmAttachment{ params->pulseWidth, pwmSlider };
	fumeUI::ParameterSliderAttachment levelAttachment{ params->level, levelSlider };
	fumeUI::ParameterSliderAttachment panAttachment{ params->pan, panSlider };
	fumeUI::ParameterSliderAttachment coarseTuneAttachment{ params->tune, coarseTuneSlider };
	fumeUI::ParameterSliderAttachment fineTuneAttachment{ params->fineTune, fineTuneSlider };
	fumeUI::ParameterSliderAttachment detuneAttachment{ params->detune, detuneSlider };
	fumeUI::ParameterSliderAttachment spreadAttachment{ params->spread, spreadSlider };

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
		addAndMakeVisible(filterADSRComponent);

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

		juce::FlexBox oscFlexBox;

		oscFlexBox.flexDirection = juce::FlexBox::Direction::column;

		oscFlexBox.items.add(juce::FlexItem(osc1Component).withFlex(1.0f).withMargin(1.0f));
		oscFlexBox.items.add(juce::FlexItem(osc2Component).withFlex(1.0f).withMargin(1.0f));
		oscFlexBox.items.add(juce::FlexItem(osc3Component).withFlex(1.0f).withMargin(1.0f));
		oscFlexBox.items.add(juce::FlexItem(osc4Component).withFlex(1.0f).withMargin(1.0f));

		juce::FlexBox vertFlexBox;
		vertFlexBox.flexDirection = juce::FlexBox::Direction::column;
		vertFlexBox.items.add(juce::FlexItem(oscFlexBox).withFlex(1.0f).withMargin(1.0f));
		vertFlexBox.items.add(juce::FlexItem(ampADSRComponent).withFlex(1.0f).withMargin(1.0f));
		vertFlexBox.items.add(juce::FlexItem(filterADSRComponent).withFlex(1.0f).withMargin(1.0f));
		
		vertFlexBox.performLayout(localBounds);
	}

private:
	fumeUI::UIContext& uiContext;
	te::FourOscPlugin* plugin;

	fumeUI::ADSRComponent ampADSRComponent{ uiContext };
	fumeUI::ADSRComponent filterADSRComponent{ uiContext };

	OscComponent osc1Component{ uiContext, plugin->oscParams[0]};
	OscComponent osc2Component{ uiContext, plugin->oscParams[1]};
	OscComponent osc3Component{ uiContext, plugin->oscParams[2]};
	OscComponent osc4Component{ uiContext, plugin->oscParams[3]};

	fumeUI::ParameterSliderAttachment ampASliderAttachment{ plugin->ampAttack, ampADSRComponent.getAttackSlider() };
	fumeUI::ParameterSliderAttachment ampDSliderAttachment{ plugin->ampDecay, ampADSRComponent.getDecaySlider() };
	fumeUI::ParameterSliderAttachment ampSSliderAttachment{ plugin->ampSustain, ampADSRComponent.getSustainSlider() };
	fumeUI::ParameterSliderAttachment ampRSliderAttachment{ plugin->ampRelease, ampADSRComponent.getReleaseSlider() };

	fumeUI::ParameterSliderAttachment filterASliderAttachment{ plugin->filterAttack, filterADSRComponent.getAttackSlider() };
	fumeUI::ParameterSliderAttachment filterDSliderAttachment{ plugin->filterDecay, filterADSRComponent.getDecaySlider() };
	fumeUI::ParameterSliderAttachment filterSSliderAttachment{ plugin->filterSustain, filterADSRComponent.getSustainSlider() };
	fumeUI::ParameterSliderAttachment filterRSliderAttachment{ plugin->filterRelease, filterADSRComponent.getReleaseSlider() };

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FourOscPluginComponent)
};