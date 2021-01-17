/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "C:\Users\SANTI\Desktop\SANTI\1-UPC\Q8\TFG\JUCE Projects\DecoderTest\Source\log2dSlider.h"
#include "C:\Users\SANTI\Desktop\SANTI\1-UPC\Q8\TFG\JUCE Projects\DecoderTest\Source\outputCoordsView.h"
#include "C:\Users\SANTI\Desktop\SANTI\1-UPC\Q8\TFG\JUCE Projects\resources\SPARTALookAndFeel.h"

//#include "outputCoordsView.h"
//#include "log2dSlider.h"
#include <thread>

typedef enum _PEIRO_WARNINGS {                                                  //Santi: "Enumeration Declaration"
    k_warning_none,
    k_warning_supported_fs,
    k_warning_mismatch_fs,
    k_warning_NinputCH,
    k_warning_NoutputCH
}PEIRO_WARNINGS;

//==============================================================================
/**
*/
class VstdecoderAudioProcessorEditor  : public AudioProcessorEditor,
                                        public ComboBox::Listener,
                                        public Slider::Listener       //Santi: Our editor is a Slider Listener
{
public:

    virtual void comboBoxChanged(ComboBox* comboBoxThatHasChanged) {};                       //Santi: Implementation of Combobox
    virtual void sliderValueChanged(Slider* slider) {};


    VstdecoderAudioProcessorEditor (VstdecoderAudioProcessor&);
    ~VstdecoderAudioProcessorEditor();

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;
    //void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override;
    //void sliderValueChanged(Slider* slider) override;   // This is an overriden function, and it's part of one of the classes on top. (public Slider::Listener)

    //[UserVariables]   -- You can add your own custom variables in this section.
    PluginProcessor* hVst;
    void* hAmbi;                                                                                              //Santi: What does this '*hAmbi' do? It's a pointer.
    //void timerCallback(int timerID) override;                                                               //Santi: We call 'timerCallback' function with 'timerID' as a parameter.
    std::unique_ptr<OpenGLGraphicsContextCustomShader> shader;
    OpenGLContext openGLContext;
    double progress = 0.0;
    ProgressBar progressbar;


    VstdecoderAudioProcessor& audioProcessor;


    /* Look and Feel */
    SPARTALookAndFeel LAF;                                                                                  //Santi: LookAndFeel objects define the appearance of all the JUCE widgets, and subclasses can be used to apply different 'skins' to the application. 

    /* freq-dependent decoding order */
    std::unique_ptr<log2dSlider> decOrder2dSlider;

    /* source coordinates viewport */
    std::unique_ptr<Viewport> outputCoordsVP;
    outputCoordsView* outputCoordsView_handle;
    FilenameComponent fileChooser;

    /* sofa file loading */
    void filenameComponentChanged(FilenameComponent*) {
        String directory = fileChooser.getCurrentFile().getFullPathName();
        const char* new_cstring = (const char*)directory.toUTF8();
        ambi_dec_setSofaFilePath(hAmbi, new_cstring);
    }

    /* warnings */
    PEIRO_WARNINGS currentWarning;

    /* tooltips */
    SharedResourcePointer<TooltipWindow> tipWindow;


    std::unique_ptr<juce::ComboBox> CBoutputDirsPreset;
    std::unique_ptr<juce::Slider> SL_num_loudspeakers;
    std::unique_ptr<juce::ToggleButton> TBuseDefaultHRIRs;
    std::unique_ptr<juce::ComboBox> CBsourcePreset;
    std::unique_ptr<juce::ComboBox> CBchFormat;
    std::unique_ptr<juce::ComboBox> CBnormScheme;
    std::unique_ptr<juce::Slider> SL_transitionFreq;
    std::unique_ptr<juce::ComboBox> CBdec1method;
    std::unique_ptr<juce::ToggleButton> TBdec1EnableMaxrE;
    std::unique_ptr<juce::ComboBox> CBdec1normtype;
    std::unique_ptr<juce::ToggleButton> TBBinauraliseLS;
    std::unique_ptr<juce::Slider> s_freqBand;
    std::unique_ptr<juce::TextButton> tb_loadJSON;
    std::unique_ptr<juce::TextButton> tb_saveJSON;
    std::unique_ptr<juce::ComboBox> CBnumOrder;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VstdecoderAudioProcessorEditor)
};
