/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "vst_decoder_internal.h"
#include "vst_decoder_internal.c"
#include "vst_decoder.h"

//==============================================================================
                                                                                                        //Santi: Constructor of the class 'AudioProcessor'. Here, the new number of inputs and outputs must be set, as it is a plug-in dedicated to Ambisonic signals.
                                                                                                        //All the parameters that need to be inicialized, they do it here. It happens once the plugin or the app is inicialized.
//VstdecoderAudioProcessor::VstdecoderAudioProcessor()
//#ifndef JucePlugin_PreferredChannelConfigurations
//     : AudioProcessor (BusesProperties()
//                     #if ! JucePlugin_IsMidiEffect
//                      #if ! JucePlugin_IsSynth
//                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
//                      #endif
//                       .withOutput ("Output", AudioChannelSet::stereo(), true)
//                     #endif
//                       )
//#endif
//{
//}

VstdecoderAudioProcessor::VstdecoderAudioProcessor() :
    AudioProcessor(BusesProperties()
        .withInput("Input", AudioChannelSet::discreteChannels(64), true)
        .withOutput("Output", AudioChannelSet::discreteChannels(64), true))
{
    vst_decoder_create(&hAmbi);
    startTimer(TIMER_PROCESSING_RELATED, 80);
}


VstdecoderAudioProcessor::~VstdecoderAudioProcessor()                                                   //Santi: Destructor of the class 'AudioProcessor'. Here is where things get shut down. If we have pointers that need to be destroyed 
{
}

float VstdecoderAudioProcessor::getParameter(int index)
{
    return 0;
    ///* standard parameters */
    //if (index < k_NumOfParameters) {
    //    switch (index) {
    //    case k_inputOrder:      return (float)(vst_decoder_getMasterDecOrder(hAmbi) - 1) / (float)(MAX_SH_ORDER - 1);
    //    case k_channelOrder:    return (float)(vst_decoder_getChOrder(hAmbi) - 1) / (float)(NUM_CH_ORDERINGS - 1);
    //    case k_normType:        return (float)(vst_decoder_getNormType(hAmbi) - 1) / (float)(NUM_NORM_TYPES - 1);
    //    case k_numLoudspeakers: return (float)(vst_decoder_getNumLoudspeakers(hAmbi)) / (float)(MAX_NUM_OUTPUTS);
    //    case k_decMethod1:      return (float)(vst_decoder_getDecMethod(hAmbi, 0) - 1) / (float)(vst_decoder_NUM_DECODING_METHODS - 1);
    //    case k_decMethod2:      return (float)(vst_decoder_getDecMethod(hAmbi, 1) - 1) / (float)(vst_decoder_NUM_DECODING_METHODS - 1);
    //    case k_maxREweight1:    return (float)(vst_decoder_getDecEnableMaxrE(hAmbi, 0));
    //    case k_maxREweight2:    return (float)(vst_decoder_getDecEnableMaxrE(hAmbi, 1));
    //    case k_diffEQ1:         return (float)(vst_decoder_getDecNormType(hAmbi, 0) - 1);
    //    case k_diffEQ2:         return (float)(vst_decoder_getDecNormType(hAmbi, 1) - 1);
    //    case k_transitionFreq:  return (vst_decoder_getTransitionFreq(hAmbi) - vst_decoder_TRANSITION_MIN_VALUE) / (vst_decoder_TRANSITION_MAX_VALUE - vst_decoder_TRANSITION_MIN_VALUE);
    //    case k_binauraliseLS:   return (float)(vst_decoder_getBinauraliseLSflag(hAmbi));
    //    default: return 0.0f;
    //    }
    //}
    ///* loudspeaker direction parameters */
    //else {
    //    index -= k_NumOfParameters;
    //    if (!(index % 2))
    //        return (vst_decoder_getLoudspeakerAzi_deg(hAmbi, index / 2) / 360.0f) + 0.5f;
    //    else
    //        return (vst_decoder_getLoudspeakerElev_deg(hAmbi, (index - 1) / 2) / 180.0f) + 0.5f;
    //}
}

void VstdecoderAudioProcessor::setParameter(int index, float newValue)
{}

//void VstdecoderAudioProcessor::setParameter(int index, float newValue)
//{
//    /* standard parameters */
//    if (index < k_NumOfParameters) {
//        switch (index) {
//        case k_inputOrder:      vst_decoder_setMasterDecOrder(hAmbi, (SH_ORDERS)(int)(newValue * (float)(MAX_SH_ORDER - 1) + 1.5f));
//            vst_decoder_setDecOrderAllBands(hAmbi, (newValue * (float)(MAX_SH_ORDER - 1) + 1.5f)); break;
//        case k_channelOrder:    vst_decoder_setChOrder(hAmbi, (int)(newValue * (float)(NUM_CH_ORDERINGS - 1) + 1.5f)); break;
//        case k_normType:        vst_decoder_setNormType(hAmbi, (int)(newValue * (float)(NUM_NORM_TYPES - 1) + 1.5f)); break;
//        case k_numLoudspeakers: vst_decoder_setNumLoudspeakers(hAmbi, (int)(newValue * (float)(MAX_NUM_OUTPUTS)+0.5)); break;
//        case k_decMethod1:      vst_decoder_setDecMethod(hAmbi, 0, (vst_decoder_DECODING_METHODS)(int)(newValue * (float)(vst_decoder_NUM_DECODING_METHODS - 1) + 1.5f)); break;
//        case k_decMethod2:      vst_decoder_setDecMethod(hAmbi, 1, (vst_decoder_DECODING_METHODS)(int)(newValue * (float)(vst_decoder_NUM_DECODING_METHODS - 1) + 1.5f)); break;
//        case k_maxREweight1:    vst_decoder_setDecEnableMaxrE(hAmbi, 0, (int)(newValue + 0.5f)); break;
//        case k_maxREweight2:    vst_decoder_setDecEnableMaxrE(hAmbi, 1, (int)(newValue + 0.5f)); break;
//        case k_diffEQ1:         vst_decoder_setDecNormType(hAmbi, 0, (int)(newValue + 1.5f)); break;
//        case k_diffEQ2:         vst_decoder_setDecNormType(hAmbi, 0, (int)(newValue + 1.5f)); break;
//        case k_transitionFreq:  vst_decoder_setTransitionFreq(hAmbi, newValue * (vst_decoder_TRANSITION_MAX_VALUE - vst_decoder_TRANSITION_MIN_VALUE) + vst_decoder_TRANSITION_MIN_VALUE); break;
//        case k_binauraliseLS:   vst_decoder_setBinauraliseLSflag(hAmbi, (int)(newValue + 0.5f)); break;
//        }
//    }
//    /* loudspeaker direction parameters */
//    else {
//        index -= k_NumOfParameters;
//        float newValueScaled;
//        if (!(index % 2)) {
//            newValueScaled = (newValue - 0.5f) * 360.0f;
//            if (newValueScaled != vst_decoder_getLoudspeakerAzi_deg(hAmbi, index / 2)) {
//                vst_decoder_setLoudspeakerAzi_deg(hAmbi, index / 2, newValueScaled);
//            }
//        }
//        else {
//            newValueScaled = (newValue - 0.5f) * 180.0f;
//            if (newValueScaled != vst_decoder_getLoudspeakerElev_deg(hAmbi, index / 2)) {
//                vst_decoder_setLoudspeakerElev_deg(hAmbi, index / 2, newValueScaled);
//            }
//        }
//    }
//}

//==============================================================================
                                                                                                        //Santi: 'getName()'. Returns the name of this processor. 
const String VstdecoderAudioProcessor::getName() const
{
    return JucePlugin_Name;
}
                                                                                                        //Santi: 'acceptsMidi()'. Returns true if the processor wants MIDI messages. 
bool VstdecoderAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}
                                                                                                        //Santi: 'producesMidi()'. Returns true if the processor produces MIDI messages. 
bool VstdecoderAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}
                                                                                                        //Santi: 'isMidiEffect()'. Returns true if this is a MIDI effect plug-in and does no audio processing. 
bool VstdecoderAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}
                                                                                                        //Santi: 'getTailLengthSeconds()'. Returns the length of the processor's tail, in seconds.
double VstdecoderAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}
                                                                                                        //Santi: 'getNumPrograms()'. Returns the number of preset programs the processor supports.
                                                                                                                                    //The value returned must be valid as soon as this object is created, and must not change over its lifetime.
                                                                                                                                    //This value shouldn't be less than 1. 
int VstdecoderAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}
                                                                                                        //Santi: 'getCurrentProgram()'. Returns the number of the currently active program. 
int VstdecoderAudioProcessor::getCurrentProgram()
{
    return 0;
}
                                                                                                        //Santi: 'setCurrentProgram()'. Called by the host to change the current program. 
void VstdecoderAudioProcessor::setCurrentProgram (int index)
{
}
                                                                                                        //Santi: 'getProgramName()'. Must return the name of a given program. 
const String VstdecoderAudioProcessor::getProgramName (int index)
{
    return {};
}
                                                                                                        //Santi: 'changeProgramName()'. Called by the host to rename a program. 
void VstdecoderAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
                                                                                                        //Santi: 'prepareToPlay()'. It is called before the playback to prepare correctly the audio processor. From this function we can call the methods like ‘getTotalNumInputChannels()’ and ‘getTotalNumOutputChannels()’
void VstdecoderAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

    nHostBlockSize = samplesPerBlock;                                                                   //Santi: 1024 samplesPerBlock
    nNumInputs = getTotalNumInputChannels();                                                            //Santi: 2
    nNumOutputs = getTotalNumOutputChannels();                                                          //Santi: 2
    nSampleRate = (int)(sampleRate + 0.5);                                                              //Santi: 44100Hz
    
    //vst_decoder_init(hAmbi, nSampleRate);
    //AudioProcessor::setLatencySamples(vst_decoder_getProcessingDelay());



    nNumInputs; //Santi: Delete this line (I use it just for test and stop the Debug)
}

void VstdecoderAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations                                                       //Santi: 'isBusesLayoutSupported()'. This method checks if the number of bus channels that is working is correct. In case of being working with Ambisonic signals of 3rd order, we must indicate it in this Method.
bool VstdecoderAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
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
                                                                                                        //Santi: 'processBlock()'. Most important method. It makes the processing of every audio blocks. In this method, we must make the programming that we would likre to apply to the audio.
                                                                                                                //It receives as input a a pointer to the audio buffer and other to the Midi buffer.
void VstdecoderAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();                                           //Santi: 'getTotalNumInputChannels()'. This method returns the total number of input channels.
    auto totalNumOutputChannels = getTotalNumOutputChannels();                                          //Santi: 'getTotalNumOutputChannels()'.This method returns the total number of output channels.

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());                                                    //Santi: Returns the number of samples allocated in each of the buffer's channels.

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);                                           //Santi: Returns a writeable pointer to one of the buffer's channels.

        // ..do something to the data...
    }
}

//==============================================================================
bool VstdecoderAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* VstdecoderAudioProcessor::createEditor()
{
    return new VstdecoderAudioProcessorEditor (*this);
}

//void PluginProcessor::saveConfigurationToFile(File destination)
//{
//
//}

//==============================================================================
                                                                                                        //Santi: 'get StateInformation()'. It basically takes a snapshot of all the parameters, all of the stuff that's happening in our plugin.                
                                                                                                                //This method takes the snapshot of where those parameters are set, saves itto an XML file and then converts it to a binary file.
void VstdecoderAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}
                                                                                                        //Santi: 'setStateInformation()'. This must restore the processor's state from a block of data previously created using getStateInformation(). 
void VstdecoderAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..                                                           //Santi: 'createPluginFilter()'. It returns a new instance of the filter subclass that we are building.
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new VstdecoderAudioProcessor();
}


/* Adapted from the AllRADecoder by Daniel Rudrich, (c) 2017 (GPLv3 license) */
//void PluginProcessor::saveConfigurationToFile(File destination)
//{
//    loudspeakers.removeAllChildren(nullptr);
//    for (int i = 0; i < vst_decoder_getNumLoudspeakers(hAmbi); i++)
//    {
//        loudspeakers.appendChild(ConfigurationHelper::
//            createElement(vst_decoder_getLoudspeakerAzi_deg(hAmbi, i),
//                vst_decoder_getLoudspeakerElev_deg(hAmbi, i),
//                1.0f,
//                i + 1,
//                false,
//                1.0f), nullptr);
//    }
//    DynamicObject* jsonObj = new DynamicObject();
//    jsonObj->setProperty("Name", var("SPARTA AmbiDEC loudspeaker directions."));
//    char versionString[10];
//    strcpy(versionString, "v");
//    strcat(versionString, JucePlugin_VersionString);
//    jsonObj->setProperty("Description", var("This configuration file was created with the SPARTA AmbiDEC " + String(versionString) + " plug-in. " + Time::getCurrentTime().toString(true, true)));
//    jsonObj->setProperty("LoudspeakerLayout", ConfigurationHelper::convertLoudspeakersToVar(loudspeakers, "Loudspeaker Directions"));
//    Result result = ConfigurationHelper::writeConfigurationToFile(destination, var(jsonObj));
//}
//
///* Adapted from the AllRADecoder by Daniel Rudrich, (c) 2017 (GPLv3 license) */
//void PluginProcessor::loadConfiguration(const File& configFile)
//{
//    int channelIDs[MAX_NUM_CHANNELS + 1] = { 0 };
//    int virtual_channelIDs[MAX_NUM_CHANNELS + 1] = { 0 };
//    loudspeakers.removeAllChildren(nullptr);
//    Result result = ConfigurationHelper::parseFileForLoudspeakerLayout(configFile, loudspeakers, nullptr);
//    if (result.wasOk()) {
//        int num_ls, num_virtual_ls, ls_idx, jj;
//        num_ls = num_virtual_ls = ls_idx = jj = 0;
//        /* get Channel IDs and find number of directions and virtual directions */
//        for (ValueTree::Iterator it = loudspeakers.begin(); it != loudspeakers.end(); ++it) {
//            if (!((*it).getProperty("Imaginary"))) {
//                num_ls++; channelIDs[jj] = (*it).getProperty("Channel");
//            }
//            else {
//                virtual_channelIDs[num_virtual_ls] = (*it).getProperty("Channel");
//                num_virtual_ls++; channelIDs[jj] = -1;
//            }
//            jj++;
//        }
//        /* remove virtual channels and shift the channel indices down */
//        for (int i = 0; i < num_virtual_ls; i++)
//            for (int j = 0; j < num_ls + num_virtual_ls; j++)
//                if (channelIDs[j] == -1)
//                    for (int k = j; k < num_ls + num_virtual_ls; k++)
//                        channelIDs[k] = channelIDs[k + 1];
//
//        /* then decriment the channel IDs to remove the gaps */
//        for (int i = 0; i < num_virtual_ls; i++)
//            for (int j = 0; j < num_ls + num_virtual_ls; j++)
//                if (channelIDs[j] > virtual_channelIDs[i] - i)
//                    channelIDs[j]--;
//
//        /* update with the new configuration  */
//        vst_decoder_setNumLoudspeakers(hAmbi, num_ls);
//        for (ValueTree::Iterator it = loudspeakers.begin(); it != loudspeakers.end(); ++it) {
//            if (!((*it).getProperty("Imaginary"))) {
//                vst_decoder_setLoudspeakerAzi_deg(hAmbi, channelIDs[ls_idx] - 1, (*it).getProperty("Azimuth"));
//                vst_decoder_setLoudspeakerElev_deg(hAmbi, channelIDs[ls_idx] - 1, (*it).getProperty("Elevation"));
//                ls_idx++;
//            }
//        }
//    }
//}
    

const String VstdecoderAudioProcessor::getInputChannelName(int channelIndex) const                      //Santi: Returns the names of all the available input channels on this device. 
{
    return String(channelIndex + 1);
}

const String VstdecoderAudioProcessor::getOutputChannelName(int channelIndex) const                     //Santi: Returns the names of all the available output channels on this device. 
{
    return String(channelIndex + 1);
}


