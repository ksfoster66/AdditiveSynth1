/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AdditiveSynth1AudioProcessorEditor::AdditiveSynth1AudioProcessorEditor (AdditiveSynth1AudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), 
	addPartial("Add Partial", .75, juce::Colours::white), 
	subtractPartial("Subtract Partial", .25, juce::Colours::white)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (600, 300);

	using namespace Params;
	auto params = getParams();

	APVTS& apvts = p.apvts;
	//numberOfPartials = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter(params.at(Names::Num_Partials)));

	//Attach controller
	masterGainSliderAttachment = std::make_unique<APVTS::SliderAttachment>(apvts, params.at(Names::Master_Gain), masterGainSlider);

	attackSliderAttach = std::make_unique<APVTS::SliderAttachment>(apvts, params.at(Names::Envelope_Attack), attackSlider);
	decaySliderAttach = std::make_unique<APVTS::SliderAttachment>(apvts, params.at(Names::Envelope_Decay), decaySlider);
	sustainSliderAttach = std::make_unique<APVTS::SliderAttachment>(apvts, params.at(Names::Envelope_Sustain), sustainSlider);
	releaseSliderAttach = std::make_unique<APVTS::SliderAttachment>(apvts, params.at(Names::Envelope_Release), releaseSlider);

	cutoffSliderAttach = std::make_unique<APVTS::SliderAttachment>(apvts, params.at(Names::Filter_Cutoff), cutoffSlider);
	resonanceSliderAttach = std::make_unique<APVTS::SliderAttachment>(apvts, params.at(Names::Filter_Resonance), resonanceSlider);
	filterBypassButtonAttach = std::make_unique<APVTS::ButtonAttachment>(apvts, params.at(Names::Filter_Bypass), filterBypassButton);

	//Button listeners
	addPartial.addListener(this);
	addPartial.setTooltip("Add Partial");
	subtractPartial.addListener(this);
	subtractPartial.setTooltip("Remove Partial");

	//Customise controls
	masterGainSlider.setSliderStyle(juce::Slider::LinearHorizontal);
	masterGainSlider.setTextBoxStyle(juce::Slider::NoTextBox, true,0,0);
	masterGainSlider.setTooltip(params.at(Names::Master_Gain));

	attackSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
	attackSlider.setTooltip(params.at(Names::Envelope_Attack));
	decaySlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
	decaySlider.setTooltip(params.at(Names::Envelope_Decay));
	sustainSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
	sustainSlider.setTooltip(params.at(Names::Envelope_Sustain));
	releaseSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
	releaseSlider.setTooltip(params.at(Names::Envelope_Release));

	cutoffSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
	cutoffSlider.setTooltip(params.at(Names::Filter_Cutoff));
	resonanceSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
	resonanceSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
	resonanceSlider.setTooltip(params.at(Names::Filter_Resonance));
	filterBypassButton.setTooltip(params.at(Names::Filter_Bypass));

	//Add and make visible
	addAndMakeVisible(masterGainSlider);

	addAndMakeVisible(addPartial);
	addAndMakeVisible(subtractPartial);

	addAndMakeVisible(attackSlider);
	addAndMakeVisible(decaySlider);
	addAndMakeVisible(sustainSlider);
	addAndMakeVisible(releaseSlider);

	addAndMakeVisible(cutoffSlider);
	addAndMakeVisible(resonanceSlider);
	addAndMakeVisible(filterBypassButton);

	//Partials controls
	for (int i = 0; i < MAX_PARTIALS; i++) {
		//Attachments
		partialSpacesSliderAttaches[i] = std::make_unique<APVTS::SliderAttachment>(apvts,
																					   params.at(Names::Partial_Distance) + juce::String(i+1),
																					   partialSpacesSliders[i]);
		partialVolumesSliderAttaches[i] = std::make_unique<APVTS::SliderAttachment>(apvts,
																					  params.at(Names::Partial_Volume) + juce::String(i+1),
																					  partialVolumesSliders[i]);
		partialBypassButtonAttaches[i] = std::make_unique<APVTS::ButtonAttachment>(apvts,
																					 params.at(Names::Partial_Bypass) + juce::String(i+1),
																					 partialBypassButtons[i]);

		//Designs
		partialSpacesSliders[i].setSliderStyle(juce::Slider::LinearHorizontal);
		partialSpacesSliders[i].setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
		partialSpacesSliders[i].setTooltip("Distance from Fundamental");
		partialVolumesSliders[i].setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
		partialVolumesSliders[i].setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
		partialVolumesSliders[i].setTooltip("Partial Volume");
		partialBypassButtons[i].setTooltip("Mute Partial");
		
		//Disable unused partials
		if (i >= NUM_PARTIALS) {
			partialSpacesSliders[i].setEnabled(false);
			partialVolumesSliders[i].setEnabled(false);
			partialBypassButtons[i].setEnabled(false);
		}

		//Add, make visible? 
		addAndMakeVisible(partialSpacesSliders[i]);
		addAndMakeVisible(partialVolumesSliders[i]);
		addAndMakeVisible(partialBypassButtons[i]);
	}
}

AdditiveSynth1AudioProcessorEditor::~AdditiveSynth1AudioProcessorEditor()
{
	numberOfPartials = nullptr;
}

//==============================================================================
void AdditiveSynth1AudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (juce::Colours::black);

    g.setColour (juce::Colours::white);
    g.setFont (20.0f);
    //g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);

	//g.setColour(juce::Colours::red);

	auto bounds = getLocalBounds();
	auto top = bounds.removeFromTop(50);
	auto middle = bounds.removeFromTop(150);
	auto bottom = bounds;

	auto envelopeBounds = bottom.removeFromLeft(300);
	auto filterBounds = bottom;

	g.drawRect(top, 2);
	g.drawRect(middle, 2);
	g.drawRect(envelopeBounds, 2);
	g.drawRect(filterBounds, 2);

	auto labelBounds = top.removeFromLeft(200).reduced(10,0);
	g.drawFittedText("AddiSynth1", labelBounds, juce::Justification::centred, 1);
}

void AdditiveSynth1AudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
	using namespace juce;

	auto bounds = getLocalBounds();
	auto top = bounds.removeFromTop(50);
	auto middle = bounds.removeFromTop(150);
	auto bottom = bounds;

	//Top: Master gain on right side
	auto masterGainBounds = top.removeFromRight(100).reduced(2,2);
	masterGainSlider.setBounds(masterGainBounds);

	//Middle: Partials controls - Spacing, Volume, bypass, add/subtract partial
	//Buttons to add and subtract partials on the right, 
	auto partialButtonsBounds = middle.removeFromRight(50);
	auto addPartialBounds = partialButtonsBounds.removeFromTop(50).reduced(10);
	auto subtractPartialBounds = partialButtonsBounds.removeFromBottom(50).reduced(10);

	addPartial.setBounds(addPartialBounds);
	subtractPartial.setBounds(subtractPartialBounds);

	
	auto partialWidth = middle.getWidth() / (float)MAX_PARTIALS;


	for (int i = 0; i < MAX_PARTIALS; i++) {
		auto partialBounds = middle.removeFromLeft(partialWidth);

		auto partialVolumeBounds = partialBounds.removeFromTop(80);
		auto patialSpaceBounds = partialBounds.removeFromTop(40);
		auto partialBypassBounds = partialBounds;

		partialVolumesSliders[i].setBounds(partialVolumeBounds);
		partialSpacesSliders[i].setBounds(patialSpaceBounds);
		partialBypassButtons[i].setBounds(partialBypassBounds);
	}

	//Bottom: Envelop and filter
	//Envelop controls on left, filter on right
	auto envelopeBounds = bottom.removeFromLeft(300);
	auto filterBounds = bottom;

	auto envelopeControlsWidth = envelopeBounds.getWidth() / 4.0f;

	auto attackBounds = envelopeBounds.removeFromLeft(envelopeControlsWidth);
	auto decayBounds = envelopeBounds.removeFromLeft(envelopeControlsWidth);
	auto sustainBounds = envelopeBounds.removeFromLeft(envelopeControlsWidth);
	auto releaseBounds = envelopeBounds;

	attackSlider.setBounds(attackBounds);
	decaySlider.setBounds(decayBounds);
	sustainSlider.setBounds(sustainBounds);
	releaseSlider.setBounds(releaseBounds);

	auto filterControlsWidth = filterBounds.getWidth() / 3.0f;
	auto filterCutoffBounds = filterBounds.removeFromLeft(filterControlsWidth);
	auto filterResonanceBounds = filterBounds.removeFromLeft(filterControlsWidth);
	auto filterBypassBounds = filterBounds;

	cutoffSlider.setBounds(filterCutoffBounds);
	resonanceSlider.setBounds(filterResonanceBounds);
	filterBypassButton.setBounds(filterBypassBounds);
}

void AdditiveSynth1AudioProcessorEditor::buttonClicked(juce::Button* button)
{
	using namespace Params;
	auto params = getParams();

	numberOfPartials = dynamic_cast<juce::AudioParameterInt*>(audioProcessor.apvts.getParameter(params.at(Names::Num_Partials)));
	int numPartials = numberOfPartials->get();

	if (button == &addPartial) {
		if (numPartials < MAX_PARTIALS) {
			DBG("Add Partial");
			auto newVal = (float)(++currentNumPartials) / MAX_PARTIALS;
			DBG(newVal);
			numberOfPartials->setValueNotifyingHost( newVal);
		}
	}
	else if (button == &subtractPartial) {
		if (numPartials > 0) {
			DBG("Remove Partial");
			auto newVal = (float)(--currentNumPartials) / MAX_PARTIALS;
			DBG(newVal);
			numberOfPartials->setValueNotifyingHost( newVal);
		}
	}

	numberOfPartials = nullptr;

	updatePartialControls();
}

void AdditiveSynth1AudioProcessorEditor::updatePartialControls() {
	for (int i = 0; i < MAX_PARTIALS; i++) {
		partialSpacesSliders[i].setEnabled(i>=currentNumPartials ? false : true);
		partialVolumesSliders[i].setEnabled(i >= currentNumPartials ? false : true);
		partialBypassButtons[i].setEnabled(i >= currentNumPartials ? false : true);
	}
}
