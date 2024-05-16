/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "GlobalDefines.h"

//==============================================================================
/**
*/
class AdditiveSynth1AudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    AdditiveSynth1AudioProcessor();
    ~AdditiveSynth1AudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

	APVTS apvts;

private:
	juce::AudioParameterFloat* masterGain{ nullptr };
	juce::AudioParameterFloat* filterCutoff{ nullptr };
	juce::AudioParameterFloat* filterResonance{ nullptr };
	juce::AudioParameterBool* filterBypass{ nullptr };

	juce::Synthesiser synth;
	juce::dsp::Gain<float> gain;
	juce::dsp::StateVariableTPTFilter<float> filter;

	APVTS::ParameterLayout getLayout();
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AdditiveSynth1AudioProcessor)
};
