/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

#include "dsp/SynthSound.h"
#include "dsp/SynthVoice.h"

//==============================================================================
AdditiveSynth1AudioProcessor::AdditiveSynth1AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
	, apvts(*this, nullptr, "PARAMETERS", getLayout())
{
	synth.clearSounds();
	synth.addSound(new SynthSound());

	synth.clearVoices();
	for (int i = 0; i < NUM_VOICES; i++)
		synth.addVoice(new SynthVoice());

	for (int i = 0; i < synth.getNumVoices(); i++)
		dynamic_cast<SynthVoice*>(synth.getVoice(i))->initialise(apvts);

	using namespace Params;
	auto params = getParams();
	using APFloat = juce::AudioParameterFloat;
	using APBool = juce::AudioParameterBool;

	//Connect param references
	masterGain = dynamic_cast<APFloat*>(apvts.getParameter(params.at(Names::Master_Gain)));
	filterCutoff = dynamic_cast<APFloat*>(apvts.getParameter(params.at(Names::Filter_Cutoff)));
	filterResonance = dynamic_cast<APFloat*>(apvts.getParameter(params.at(Names::Filter_Resonance)));
	filterBypass = dynamic_cast<APBool*>(apvts.getParameter(params.at(Names::Filter_Bypass)));

	filter.setType(juce::dsp::StateVariableTPTFilterType::lowpass);

	DBG("Audio Processor Constructed");
}

AdditiveSynth1AudioProcessor::~AdditiveSynth1AudioProcessor()
{
}

//==============================================================================
const juce::String AdditiveSynth1AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AdditiveSynth1AudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AdditiveSynth1AudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AdditiveSynth1AudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AdditiveSynth1AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AdditiveSynth1AudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AdditiveSynth1AudioProcessor::getCurrentProgram()
{
    return 0;
}

void AdditiveSynth1AudioProcessor::setCurrentProgram (int index)
{
}

const juce::String AdditiveSynth1AudioProcessor::getProgramName (int index)
{
    return {};
}

void AdditiveSynth1AudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void AdditiveSynth1AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
	synth.setCurrentPlaybackSampleRate(sampleRate);

	juce::dsp::ProcessSpec spec;
	spec.sampleRate = sampleRate;
	spec.maximumBlockSize = samplesPerBlock;
	spec.numChannels = getNumOutputChannels();

	//Prepare the gain
	gain.prepare(spec);
	gain.setRampDurationSeconds(0.005);

	//Prepare the filter
	filter.prepare(spec);

	//Prepare all the voices
	for (int i = 0; i < synth.getNumVoices(); i++)
		dynamic_cast<SynthVoice*>(synth.getVoice(i))->prepareToPlay(spec);

	DBG("Audio Processor is prepared to play");
}

void AdditiveSynth1AudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool AdditiveSynth1AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void AdditiveSynth1AudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
	gain.setGainLinear(masterGain->get());
	filter.setCutoffFrequency(filterCutoff->get());
	filter.setResonance(filterResonance->get());

	synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());

	juce::dsp::AudioBlock<float> block{ buffer };
	auto ctx = juce::dsp::ProcessContextReplacing{ block };

	gain.process(ctx);
	if(!filterBypass->get()) filter.process(ctx);
}

//==============================================================================
bool AdditiveSynth1AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AdditiveSynth1AudioProcessor::createEditor()
{
	DBG("Creating Editor");
    return new AdditiveSynth1AudioProcessorEditor (*this);
	//return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void AdditiveSynth1AudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.

	juce::MemoryOutputStream mos(destData, true);
	apvts.state.writeToStream(mos);
}

void AdditiveSynth1AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.

	auto tree = juce::ValueTree::readFromData(data, sizeInBytes);
	if (tree.isValid()) {
		apvts.replaceState(tree);
	}
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AdditiveSynth1AudioProcessor();
}

APVTS::ParameterLayout AdditiveSynth1AudioProcessor::getLayout() {
	APVTS::ParameterLayout layout;

	using namespace Params;
	auto params = getParams();

	layout.add(std::make_unique<juce::AudioParameterFloat>(params.at(Names::Master_Gain), params.at(Names::Master_Gain),
														   juce::NormalisableRange(MASTER_GAIN_MIN, MASTER_GAIN_MAX, MASTER_GAIN_STEP), MASTER_GAIN_DEF));

	juce::NormalisableRange partialVolumeRange{ PARTIAL_VOLUME_MIN, PARTIAL_VOLUME_MAX, PARTIAL_VOLUME_STEP };

	for (int i = 0; i < MAX_PARTIALS; i++) {
		layout.add(std::make_unique<juce::AudioParameterFloat>(params.at(Names::Partial_Distance) + juce::String(i+1), 
															   params.at(Names::Partial_Distance) + juce::String(i+1),
															   //Frequency being determined from previous frequency
															   //juce::NormalisableRange(PARTIAL_DISTANCE_MIN, PARTIAL_DISTANCE_MAX, PARTIAL_DISTANCE_STEP), PARTIAL_DISTANCE_DEF));
															   //Frequency determined from fundamental
															   juce::NormalisableRange(PARTIAL_DISTANCE_MIN, PARTIAL_DISTANCE_MAX + i , PARTIAL_DISTANCE_STEP), PARTIAL_DISTANCE_DEF+i ));
		layout.add(std::make_unique<juce::AudioParameterFloat>(params.at(Names::Partial_Volume) + juce::String(i+1), 
															   params.at(Names::Partial_Volume) + juce::String(i+1),
															   partialVolumeRange, PARTIAL_VOLUME_DEF));
		layout.add(std::make_unique<juce::AudioParameterBool>(params.at(Names::Partial_Bypass) + juce::String(i+1), 
															  params.at(Names::Partial_Bypass) + juce::String(i+1), 
															  PARTIAL_BYPASS_DEF));
	}

	//Envelope params
	layout.add(std::make_unique<juce::AudioParameterFloat>(params.at(Names::Envelope_Attack), 
														   params.at(Names::Envelope_Attack),
														   juce::NormalisableRange(ATTACK_MIN, ATTACK_MAX, ATTACK_STEP), ATTACK_DEF));
	layout.add(std::make_unique<juce::AudioParameterFloat>(params.at(Names::Envelope_Decay), 
														   params.at(Names::Envelope_Decay),
														   juce::NormalisableRange(DECAY_MIN, DECAY_MAX, DECAY_STEP), DECAY_DEF));
	layout.add(std::make_unique<juce::AudioParameterFloat>(params.at(Names::Envelope_Sustain), 
														   params.at(Names::Envelope_Sustain),
														   juce::NormalisableRange(SUSTAIN_MIN, SUSTAIN_MAX, SUSTAIN_STEP), SUSTAIN_DEF));
	layout.add(std::make_unique<juce::AudioParameterFloat>(params.at(Names::Envelope_Release), 
														   params.at(Names::Envelope_Release),
														   juce::NormalisableRange(RELEASE_MIN, RELEASE_MAX, RELEASE_STEP), RELEASE_DEF));

	//Filter Params
	layout.add(std::make_unique<juce::AudioParameterFloat>(params.at(Names::Filter_Cutoff), 
														   params.at(Names::Filter_Cutoff),
														   juce::NormalisableRange(FILTER_CUTOFF_MIN, FILTER_CUTOFF_MAX, FILTER_CUTOFF_STEP), FILTER_CUTOFF_DEF));
	layout.add(std::make_unique<juce::AudioParameterFloat>(params.at(Names::Filter_Resonance), 
														   params.at(Names::Filter_Resonance),
														   juce::NormalisableRange(FILTER_RESONANCE_MIN, FILTER_RESONANCE_MAX, FILTER_RESONANCE_STEP), FILTER_RESONANCE_DEF));
	layout.add(std::make_unique<juce::AudioParameterBool>(params.at(Names::Filter_Bypass),
														  params.at(Filter_Bypass),
														  false));

	//Number of voices and partials. 
	layout.add(std::make_unique<juce::AudioParameterInt>(params.at(Names::Num_Partials),
														 params.at(Names::Num_Partials),
														 0, MAX_PARTIALS, NUM_PARTIALS));
	

	DBG("Parameter layout created");
	return layout;

}
