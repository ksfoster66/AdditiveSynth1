/*
  ==============================================================================

    SynthVoice.h
    
	Implementation of juce::SynthesiserVoice

	Contains all information related to specific voices of the synthesiser

  ==============================================================================
*/

#pragma once
#include "../GlobalDefines.h"
#include "SynthSound.h"
#include <array>

#define HARMONICS 3 

class SynthVoice : public juce::SynthesiserVoice {
public:
	~SynthVoice();
	bool canPlaySound(juce::SynthesiserSound*) override;
	void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition) override;
	void stopNote(float velocity, bool allowTailOff) override;
	void pitchWheelMoved(int newPitchWheelValue) override;
	void controllerMoved(int controllerNumber, int newControllerValue) override;
	void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;

	void prepareToPlay(juce::dsp::ProcessSpec& spec);
	void initialise(APVTS& apvts);
private:
	float velocity;
	std::array<float, MAX_PARTIALS+1> frequencies{};
	std::array<float, MAX_PARTIALS+1> volumes{ 1 };
	std::array<float, MAX_PARTIALS+1> volumeWeights{1};
	std::array<bool, MAX_PARTIALS+1> isBypassed{ false };

	std::array<float, MAX_PARTIALS+1> deltas{};
	std::array<float, MAX_PARTIALS+1> currentPos{};

	double sampleRate;
	int numberOfPartials;

	juce::AudioParameterInt* numberOfPartialsParam;

	//Partials are offset by one, ie the 0th position is 1
	std::array<juce::AudioParameterFloat*, MAX_PARTIALS> partial_spaces{nullptr};
	std::array<juce::AudioParameterFloat*, MAX_PARTIALS> partial_volumes{nullptr};
	std::array<juce::AudioParameterBool*, MAX_PARTIALS> partial_bypass{nullptr};

	juce::ADSR adsr;
	juce::AudioParameterFloat* attackParam{ nullptr };
	juce::AudioParameterFloat* decayParam{ nullptr };
	juce::AudioParameterFloat* sustainParam{ nullptr };
	juce::AudioParameterFloat* releaseParam{ nullptr };

	void updateParams();

	//To implement Wavetable lookup..
	SynthSound* synthSound = nullptr;
};