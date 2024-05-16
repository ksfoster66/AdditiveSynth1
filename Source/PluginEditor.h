/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

#include <array>

//==============================================================================
/**
*/
class AdditiveSynth1AudioProcessorEditor  : public juce::AudioProcessorEditor, juce::Button::Listener
{
public:
    AdditiveSynth1AudioProcessorEditor (AdditiveSynth1AudioProcessor&);
    ~AdditiveSynth1AudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AdditiveSynth1AudioProcessor& audioProcessor;

	juce::AudioParameterInt* numberOfPartials{ nullptr };
	int currentNumPartials = NUM_PARTIALS;

	//Master gain
	juce::Slider masterGainSlider;
	std::unique_ptr<APVTS::SliderAttachment> masterGainSliderAttachment;

	//Partial controls
	std::array<juce::Slider, MAX_PARTIALS> partialSpacesSliders;
	std::array<juce::Slider, MAX_PARTIALS> partialVolumesSliders;
	std::array<juce::ToggleButton, MAX_PARTIALS> partialBypassButtons;

	std::array<std::unique_ptr<APVTS::SliderAttachment>, MAX_PARTIALS> partialSpacesSliderAttaches;
	std::array<std::unique_ptr<APVTS::SliderAttachment>, MAX_PARTIALS> partialVolumesSliderAttaches;
	std::array<std::unique_ptr<APVTS::ButtonAttachment>, MAX_PARTIALS> partialBypassButtonAttaches;

	//Envelope Sliders
	juce::Slider attackSlider;
	juce::Slider decaySlider;
	juce::Slider sustainSlider;
	juce::Slider releaseSlider;

	std::unique_ptr<APVTS::SliderAttachment> attackSliderAttach;
	std::unique_ptr<APVTS::SliderAttachment> decaySliderAttach;
	std::unique_ptr<APVTS::SliderAttachment> sustainSliderAttach;
	std::unique_ptr<APVTS::SliderAttachment> releaseSliderAttach;

	//Filter controls
	juce::Slider cutoffSlider;
	juce::Slider resonanceSlider;
	juce::ToggleButton filterBypassButton;

	std::unique_ptr<APVTS::SliderAttachment> cutoffSliderAttach;
	std::unique_ptr<APVTS::SliderAttachment> resonanceSliderAttach;
	std::unique_ptr<APVTS::ButtonAttachment> filterBypassButtonAttach;

	//Partial buttons
	juce::ArrowButton addPartial;
	juce::ArrowButton subtractPartial;

	juce::TooltipWindow tooltip{ this };

	void buttonClicked(juce::Button*) override;
	void updatePartialControls();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AdditiveSynth1AudioProcessorEditor)

};
