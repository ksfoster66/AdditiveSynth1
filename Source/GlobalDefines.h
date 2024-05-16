/*
  ==============================================================================

    GlobalDefines.h

	Parameters

	Todo: Num Harmonics

	Todo: Num Voices

	DSP:
	DONE Master volume
	DONE Distance between partials
	DONE Volume of partials
	DONE Bypass Partials
	Done? Envelope
	Done Filter
	DONE Add/Subtract Partials
	Add/subtract voices

	GUI:
	DONE Master volume
	DONE Distance between partials
	DONE Volume of partials
	DONE Bypass Partials
	DONE Envelope
	DONE Filter
	LookAndFeel

	TODOS:
		Fix artifacting/transiants
			Juce::ADSR pops when going to release early. Need a custom ADSR with better ramping?
		Gain ramping on the partial volumes.
			juce::dsp::Gain for each or custom ramp solution? Which is less process intensive
				Custom: Have a target Volume value, and a current value that is additively added to each cycle until it reaches the target
		Filter Type switching

		Add an envelope to the filter

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

using APVTS = juce::AudioProcessorValueTreeState;

#define TWOPI 6.283185

#define TABLE_SIZE 65536

#define NUM_PARTIALS 4
#define MAX_PARTIALS 8

#define NUM_VOICES 8
#define MAX_VOICES 8

#define MASTER_GAIN_DEF 0.8f
#define MASTER_GAIN_MAX 1.0f
#define MASTER_GAIN_MIN 0.0f
#define MASTER_GAIN_STEP 0.01f

#define PARTIAL_DISTANCE_DEF 0.5f
#define PARTIAL_DISTANCE_MAX 1.0f
#define PARTIAL_DISTANCE_MIN 0.01f
#define PARTIAL_DISTANCE_STEP 0.01f

#define PARTIAL_VOLUME_DEF 0.8f
#define PARTIAL_VOLUME_MAX 1.0f
#define PARTIAL_VOLUME_MIN 0.0f
#define PARTIAL_VOLUME_STEP 0.01f

#define PARTIAL_BYPASS_DEF false

#define ATTACK_DEF 0.5f
#define ATTACK_MAX 5.0f
#define ATTACK_MIN 0.005f
#define ATTACK_STEP 0.001f

#define DECAY_DEF 0.5f
#define DECAY_MAX 2.0f
#define DECAY_MIN 0.005f
#define DECAY_STEP 0.001f

#define SUSTAIN_DEF 0.8f
#define SUSTAIN_MAX 1.0f
#define SUSTAIN_MIN 0.0f
#define SUSTAIN_STEP 0.01f

#define RELEASE_DEF 0.5f
#define RELEASE_MAX 5.0f
#define RELEASE_MIN 0.005f
#define RELEASE_STEP 0.001f

#define FILTER_CUTOFF_DEF 2000.0f
#define FILTER_CUTOFF_MAX 20000.0f
#define FILTER_CUTOFF_MIN 20.0f
#define FILTER_CUTOFF_STEP 0.1f

#define FILTER_RESONANCE_DEF 0.707f
#define FILTER_RESONANCE_MAX 1.0f
#define FILTER_RESONANCE_MIN 0.0f
#define FILTER_RESONANCE_STEP 0.001f


namespace Params {
	enum Names {
		Master_Gain,

		Partial_Distance,
		Partial_Volume,
		Partial_Bypass,

		Envelope_Attack,
		Envelope_Decay,
		Envelope_Sustain,
		Envelope_Release,

		Filter_Cutoff,
		Filter_Resonance,
		Filter_Bypass,
		Filter_Type,

		Num_Partials

	};

	inline const std::map<Names, juce::String>& getParams() {
		static std::map<Names, juce::String> params = {
			{Master_Gain, "Master Gain"},
			{Partial_Distance, "Partial Distance "},
			{Partial_Volume, "Partial Volume "},
			{Partial_Bypass, "Mute Partial "},

			{Envelope_Attack, "Envelope Attack"},
			{Envelope_Decay, "Envelope Decay"},
			{Envelope_Sustain, "Envelope Sustain"},
			{Envelope_Release, "Envelope Release"},

			{Filter_Cutoff, "Filter Cutoff"},
			{Filter_Resonance, "Filter Resonance"},
			{Filter_Bypass, "Filter Bypass"},
			{Filter_Type, "FilterType"},

			{Num_Partials, "Number of Partials"}
		};

		return params;
	}
}