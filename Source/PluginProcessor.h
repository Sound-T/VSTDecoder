/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "vst_decoder.h"
#include <thread>
#define CONFIGURATIONHELPER_ENABLE_LOUDSPEAKERLAYOUT_METHODS 1
#define CONFIGURATIONHELPER_ENABLE_GENERICLAYOUT_METHODS 1
#include "ConfigurationHelper.h"
#define BUILD_VER_SUFFIX "" /* String to be added before the version name on the GUI (e.g. beta, alpha etc..) */ 

//==============================================================================
/**
*/

typedef enum _TIMERS {                                                                                //Santi: Unblock to declare the timers
    TIMER_PROCESSING_RELATED = 1,                                                                     // At the moment, this timers lead to some errors.
    TIMER_GUI_RELATED
}TIMERS;

/* Parameter tags: for the default VST GUI */
enum {
    k_inputOrder,
    k_channelOrder,
    k_normType,
    k_decMethod1,
    k_decMethod2,
    k_maxREweight1,
    k_maxREweight2,
    k_diffEQ1,
    k_diffEQ2,
    k_transitionFreq,
    k_binauraliseLS,
    k_numLoudspeakers,

    k_NumOfParameters
};



class VstdecoderAudioProcessor : public AudioProcessor,                                                 //Santi Class 'AudioProcessor'
                                 public MultiTimer
                                 //public VSTCallbackHandler                                           //Santi: 'VSTCallbackHandler'. An interface to allow an AudioProcessor to send and receive VST specific calls from the host. 
{
public:

    //METHODS OF THE CLASS (Constructor and Destructor)
    //==============================================================================
    VstdecoderAudioProcessor();                                                                         //Santi: Constructor
    ~VstdecoderAudioProcessor();

    /* Get functions */
    void* getFXHandle() { return hAmbi; }                                                               //Santi: Returns the 'hAmbi' variable.
    int getCurrentBlockSize() { return nHostBlockSize; }                                                //Santi: Returns the 'nHostBlockSize' variable.
    int getCurrentNumInputs() { return nNumInputs; }                                                    //Santi: Returns the 'nNumInputs' variable.
    int getCurrentNumOutputs() { return nNumOutputs; }                                                  //Santi: Returns the 'nNumOutputs' variable.

    /* JSON */                                                                                          //Santi: Different functions to work with JSON files.
    void saveConfigurationToFile(File destination);
    void loadConfiguration(const File& presetFile);
    void setLastDir(File newLastDir) { lastDir = newLastDir; }                                          //Santi: Set the last directory used.
    File getLastDir() { return lastDir; };                                                              //Santi: Get the las directory used.
                                                                      //Santi: Destructor

    //METHODS OF THE CLASS
    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;                                //Santi: 'prepareToPlay()'. It is called before the playback to prepare correctly the audio processor. From this function we can call the methods like ‘getTotalNumInputChannels()’ and ‘getTotalNumOutputChannels()’
    void releaseResources() override;                                                                   //Santi: 'releaseResources()'. Allows the source to release anything it no longer needs after playback has stopped. 
                                                                                                                  //This will be called when the source no longer has its getNextAudioBlock () method called, so it should free up any spare memory, etc. that you might have assigned during the prepareToPlay () call.

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;                            //Santi: 'isBusesLayoutSupported()'. This method checks if the number of bus channels that is working is correct. In case of being working with Ambisonic signals of 3rd order, we must indicate it in this Method.
   #endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;                                      //Santi: 'processBlock()'. Most important method. It makes the processing of every audio blocks. In this method, we must make the programming that we would likre to apply to the audio.
                                                                                                                //It receives as input a a pointer to the audio buffer and other to the Midi buffer.

    //==============================================================================
    AudioProcessorEditor* createEditor() override;                                                      //Santi: 'createEditor()'. Creates the processor's GUI. 
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;                                                              //Santi: 'getName()'. Returns the name of this processor. 

    float getParameter(int index) override;
    void setParameter(int index, float newValue) override;                                              //Santi: 'setParameter()' function. It receives 2 parameters, an int called 'index' and a float called 'newValue'.



    bool acceptsMidi() const override;                                                                  //Santi: 'acceptsMidi()'. Returns true if the processor wants MIDI messages. 
    bool producesMidi() const override;                                                                 //Santi: 'producesMidi()'. Returns true if the processor produces MIDI messages.
    bool isMidiEffect() const override;                                                                 //Santi: 'isMidiEffect()'. Returns true if this is a MIDI effect plug-in and does no audio processing. 
    double getTailLengthSeconds() const override;                                                       //Santi: 'getTailLengthSeconds()'. Returns the length of the processor's tail, in seconds.

    //==============================================================================
    int getNumPrograms() override;                                                                      //Santi: 'getNumPrograms()'. Returns the number of preset programs the processor supports.
                                                                                                                                    //The value returned must be valid as soon as this object is created, and must not change over its lifetime.
                                                                                                                                    //This value shouldn't be less than 1. 
    int getCurrentProgram() override;                                                                   //Santi: 'getCurrentProgram()'. Returns the number of the currently active program. 
    void setCurrentProgram (int index) override;                                                        //Santi: 'setCurrentProgram()'. Called by the host to change the current program. 
    const String getProgramName (int index) override;                                                   //Santi: 'getProgramName()'. Must return the name of a given program. 
    void changeProgramName (int index, const String& newName) override;                                 //Santi: 'changeProgramName()'. Called by the host to rename a program. 

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;                                          //Santi: 'get StateInformation()'. It basically takes a snapshot of all the parameters, all of the stuff that's happening in our plugin.                
                                                                                                                //This method takes the snapshot of where those parameters are set, saves itto an XML file and then converts it to a binary file.
    void setStateInformation (const void* data, int sizeInBytes) override;                              //Santi: 'setStateInformation()'. This must restore the processor's state from a block of data previously created using getStateInformation(). 


    const String getInputChannelName(int channelIndex) const override;
    const String getOutputChannelName(int channelIndex) const override;





private:

    void* hAmbi;          /* vst_decoder handle */                                                         //Santi: In computer programming, a handle is an abstract reference to a resource that is used when application software references blocks of memory or objects that are managed by another system like a database or an operating system. 
    int nNumInputs;       /* current number of input channels */
    int nNumOutputs;      /* current number of output channels */
    int nSampleRate;      /* current host sample rate */
    int nHostBlockSize;   /* typical host block size to expect, in samples */
    File lastDir;
    ValueTree loudspeakers{ "Loudspeakers" };                                                           //Santi: 'ValueTree'. A powerful tree structure that can be used to hold free-form data, and which can handle its own undo and redo behaviour. 



    void timerCallback(int timerID) //override                                                          //Santi: The user - defined callback routine that actually gets called by each of the timers that are running.
    {
    //    switch (timerID) {
    //    case TIMER_PROCESSING_RELATED:
    //        /* reinitialise codec if needed */
    //        if (vst_decoder_getCodecStatus(hAmbi) == CODEC_STATUS_NOT_INITIALISED) {
    //            try {
    //                std::thread threadInit(vst_decoder_initCodec, hAmbi);
    //                threadInit.detach();
    //            }
    //            catch (const std::exception& exception) {
    //                std::cout << "Could not create thread" << exception.what() << std::endl;
    //            }
    //        }
    //        break;
    //    case TIMER_GUI_RELATED:
    //        /* handled in PluginEditor */
    //        break;
    //    }
    }


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VstdecoderAudioProcessor)
};
