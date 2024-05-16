/*
  ==============================================================================

    SynthVoice.cpp
    Created: 13 May 2024 1:21:00pm
    Author:  ksfos

  ==============================================================================
*/

#include "SynthVoice.h"

SynthVoice::~SynthVoice() {
	synthSound = nullptr;
	DBG("Constructed Voice");
}

bool SynthVoice::canPlaySound(juce::SynthesiserSound*)
{
    return true;
}

void SynthVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition)
{
	synthSound = dynamic_cast<SynthSound*>(sound);

	jassert(synthSound != nullptr);

	this->velocity = velocity;
	float frequency = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);

	frequencies[0] = frequency;
	for (int i = 0; i <= numberOfPartials; i++) {
		currentPos[i] = 0;
	}
	
	updateParams();

	adsr.noteOn();
}

void SynthVoice::stopNote(float velocity, bool allowTailOff)
{
	adsr.noteOff();
}

void SynthVoice::pitchWheelMoved(int newPitchWheelValue)
{
}

void SynthVoice::controllerMoved(int controllerNumber, int newControllerValue)
{
}

void SynthVoice::initialise(APVTS& apvts) {
	using namespace Params;
	using APFloat = juce::AudioParameterFloat;
	using APBool = juce::AudioParameterBool;
	using APInt = juce::AudioParameterInt;

	auto params = getParams();

	//Set number of initial harmonies
	numberOfPartialsParam = dynamic_cast<APInt*>(apvts.getParameter(params.at(Names::Num_Partials)));
	numberOfPartials = numberOfPartialsParam->get();

	//Hook up partial controls
	for (int i = 0; i < MAX_PARTIALS; i++) {
		partial_spaces[i] = dynamic_cast<APFloat*>(apvts.getParameter(params.at(Names::Partial_Distance) + juce::String(i+1)));
		partial_volumes[i] = dynamic_cast<APFloat*>(apvts.getParameter(params.at(Names::Partial_Volume) + juce::String(i+1)));
		partial_bypass[i]=dynamic_cast<APBool*>(apvts.getParameter(params.at(Names::Partial_Bypass) + juce::String(i+1)));

		volumeWeights[i+1] = sqrt(i + 2);
	}

	//Envelope initialisation
	attackParam = dynamic_cast<APFloat*>(apvts.getParameter(params.at(Names::Envelope_Attack)));
	decayParam = dynamic_cast<APFloat*>(apvts.getParameter(params.at(Names::Envelope_Decay)));
	sustainParam = dynamic_cast<APFloat*>(apvts.getParameter(params.at(Names::Envelope_Sustain)));
	releaseParam = dynamic_cast<APFloat*>(apvts.getParameter(params.at(Names::Envelope_Release)));

	DBG("Initialised Voice");
}

void SynthVoice::prepareToPlay(juce::dsp::ProcessSpec& spec) {
	sampleRate = spec.sampleRate;
	adsr.setSampleRate(sampleRate);
	DBG("Voice is prepared to play");
}

void SynthVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
	juce::AudioBuffer<float> buffer{ outputBuffer.getNumChannels(), outputBuffer.getNumSamples()};
	buffer.clear();

	updateParams();

	//float max = 0;

	for (int sample = startSample; sample < numSamples; sample++) {
		
		double val = 0;
		if (adsr.isActive() && synthSound != nullptr) {
			val += synthSound->lookup(currentPos[0]);
			for (int i = 1; i <= numberOfPartials; i++)
				if (!isBypassed[i])
					val += synthSound->lookup(currentPos[i]) * (volumes[i] / volumeWeights[i]);
			val *= velocity;
		}
		//DBG(val);
		//if (val > max) max = val;

		
		for (int channel = 0; channel < buffer.getNumChannels(); channel++) {
			buffer.addSample(channel, sample, val);
		}

		for (int i = 0; i <= numberOfPartials; i++) {
			currentPos[i] += deltas[i];
			if (currentPos[i] > TABLE_SIZE) currentPos[i] -= TABLE_SIZE;
		}
	}

	adsr.applyEnvelopeToBuffer(buffer, startSample, numSamples);

	for (int channel = 0; channel < outputBuffer.getNumChannels(); channel++) {
		outputBuffer.addFrom(channel, startSample, buffer, channel, startSample, numSamples);
	}


	//if (max != 0)DBG(max);
}

void SynthVoice::updateParams() {
	numberOfPartials = numberOfPartialsParam->get();

	//gain.setGainLinear(masterGain->get());
	
	for (int i = 1; i <= numberOfPartials; i++) {
		//Partials are offset by one
		// If the frequency is being determined from the previous frequency use this 
		//frequencies[i] = frequencies[i - 1] * (1 + partial_spaces[i-1]->get());
		//If the frequency is determined from the fundamental, use this
		frequencies[i] = frequencies[0] * (1 + partial_spaces[i-1]->get());
		volumes[i] = partial_volumes[i-1]->get();
		isBypassed[i] = partial_bypass[i-1]->get();
	}

	juce::ADSR::Parameters adsrParams{attackParam->get() + 0.005f,
									  decayParam->get() + 0.005f,
									  sustainParam->get(),
									  releaseParam->get() + 0.005f};

	adsr.setParameters(adsrParams);

	for (int i = 0; i <= numberOfPartials; i++) {
		deltas[i] = (TABLE_SIZE * frequencies[i]) / sampleRate;
	}
}
