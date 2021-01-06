/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "C:\Users\SANTI\Desktop\SANTI\1-UPC\Q8\TFG\JUCE Projects\SDKs\Spatial_Audio_Framework\examples\include\_common.h"

//==============================================================================
VstdecoderAudioProcessorEditor::VstdecoderAudioProcessorEditor(VstdecoderAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p), progressbar(progress), fileChooser("File", File(), true, false, false,
        "*.sofa;*.nc;", String(),
        "Load SOFA File")
{
    //--------------------------COMBOBOX NUMBER OF ORDERS
    {
        CBnumOrder.reset(new juce::ComboBox("new combo box"));
        addAndMakeVisible(CBnumOrder.get());                                          //Santi: (18) ComboBox numOrder. Maximum decoder order (can be lower at different frequencies).
        CBnumOrder->setEditableText(false);
        CBnumOrder->setJustificationType(juce::Justification::centredLeft);
        CBnumOrder->setTextWhenNothingSelected(TRANS("ORDER"));
        CBnumOrder->setTextWhenNoChoicesAvailable(TRANS("(no choices)"));
        //CBnumOrder->addListener(this);

        CBnumOrder->setBounds(98, 65, 118, 20);
    }

    /* add master decoding order options */
    CBnumOrder->addItem(TRANS("1st order"), SH_ORDER_FIRST);                        //Santi: (18) Here, the ComboBox of the order options is filled, with the 7 different orders.
    CBnumOrder->addItem(TRANS("2nd order"), SH_ORDER_SECOND);

    //--------------------------Toggle Button Binauralised Loudspeakers

    TBBinauraliseLS.reset(new juce::ToggleButton("new toggle button"));
    addAndMakeVisible(TBBinauraliseLS.get());                                       //Santi: (13) ToggleButton Binauralised Loudspeakers. If enabled, the loudspeaker signals are binauralised and sent to the first 2 output channels.
    TBBinauraliseLS->setButtonText(juce::String());
    //TBBinauraliseLS->addListener(this);

    TBBinauraliseLS->setBounds(210, 95, 24, 24);

    //--------------------------Slider Band Frequencies
    {
        s_freqBand.reset(new juce::Slider("new slider"));
        addAndMakeVisible(s_freqBand.get());                                       //Santi: (15) Slider freqBand. This sets the decoder order for all frequencies. Use the 2-D slider to change the decoder order for specific frequencies.
        s_freqBand->setRange(0, 1, 1);
        s_freqBand->setSliderStyle(juce::Slider::LinearVertical);
        s_freqBand->setTextBoxStyle(juce::Slider::NoTextBox, false, 80, 20);
        s_freqBand->setColour(juce::Slider::backgroundColourId, juce::Colour(0xff5c5d5e));
        s_freqBand->setColour(juce::Slider::trackColourId, juce::Colour(0xff315b6d));
        s_freqBand->setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
        s_freqBand->setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0x00ffffff));
        //s_freqBand->addListener(this);

        s_freqBand->setBounds(384, 176, 40, 80);
    }



    //--------------------------LOAD FILE BUTTON
    {
        tb_loadJSON.reset(new juce::TextButton("new button"));
        addAndMakeVisible(tb_loadJSON.get());                                      //Santi: (16) TextButton LoadJSON. Loads loudspeakers directions from a JSON file.
        tb_loadJSON->setButtonText(TRANS("Import"));
        tb_loadJSON->setConnectedEdges(juce::Button::ConnectedOnRight);
        //tb_loadJSON->addListener(this);
        tb_loadJSON->setColour(juce::TextButton::buttonColourId, juce::Colour(0xff14889e));

        tb_loadJSON->setBounds(427, 40, 60, 30);
    }

    ////--------------------------EXPORT FILE BUTTON
    {
        tb_saveJSON.reset(new juce::TextButton("new button"));
        addAndMakeVisible(tb_saveJSON.get());                                      //Santi: (17) TextButton SaveJSON. Saves the current loudspeakers directions to a JSON file.
        tb_saveJSON->setButtonText(TRANS("Export"));
        tb_saveJSON->setConnectedEdges(juce::Button::ConnectedOnLeft);
        //tb_saveJSON->addListener(this);
        tb_saveJSON->setColour(juce::TextButton::buttonColourId, juce::Colour(0xff224d97));
        tb_saveJSON->setColour(juce::TextButton::buttonOnColourId, juce::Colour(0xff181f9a));

        tb_saveJSON->setBounds(427 + 60, 40, 60, 30);
    }



    setSize(556, 356);


    /* tooltips */
    {
    //CBnumOrder->setTooltip("Maximum decoding order (can be lower at different frequencies). Note that the plug-in will require (order+1)^2 Ambisonic (spherical harmonic) signals as input.");
    //TBuseDefaultHRIRs->setTooltip("If this is 'ticked', the plug-in is using the default HRIR set from the Spatial_Audio_Framework.");
    //fileChooser.setTooltip("Optionally, a custom HRIR set may be loaded via the SOFA standard. Note that if the plug-in fails to load the specified .sofa file, it will revert to the default HRIR data.");
    //SL_num_loudspeakers->setTooltip("Number of loudspeakers to decode to.");
    //CBchFormat->setTooltip("Ambisonic channel ordering convention (Note that AmbiX: ACN/SN3D).");
    //CBnormScheme->setTooltip("Ambisonic normalisation scheme (Note that AmbiX: ACN/SN3D).");
    //s_freqBand->setTooltip("This sets the decoding order for all frequencies. Use the 2-D slider to change the decoding order for specific frequencies.");
    //TBBinauraliseLS->setTooltip("If enabled, the loudspeaker signals are binauralised and sent to the first 2 output channels.");
    //CBdec1method->setTooltip("Decoding method for the low-frequencies. The methods are equivalent if the loudspeakers are uniformly distributed (e.g. a t-design). Sampling Ambisonic Decoder (SAD) is the simplest, as it simply steers a hyper-cardioid beamformer to each loudspeaker direction. Mode-Matching Decoder (MMD), lends more energy to parts of the sphere which are more sparsely populated with loudspeakers (therefore, be careful! as a lot of energy can go to VOG, for example). AllRAD first decodes to a t-design and pans the t-design signals to the loudspeaker set-up using VBAP (most recommended for irregular loudspeaker arrangements).");
    //TBdec1EnableMaxrE->setTooltip("Enables/Disables the max_rE weights applied to the low-frequency decoding matrix.");
    //CBdec1normtype->setTooltip("The plug-in applies a frequency-dependent normalisation factor, which helps preserve omni-directional amplitude or energy when the decoding order differs for different frequencies. Generally, it is recommended to preserve amplitude (AP) at low-frequencies, and preserve high-frequencies (EP) at high-frequencies.");
    //SL_transitionFreq->setTooltip("Sets the frequency at which to transition from the low-frequency decoder, to the high-frequency decoder.");
    //CBsourcePreset->setTooltip("Presets for for various higher-order microphone arrays, which configure the frequency-dependent decoding order.");
    //CBoutputDirsPreset->setTooltip("Presets for loudspeaker arrangements to decode to.");
    //tb_loadJSON->setTooltip("Loads loudspeaker directions from a JSON file. The JSON file format follows the same convention as the one employed by the IEM plugin suite (https://plugins.iem.at/docs/configurationfiles/).");
    //tb_saveJSON->setTooltip("Saves the current loudspeaker directions to a JSON file. The JSON file format follows the same convention as the one employed by the IEM plugin suite (https://plugins.iem.at/docs/configurationfiles/).");
    }



}

VstdecoderAudioProcessorEditor::~VstdecoderAudioProcessorEditor()
{
}

//==============================================================================
void VstdecoderAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(juce::Colours::black);

    {
        int x = 0, y = 30, width = 656, height = 163;
        juce::Colour fillColour1 = juce::Colour(0xff19313f), fillColour2 = juce::Colour(0xff041518);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setGradientFill(juce::ColourGradient(fillColour1,
            8.0f - 0.0f + x,
            32.0f - 30.0f + y,
            fillColour2,
            8.0f - 0.0f + x,
            104.0f - 30.0f + y,
            false));
        g.fillRect(x, y, width, height);
    }

    //-----------------DATE VERSION

    g.setColour(Colours::white);
    g.setFont(Font(11.00f, Font::plain));
    g.drawText(TRANS("Ver ") + JucePlugin_VersionString + BUILD_VER_SUFFIX + TRANS(", Build Date ") + __DATE__ + TRANS(" "),
        390, 14, 530, 11,
        Justification::centredLeft, true);

    //-----------------TITLE OF THE PLUGIN
    {
        int x = 16, y = 1, width = 100, height = 32;
        juce::String text(TRANS("PEIRO|"));
        juce::Colour fillColour = juce::Colours::white;
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour(fillColour);
        g.setFont(juce::Font(18.80f, juce::Font::plain).withTypefaceStyle("Bold"));
        g.drawText(text, x, y, width, height,
            juce::Justification::centredLeft, true);
    }
    //-----------------TITLE OF THE PLUGIN
    {
        int x = 80, y = 1, width = 112, height = 32;
        juce::String text(TRANS("VST Decoder"));
        juce::Colour fillColour = juce::Colour(0xff00d8df);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour(fillColour);
        g.setFont(juce::Font(18.00f, juce::Font::plain).withTypefaceStyle("Bold"));
        g.drawText(text, x, y, width, height,
            juce::Justification::centredLeft, true);
    }
    //-----------------Label of MAX ORDER on the left of the ComboBox CMnumOrder
    {
        int x = 19, y = 58, width = 132, height = 30;                                                   //Santi: Label of MAX ORDER on the left of the ComboBox CMnumOrder
        juce::String text(TRANS("Max Order:"));
        juce::Colour fillColour = juce::Colours::white;
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour(fillColour);
        g.setFont(juce::Font(14.50f, juce::Font::plain).withTypefaceStyle("Bold"));
        g.drawText(text, x, y, width, height,
            juce::Justification::centredLeft, true);
    }

    //----------------DESIGN OF FREQUENCY BANDS
    {
        int x = 19, y = 156, width = 261, height = 30;                                                  //Santi: Design of the TABLE OF FREQUENCY BANDS: Title
        juce::String text(TRANS("Decoding Order Per Frequency Band"));
        juce::Colour fillColour = juce::Colours::white;
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour(fillColour);
        g.setFont(juce::Font(15.00f, juce::Font::plain).withTypefaceStyle("Bold"));
        g.drawText(text, x, y, width, height,
            juce::Justification::centredLeft, true);
    }

    {
        int x = 19, y = 238, width = 37, height = 30;                                                   //Santi: Design of the TABLE OF FREQUENCY BANDS: 100Hz
        juce::String text(TRANS("100"));
        juce::Colour fillColour = juce::Colours::white;
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour(fillColour);
        g.setFont(juce::Font(12.00f, juce::Font::plain).withTypefaceStyle("Bold"));
        g.drawText(text, x, y, width, height,
            juce::Justification::centredLeft, true);
    }

    {
        int x = 174, y = 238, width = 37, height = 30;                                                  //Santi: Design of the TABLE OF FREQUENCY BANDS: 1kHz
        juce::String text(TRANS("1k"));
        juce::Colour fillColour = juce::Colours::white;
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour(fillColour);
        g.setFont(juce::Font(12.00f, juce::Font::plain).withTypefaceStyle("Bold"));
        g.drawText(text, x, y, width, height,
            juce::Justification::centredLeft, true);
    }

    {
        int x = 328, y = 238, width = 37, height = 30;                                                  //Santi: Design of the TABLE OF FREQUENCY BANDS: 10kHz
        juce::String text(TRANS("10k"));
        juce::Colour fillColour = juce::Colours::white;
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour(fillColour);
        g.setFont(juce::Font(12.00f, juce::Font::plain).withTypefaceStyle("Bold"));
        g.drawText(text, x, y, width, height,
            juce::Justification::centredLeft, true);
    }

    {
        int x = 210, y = 238, width = 93, height = 30;                                                  //Santi: Design of the TABLE OF FREQUENCY BANDS: "Frequency (Hz)"
        juce::String text(TRANS("Frequency (Hz)"));
        juce::Colour fillColour = juce::Colours::white;
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour(fillColour);
        g.setFont(juce::Font(12.00f, juce::Font::plain).withTypefaceStyle("Bold"));
        g.drawText(text, x, y, width, height,
            juce::Justification::centredLeft, true);
    }

    {
        int x = 371, y = 238, width = 37, height = 30;                                                  //Santi: Design of the TABLE OF FREQUENCY BANDS: 20kHz
        juce::String text(TRANS("20k"));
        juce::Colour fillColour = juce::Colours::white;
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour(fillColour);
        g.setFont(juce::Font(12.00f, juce::Font::plain).withTypefaceStyle("Bold"));
        g.drawText(text, x, y, width, height,
            juce::Justification::centredLeft, true);
    }

    //----------------label for max ORDER on Frequency Band Slider
    {
        int x = 419, y = 170, width = 13, height = 30;                                                  //Santi: Design of the TABLE OF FREQUENCY BANDS: Label of the maximum order
        String text = String(CBnumOrder->getSelectedId());
        Colour fillColour = Colours::white;
        g.setColour(fillColour);
        g.setFont(Font(15.00f, Font::plain).withTypefaceStyle("Bold"));

        g.drawText(text, x, y, width, height,
            Justification::centredLeft, true);
    }

    //----------------Toggle Button Binauralised Loudspeakers
    {
        int x = 19, y =90, width = 189, height = 30;
        juce::String text(TRANS("Binauralise Loudspeakers:"));
        juce::Colour fillColour = juce::Colours::white;
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour(fillColour);
        g.setFont(juce::Font(14.50f, juce::Font::plain).withTypefaceStyle("Bold"));
        g.drawText(text, x, y, width, height,
            juce::Justification::centredLeft, true);
    }
}

void VstdecoderAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}


