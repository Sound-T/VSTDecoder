/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "C:\Users\SANTI\Desktop\SANTI\1-UPC\Q8\TFG\JUCE Projects\SDKs\Spatial_Audio_Framework\examples\include\ambi_dec.h"
#include <thread>
#define CONFIGURATIONHELPER_ENABLE_LOUDSPEAKERLAYOUT_METHODS 1
#define CONFIGURATIONHELPER_ENABLE_GENERICLAYOUT_METHODS 1
#include "C:\Users\SANTI\Desktop\SANTI\1-UPC\Q8\TFG\JUCE Projects\resources\ConfigurationHelper.h"
#define BUILD_VER_SUFFIX "" /* String to be added before the version name on the GUI (e.g. beta, alpha etc..) */ 

//==============================================================================
/**
*/

//typedef enum _TIMERS {                                                                                    //Santi: Unblock to declare the timers
//    TIMER_PROCESSING_RELATED = 1,                                                                         // At the moment, this timers lead to some errors.
//    TIMER_GUI_RELATED
//}TIMERS;



class VstdecoderAudioProcessor  : public AudioProcessor                                                     //Santi Class 'AudioProcessor'
                                  /*public MultiTimer,                                                      //Santi: HOW COULD I DECLARE THIS CLASSES?
                                  public VSTCallbackHandler*/
{
public:


    /* JSON */                                                                                              //Santi: Different functions to work with JSON files.
    void saveConfigurationToFile(File destination);
    void loadConfiguration(const File& presetFile);
    void setLastDir(File newLastDir) { lastDir = newLastDir; }
    File getLastDir() { return lastDir; };




    //==============================================================================
    VstdecoderAudioProcessor();
    ~VstdecoderAudioProcessor();

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;    //Santi: It is called before the playback to prepare correctly the audio processor. From this function we can call the methods like ‘getTotalNumInputChannels()’ and ‘getTotalNumOutputChannels()’
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;    //Santi: 
   #endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:

    void* hAmbi;          /* ambi_dec handle */
    int nNumInputs;       /* current number of input channels */
    int nNumOutputs;      /* current number of output channels */
    int nSampleRate;      /* current host sample rate */
    int nHostBlockSize;   /* typical host block size to expect, in samples */
    File lastDir;
    ValueTree loudspeakers{ "Loudspeakers" };



    void timerCallback(int timerID) //override
    {

    }


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VstdecoderAudioProcessor)
};
