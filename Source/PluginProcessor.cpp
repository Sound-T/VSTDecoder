/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
VstdecoderAudioProcessor::VstdecoderAudioProcessor()                                        //Santi: Constructor of the class 'AudioProcessor'
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

VstdecoderAudioProcessor::~VstdecoderAudioProcessor()                                       //Santi: Destructor of the class 'AudioProcessor'                                     
{
}

//==============================================================================
const String VstdecoderAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool VstdecoderAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool VstdecoderAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool VstdecoderAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double VstdecoderAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int VstdecoderAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int VstdecoderAudioProcessor::getCurrentProgram()
{
    return 0;
}

void VstdecoderAudioProcessor::setCurrentProgram (int index)
{
}

const String VstdecoderAudioProcessor::getProgramName (int index)
{
    return {};
}

void VstdecoderAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void VstdecoderAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
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
                                                                                                        //      It receives as input a a pointer to the audio buffer and other to the Midi buffer.
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
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

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
void VstdecoderAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void VstdecoderAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new VstdecoderAudioProcessor();
}


/* Adapted from the AllRADecoder by Daniel Rudrich, (c) 2017 (GPLv3 license) */
//void PluginProcessor::saveConfigurationToFile(File destination)
//{
//    loudspeakers.removeAllChildren(nullptr);
//    for (int i = 0; i < ambi_dec_getNumLoudspeakers(hAmbi); i++)
//    {
//        loudspeakers.appendChild(ConfigurationHelper::
//            createElement(ambi_dec_getLoudspeakerAzi_deg(hAmbi, i),
//                ambi_dec_getLoudspeakerElev_deg(hAmbi, i),
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
//        ambi_dec_setNumLoudspeakers(hAmbi, num_ls);
//        for (ValueTree::Iterator it = loudspeakers.begin(); it != loudspeakers.end(); ++it) {
//            if (!((*it).getProperty("Imaginary"))) {
//                ambi_dec_setLoudspeakerAzi_deg(hAmbi, channelIDs[ls_idx] - 1, (*it).getProperty("Azimuth"));
//                ambi_dec_setLoudspeakerElev_deg(hAmbi, channelIDs[ls_idx] - 1, (*it).getProperty("Elevation"));
//                ls_idx++;
//            }
//        }
//    }
//}
