/*
  ==============================================================================

    SynthSound.cpp
    Created: 14 May 2024 11:02:30am
    Author:  ksfos

  ==============================================================================
*/

#include "SynthSound.h"

float SynthSound::lookup(float index) {

	int lindex = (int)index;
	int rindex = lindex + 1;

	float lval = table[lindex];
	float rval = table[rindex];

	float dif = index - lindex;
	float height = rval - lval;

	float val = lval + dif * height;

	return val;
}

void SynthSound::generateTable(int length) {
	double step = TWOPI / length;

	for (int i = 0; i < length; i++) {
		table[i] = sin(step * i);
	}

	table[TABLE_SIZE] = table[0];
}