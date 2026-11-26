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

		levelSlider.setSkewFactorFromMidPoint(-12.0f);
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
	fumeUI::OptionsSlider waveShapeSlider{ "Wave Shape", &uiContext.extendedEdit.getUndoManager(), { "None", "Sine", "Square", "Saw", "Triangle", "Noise"}};
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

class FourOscPluginComponent : public juce::Component, public juce::Button::Listener
{
public:
	FourOscPluginComponent(fumeUI::UIContext& context, te::FourOscPlugin* plugin)
		: uiContext(context), plugin(plugin)
	{
		// filter 

		filterTypeSlider.setRange(0.0, 4.0, 1.0);
		filterTypeSlider.setValue(plugin->filterTypeValue, juce::dontSendNotification);
		filterTypeSlider.getValueObject().referTo(plugin->filterTypeValue.getPropertyAsValue());

		filterTypeSlider.setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
		addAndMakeVisible(filterTypeLabel);

		filterCutOffSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
		filterCutOffSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
		addAndMakeVisible(filterCutOffLabel);
		filterResonanceSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
		filterResonanceSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
		addAndMakeVisible(filterResonanceLabel);
		filterEnvAmountSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
		filterEnvAmountSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
		addAndMakeVisible(filterEnvAmountLabel);
		filterKeyTrackSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
		filterKeyTrackSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
		addAndMakeVisible(filterKeyTrackLabel);
		filterVelocityAmountSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
		filterVelocityAmountSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
		addAndMakeVisible(filterVelocityAmountLabel);
		filterSlopeSlider.setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
		addAndMakeVisible(filterSlopeLabel);
		addAndMakeVisible(filterADSRComponent);

		ampADSRComponent.getAttackSlider().setSkewFactorFromMidPoint(3.0);
		ampADSRComponent.getDecaySlider().setSkewFactorFromMidPoint(0.75);
		ampADSRComponent.getReleaseSlider().setSkewFactorFromMidPoint(0.75);

		addAndMakeVisible(osc1Component);
		addAndMakeVisible(osc2Component);
		addAndMakeVisible(osc3Component);
		addAndMakeVisible(osc4Component);

		
		addAndMakeVisible(env1Component);
		addAndMakeVisible(env2Component);

		addAndMakeVisible(ampADSRComponent);

		// distortion controls

		distortionToggleButton.setToggleState(plugin->distortionOnValue, juce::dontSendNotification);
		distortionToggleButton.addListener(this);
		distortionToggleButton.setHasFocusOutline(true);
		addAndMakeVisible(distortionToggleButton);

		distortionDriveSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
		distortionDriveSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
		addAndMakeVisible(distortionDriveLabel);

		// chorus controls
		chorusToggleButton.setToggleState(plugin->chorusOnValue, juce::dontSendNotification);
		chorusToggleButton.addListener(this);
		chorusToggleButton.setHasFocusOutline(true);
		addAndMakeVisible(chorusToggleButton);

		chorusSpeedSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
		chorusSpeedSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
		addAndMakeVisible(chorusSpeedLabel);

		chorusDepthSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
		chorusDepthSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
		addAndMakeVisible(chorusDepthLabel);

		chorusWidthSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
		chorusWidthSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
		addAndMakeVisible(chorusWidthLabel);

		chorusMixSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
		chorusMixSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
		addAndMakeVisible(chorusMixLabel);

		// delay controls
		delayToggleButton.setToggleState(plugin->delayOnValue, juce::dontSendNotification);
		delayToggleButton.addListener(this);
		delayToggleButton.setHasFocusOutline(true);
		addAndMakeVisible(delayToggleButton);

		// force initialisation of voicesValue
		plugin->delayValue = juce::jlimit(0.0f, 4.0f, plugin->delayValue.get());
		delayTimeSlider.setRange(0.0, 4.0, 0.01);
		delayTimeSlider.setValue(plugin->delayValue, juce::dontSendNotification);
		delayTimeSlider.getValueObject().referTo(plugin->delayValue.getPropertyAsValue());
		delayTimeSlider.setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
		addAndMakeVisible(delayTimeLabel);

		delayFeedbackSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
		delayFeedbackSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
		addAndMakeVisible(delayFeedbackLabel);

		delayCrossfeedSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
		delayCrossfeedSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
		addAndMakeVisible(delayCrossfeedLabel);

		delayMixSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
		delayMixSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
		addAndMakeVisible(delayMixLabel);

		// reverb controls
		reverbToggleButton.setToggleState(plugin->reverbOnValue, juce::dontSendNotification);
		reverbToggleButton.addListener(this);
		reverbToggleButton.setHasFocusOutline(true);
		addAndMakeVisible(reverbToggleButton);

		reverbSizeSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
		reverbSizeSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
		addAndMakeVisible(reverbSizeLabel);

		reverbDampingSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
		reverbDampingSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
		addAndMakeVisible(reverbDampingLabel);

		reverbWidthSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
		reverbWidthSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
		addAndMakeVisible(reverbWidthLabel);

		reverbMixSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
		reverbMixSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
		addAndMakeVisible(reverbMixLabel);

		setSize(900, 300);
	}

	~FourOscPluginComponent() override
	{
	}

	void paint(juce::Graphics& g) override
	{
		g.fillAll(juce::Colours::black);
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

		juce::FlexBox filterFlexBox;
		filterFlexBox.flexDirection = juce::FlexBox::Direction::row;
		filterFlexBox.flexWrap = juce::FlexBox::Wrap::wrap;
		filterFlexBox.items.add(juce::FlexItem(filterTypeLabel).withFlex(1.0f).withMinWidth(50.0f).withMargin(1.0f));
		filterFlexBox.items.add(juce::FlexItem(filterCutOffLabel).withFlex(1.0f).withMinWidth(50.0f).withMargin(1.0f));
		filterFlexBox.items.add(juce::FlexItem(filterResonanceLabel).withFlex(1.0f).withMinWidth(50.0f).withMargin(1.0f));
		filterFlexBox.items.add(juce::FlexItem(filterEnvAmountLabel).withFlex(1.0f).withMinWidth(50.0f).withMargin(1.0f));
		filterFlexBox.items.add(juce::FlexItem(filterKeyTrackLabel).withFlex(1.0f).withMinWidth(50.0f).withMargin(1.0f));
		filterFlexBox.items.add(juce::FlexItem(filterVelocityAmountLabel).withFlex(1.0f).withMinWidth(50.0f).withMargin(1.0f));
		filterFlexBox.items.add(juce::FlexItem(filterSlopeLabel).withFlex(1.0f).withMinWidth(50.0f).withMargin(1.0f));
		filterFlexBox.items.add(juce::FlexItem(filterADSRComponent).withFlex(1.0f).withMinWidth(50.0f).withMargin(1.0f));

		juce::FlexBox topFlexBox;
		topFlexBox.flexDirection = juce::FlexBox::Direction::row;
		topFlexBox.items.add(juce::FlexItem(oscFlexBox).withFlex(3.0f).withMargin(1.0f));
		topFlexBox.items.add(juce::FlexItem(filterFlexBox).withFlex(2.0f).withMargin(1.0f));

		juce::FlexBox distortionFlexBox;
		distortionFlexBox.flexDirection = juce::FlexBox::Direction::row;
		distortionFlexBox.flexWrap = juce::FlexBox::Wrap::wrap;
		distortionFlexBox.items.add(juce::FlexItem(distortionToggleButton).withFlex(1.0f).withMinWidth(50.0f).withMargin(1.0f));
		distortionFlexBox.items.add(juce::FlexItem(distortionDriveLabel).withFlex(1.0f).withMinWidth(50.0f).withMargin(1.0f));

		juce::FlexBox chorusFlexBox;
		chorusFlexBox.flexDirection = juce::FlexBox::Direction::row;
		chorusFlexBox.flexWrap = juce::FlexBox::Wrap::wrap;
		chorusFlexBox.items.add(juce::FlexItem(chorusToggleButton).withFlex(1.0f).withMinWidth(50.0f).withMargin(1.0f));
		chorusFlexBox.items.add(juce::FlexItem(chorusSpeedLabel).withFlex(1.0f).withMinWidth(50.0f).withMargin(1.0f));
		chorusFlexBox.items.add(juce::FlexItem(chorusDepthLabel).withFlex(1.0f).withMinWidth(50.0f).withMargin(1.0f));
		chorusFlexBox.items.add(juce::FlexItem(chorusWidthLabel).withFlex(1.0f).withMinWidth(50.0f).withMargin(1.0f));
		chorusFlexBox.items.add(juce::FlexItem(chorusMixLabel).withFlex(1.0f).withMinWidth(50.0f).withMargin(1.0f));

		juce::FlexBox delayFlexBox;
		delayFlexBox.flexDirection = juce::FlexBox::Direction::row;
		delayFlexBox.flexWrap = juce::FlexBox::Wrap::wrap;
		delayFlexBox.items.add(juce::FlexItem(delayToggleButton).withFlex(1.0f).withMinWidth(50.0f).withMargin(1.0f));
		delayFlexBox.items.add(juce::FlexItem(delayTimeLabel).withFlex(1.0f).withMinWidth(50.0f).withMargin(1.0f));
		delayFlexBox.items.add(juce::FlexItem(delayFeedbackLabel).withFlex(1.0f).withMinWidth(50.0f).withMargin(1.0f));
		delayFlexBox.items.add(juce::FlexItem(delayCrossfeedLabel).withFlex(1.0f).withMinWidth(50.0f).withMargin(1.0f));
		delayFlexBox.items.add(juce::FlexItem(delayMixLabel).withFlex(1.0f).withMinWidth(50.0f).withMargin(1.0f));

		juce::FlexBox reverbFlexBox;
		reverbFlexBox.flexDirection = juce::FlexBox::Direction::row;
		reverbFlexBox.flexWrap = juce::FlexBox::Wrap::wrap;
		reverbFlexBox.items.add(juce::FlexItem(reverbToggleButton).withFlex(1.0f).withMinWidth(50.0f).withMargin(1.0f));
		reverbFlexBox.items.add(juce::FlexItem(reverbSizeLabel).withFlex(1.0f).withMinWidth(50.0f).withMargin(1.0f));
		reverbFlexBox.items.add(juce::FlexItem(reverbDampingLabel).withFlex(1.0f).withMinWidth(50.0f).withMargin(1.0f));
		reverbFlexBox.items.add(juce::FlexItem(reverbWidthLabel).withFlex(1.0f).withMinWidth(50.0f).withMargin(1.0f));
		reverbFlexBox.items.add(juce::FlexItem(reverbMixLabel).withFlex(1.0f).withMinWidth(50.0f).withMargin(1.0f));

		juce::FlexBox bottomFlexBox;
		bottomFlexBox.flexDirection = juce::FlexBox::Direction::row;
		bottomFlexBox.items.add(juce::FlexItem(ampADSRComponent).withFlex(1.0f).withMargin(1.0f));
		bottomFlexBox.items.add(juce::FlexItem(env1Component).withFlex(1.0f).withMargin(1.0f));
		bottomFlexBox.items.add(juce::FlexItem(env2Component).withFlex(1.0f).withMargin(1.0f));
		bottomFlexBox.items.add(juce::FlexItem(distortionFlexBox).withFlex(1.0f).withMargin(1.0f));
		bottomFlexBox.items.add(juce::FlexItem(chorusFlexBox).withFlex(1.0f).withMargin(1.0f));
		bottomFlexBox.items.add(juce::FlexItem(delayFlexBox).withFlex(1.0f).withMargin(1.0f));
		bottomFlexBox.items.add(juce::FlexItem(reverbFlexBox).withFlex(1.0f).withMargin(1.0f));

		juce::FlexBox vertFlexBox;
		vertFlexBox.flexDirection = juce::FlexBox::Direction::column;
		vertFlexBox.items.add(juce::FlexItem(topFlexBox).withFlex(3.0f).withMargin(1.0f));
		vertFlexBox.items.add(juce::FlexItem(bottomFlexBox).withFlex(1.0f).withMargin(1.0f));

		vertFlexBox.performLayout(localBounds);
	}

	void buttonClicked(juce::Button* button) override
	{
		if (button == &distortionToggleButton)
		{
			plugin->distortionOnValue = distortionToggleButton.getToggleState();
		}
		else if (button == &chorusToggleButton)
		{
			plugin->chorusOnValue = chorusToggleButton.getToggleState();
		}
		else if (button == &delayToggleButton)
		{
			plugin->delayOnValue = delayToggleButton.getToggleState();
		}
		else if (button == &reverbToggleButton)
		{
			plugin->reverbOnValue = reverbToggleButton.getToggleState();
		}
	}

private:
	fumeUI::UIContext& uiContext;
	te::FourOscPlugin* plugin;

	// osc controls
	
	OscComponent osc1Component{ uiContext, plugin->oscParams[0]};
	OscComponent osc2Component{ uiContext, plugin->oscParams[1]};
	OscComponent osc3Component{ uiContext, plugin->oscParams[2]};
	OscComponent osc4Component{ uiContext, plugin->oscParams[3]};

	// filter controls

	fumeUI::OptionsSlider filterTypeSlider{ "Filter Type", &uiContext.extendedEdit.getUndoManager(), { "None", "Low Pass", "High Pass", "Band Pass", "Notch" } };
	fumeUI::KeyboardSlider filterCutOffSlider{ "Filter Cutoff", &uiContext.extendedEdit.getUndoManager() };
	fumeUI::KeyboardSlider filterResonanceSlider{ "Filter Resonance", &uiContext.extendedEdit.getUndoManager() };
	fumeUI::KeyboardSlider filterEnvAmountSlider{ "Filter Env Amount", &uiContext.extendedEdit.getUndoManager() };
	fumeUI::KeyboardSlider filterKeyTrackSlider{ "Filter Key Track", &uiContext.extendedEdit.getUndoManager() };
	fumeUI::KeyboardSlider filterVelocityAmountSlider{ "Filter Velocity Amount", &uiContext.extendedEdit.getUndoManager() };
	fumeUI::OptionsSlider filterSlopeSlider{ "Filter Slope", &uiContext.extendedEdit.getUndoManager(),{"12db", "24db"}};
	
	fumeUI::ADSRComponent filterADSRComponent{ uiContext };

	// filter containers

	fumeUI::LabeledSliderContainer filterTypeLabel{ filterTypeSlider, "Type" };
	fumeUI::LabeledSliderContainer filterCutOffLabel{ filterCutOffSlider, "Cut" };
	fumeUI::LabeledSliderContainer filterResonanceLabel{ filterResonanceSlider, "Res" };
	fumeUI::LabeledSliderContainer filterEnvAmountLabel{ filterEnvAmountSlider, "Env" };
	fumeUI::LabeledSliderContainer filterKeyTrackLabel{ filterKeyTrackSlider, "Key" };
	fumeUI::LabeledSliderContainer filterVelocityAmountLabel{ filterVelocityAmountSlider, "Vel" };
	fumeUI::LabeledSliderContainer filterSlopeLabel{ filterSlopeSlider, "Slope" };

	// filter attachments

	fumeUI::ParameterSliderAttachment filterCutoffSliderAttachment{ plugin->filterFreq, filterCutOffSlider };
	fumeUI::ParameterSliderAttachment filterResonanceSliderAttachment{ plugin->filterResonance, filterResonanceSlider };
	fumeUI::ParameterSliderAttachment filterEnvAmountSliderAttachment{ plugin->filterAmount, filterEnvAmountSlider };
	fumeUI::ParameterSliderAttachment filterKeyTrackSliderAttachment{ plugin->filterKey, filterKeyTrackSlider };
	fumeUI::ParameterSliderAttachment filterVelocityAmountSliderAttachment{ plugin->filterVelocity, filterVelocityAmountSlider };

	fumeUI::ParameterSliderAttachment filterASliderAttachment{ plugin->filterAttack, filterADSRComponent.getAttackSlider() };
	fumeUI::ParameterSliderAttachment filterDSliderAttachment{ plugin->filterDecay, filterADSRComponent.getDecaySlider() };
	fumeUI::ParameterSliderAttachment filterSSliderAttachment{ plugin->filterSustain, filterADSRComponent.getSustainSlider() };
	fumeUI::ParameterSliderAttachment filterRSliderAttachment{ plugin->filterRelease, filterADSRComponent.getReleaseSlider() };


	// distortion controls
	juce::ToggleButton distortionToggleButton{ "Distortion" };
	fumeUI::KeyboardSlider distortionDriveSlider{ "Distortion Drive", &uiContext.extendedEdit.getUndoManager() };
	fumeUI::LabeledSliderContainer distortionDriveLabel{ distortionDriveSlider, "Drive" };
	fumeUI::ParameterSliderAttachment distortionDriveSliderAttachment{ plugin->distortion, distortionDriveSlider };

	// chorus controls
	juce::ToggleButton chorusToggleButton{ "Chorus" };
	fumeUI::KeyboardSlider chorusSpeedSlider{ "Chorus Speed", &uiContext.extendedEdit.getUndoManager() };
	fumeUI::KeyboardSlider chorusDepthSlider{ "Chorus Depth", &uiContext.extendedEdit.getUndoManager() };
	fumeUI::KeyboardSlider chorusWidthSlider{ "Chorus Width", &uiContext.extendedEdit.getUndoManager() };
	fumeUI::KeyboardSlider chorusMixSlider{ "Chorus Mix", &uiContext.extendedEdit.getUndoManager() };

	fumeUI::LabeledSliderContainer chorusSpeedLabel{ chorusSpeedSlider, "Speed" };
	fumeUI::LabeledSliderContainer chorusDepthLabel{ chorusDepthSlider, "Depth" };
	fumeUI::LabeledSliderContainer chorusWidthLabel{ chorusWidthSlider, "Width" };
	fumeUI::LabeledSliderContainer chorusMixLabel{ chorusMixSlider, "Mix" };

	fumeUI::ParameterSliderAttachment chorusSpeedSliderAttachment{ plugin->chorusSpeed, chorusSpeedSlider };
	fumeUI::ParameterSliderAttachment chorusDepthSliderAttachment{ plugin->chorusDepth, chorusDepthSlider };
	fumeUI::ParameterSliderAttachment chorusWidthSliderAttachment{ plugin->chorusWidth, chorusWidthSlider };
	fumeUI::ParameterSliderAttachment chorusMixSliderAttachment{ plugin->chorusMix, chorusMixSlider };

	// delay controls
	juce::ToggleButton delayToggleButton{ "Delay" };
	fumeUI::KeyboardSlider delayTimeSlider{ "Delay Time", &uiContext.extendedEdit.getUndoManager() };
	fumeUI::KeyboardSlider delayFeedbackSlider{ "Delay Feedback", &uiContext.extendedEdit.getUndoManager() };
	fumeUI::KeyboardSlider delayCrossfeedSlider{ "Delay Crossfeed Pass", &uiContext.extendedEdit.getUndoManager() };
	fumeUI::KeyboardSlider delayMixSlider{ "Delay Mix", &uiContext.extendedEdit.getUndoManager() };

	fumeUI::LabeledSliderContainer delayTimeLabel{ delayTimeSlider, "Time" };
	fumeUI::LabeledSliderContainer delayFeedbackLabel{ delayFeedbackSlider, "Feedback" };
	fumeUI::LabeledSliderContainer delayCrossfeedLabel{ delayCrossfeedSlider, "Crossfeed" };
	fumeUI::LabeledSliderContainer delayMixLabel{ delayMixSlider, "Mix" };

	fumeUI::ParameterSliderAttachment delayFeedbackSliderAttachment{ plugin->delayFeedback, delayFeedbackSlider };
	fumeUI::ParameterSliderAttachment delayCrossfeedSliderAttachment{ plugin->delayCrossfeed, delayCrossfeedSlider };
	fumeUI::ParameterSliderAttachment delayMixSliderAttachment{ plugin->delayMix, delayMixSlider };

	// reverb controls
	juce::ToggleButton reverbToggleButton{ "Reverb" };
	fumeUI::KeyboardSlider reverbSizeSlider{ "Reverb Size", &uiContext.extendedEdit.getUndoManager() };
	fumeUI::KeyboardSlider reverbDampingSlider{ "Reverb Damping", &uiContext.extendedEdit.getUndoManager() };
	fumeUI::KeyboardSlider reverbWidthSlider{ "Reverb Width", &uiContext.extendedEdit.getUndoManager() };
	fumeUI::KeyboardSlider reverbMixSlider{ "Reverb Mix", &uiContext.extendedEdit.getUndoManager() };

	fumeUI::LabeledSliderContainer reverbSizeLabel{ reverbSizeSlider, "Size" };
	fumeUI::LabeledSliderContainer reverbDampingLabel{ reverbDampingSlider, "Damping" };
	fumeUI::LabeledSliderContainer reverbWidthLabel{ reverbWidthSlider, "Width" };
	fumeUI::LabeledSliderContainer reverbMixLabel{ reverbMixSlider, "Mix" };

	fumeUI::ParameterSliderAttachment reverbSizeSliderAttachment{ plugin->reverbSize, reverbSizeSlider };
	fumeUI::ParameterSliderAttachment reverbDampingSliderAttachment{ plugin->reverbDamping, reverbDampingSlider };
	fumeUI::ParameterSliderAttachment reverbWidthSliderAttachment{ plugin->reverbWidth, reverbWidthSlider };
	fumeUI::ParameterSliderAttachment reverbMixSliderAttachment{ plugin->reverbMix, reverbMixSlider };

	// modulation controls

	fumeUI::ADSRComponent env1Component{ uiContext };
	fumeUI::ADSRComponent env2Component{ uiContext };

	fumeUI::OptionsSlider lfo1WaveSlider{ "LFO1 Wave", &uiContext.extendedEdit.getUndoManager(),{ "Sine", "Square", "Triangle", "Saw" } };
	fumeUI::KeyboardSlider lfo1RateSlider{ "LFO1 Rate", &uiContext.extendedEdit.getUndoManager() };
	juce::ToggleButton lfo1SyncToggleButton{ "LFO1 Sync" };
	fumeUI::KeyboardSlider lfo1DepthSlider{ "LFO1 Depth", &uiContext.extendedEdit.getUndoManager() };

	fumeUI::OptionsSlider lfo2WaveSlider{ "LFO2 Wave", &uiContext.extendedEdit.getUndoManager(),{ "Sine", "Square", "Triangle", "Saw" } };
	fumeUI::KeyboardSlider lfo2RateSlider{ "LFO2 Rate", &uiContext.extendedEdit.getUndoManager() };
	juce::ToggleButton lfo2SyncToggleButton{ "LFO2 Sync" };
	fumeUI::KeyboardSlider lfo2DepthSlider{ "LFO2 Depth", &uiContext.extendedEdit.getUndoManager() };

	// global controls

	fumeUI::OptionsSlider modeSlider{ "Mode", &uiContext.extendedEdit.getUndoManager(),{ "Mono", "Legato", "Poly" } };
	fumeUI::KeyboardSlider levelSlider{ "Level", &uiContext.extendedEdit.getUndoManager() };
	fumeUI::KeyboardSlider legatoSlider{ "Legato", &uiContext.extendedEdit.getUndoManager() };
	fumeUI::KeyboardSlider voicesSlider{ "Voices", &uiContext.extendedEdit.getUndoManager() };

	fumeUI::ADSRComponent ampADSRComponent{ uiContext };
	juce::ToggleButton analogAmpEnvToggleButton{ "Analog Amp Env" };


	
	
	// modulation attachments

	fumeUI::ParameterSliderAttachment env1ASliderAttachment{ plugin->modEnvParams[0]->modAttack, env1Component.getAttackSlider() };
	fumeUI::ParameterSliderAttachment env1DSliderAttachment{ plugin->modEnvParams[0]->modDecay, env1Component.getDecaySlider() };
	fumeUI::ParameterSliderAttachment env1SSliderAttachment{ plugin->modEnvParams[0]->modSustain, env1Component.getSustainSlider() };
	fumeUI::ParameterSliderAttachment env1RSliderAttachment{ plugin->modEnvParams[0]->modRelease, env1Component.getReleaseSlider() };

	fumeUI::ParameterSliderAttachment env2ASliderAttachment{ plugin->modEnvParams[1]->modAttack, env2Component.getAttackSlider() };
	fumeUI::ParameterSliderAttachment env2DSliderAttachment{ plugin->modEnvParams[1]->modDecay, env2Component.getDecaySlider() };
	fumeUI::ParameterSliderAttachment env2SSliderAttachment{ plugin->modEnvParams[1]->modSustain, env2Component.getSustainSlider() };
	fumeUI::ParameterSliderAttachment env2RSliderAttachment{ plugin->modEnvParams[1]->modRelease, env2Component.getReleaseSlider() };

	// global attachments

	fumeUI::ParameterSliderAttachment ampASliderAttachment{ plugin->ampAttack, ampADSRComponent.getAttackSlider() };
	fumeUI::ParameterSliderAttachment ampDSliderAttachment{ plugin->ampDecay, ampADSRComponent.getDecaySlider() };
	fumeUI::ParameterSliderAttachment ampSSliderAttachment{ plugin->ampSustain, ampADSRComponent.getSustainSlider() };
	fumeUI::ParameterSliderAttachment ampRSliderAttachment{ plugin->ampRelease, ampADSRComponent.getReleaseSlider() };


	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FourOscPluginComponent)
};