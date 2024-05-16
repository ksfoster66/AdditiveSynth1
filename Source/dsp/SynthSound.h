/*
  ==============================================================================

    SynthSound.h

	Implementation of juce::SynthesiserSound

	Probably will end up holding global/shared information, like computed wavetables

  ==============================================================================
*/

#pragma once
#include "../GlobalDefines.h"

class SynthSound : public juce::SynthesiserSound {
public:
	SynthSound()
	{
		generateTable(TABLE_SIZE);
		DBG("Sound created");
	}
	bool appliesToNote(int midiNoteNumber) override { return true; }
	bool appliesToChannel(int midiChannel) override { return true; }

	int getTableSize() { return TABLE_SIZE; }

	float lookup(float index);

private:

	float table[TABLE_SIZE+1];

	void generateTable(int length);
};