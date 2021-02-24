/*
  ==============================================================================

    vst_decoder.h
    Created: 9 Feb 2021 3:27:41am
    Author:  SANTI

  ==============================================================================
*/

#ifndef __VST_DECODER_H_INCLUDED__
#define __VST_DECODER_H_INCLUDED__



#include "_common.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

    /* ========================================================================== */
    /*                             Presets + Constants                            */
    /* ========================================================================== */

    /**
     * Available decoding methods. See saf_hoa_internal.h for a more indepth
     * description of each decoding approach.
     */
    typedef enum {
        DECODING_METHOD_SAD = 1, /**< Sampling Ambisonic Decoder (SAD) */
        DECODING_METHOD_MMD,     /**< Mode-Matching Decoder (MMD) */
        DECODING_METHOD_EPAD,    /**< Energy-Preserving Ambisonic Decoder (EPAD) */
        DECODING_METHOD_ALLRAD   /**< All-Round Ambisonic Decoder (AllRAD) */

    } VST_DECODER_DECODING_METHODS;

    /** Number of decoding method options */
#define VST_DECODER_NUM_DECODING_METHODS ( 4 )

/**
 * When using mixed order decoding (i.e. different decoding orders for
 * different frequencies), this equalisation helps maintain equal perceived
 * "loudness"
 *
 * At low frequencies, preserving amplitude is more favourable, whereas for high
 * frequencies, preserving energy is better.
 */
    typedef enum {
        AMPLITUDE_PRESERVING = 1, /**< preserve omni amplitude */
        ENERGY_PRESERVING       /**< preserve omni energy */

    } VST_DECODER_DIFFUSE_FIELD_EQ_APPROACH;

    /** Minimum transition value between low/high frequency decoders, in Hz */
#define VST_DECODER_TRANSITION_MIN_VALUE ( 500.0f )

/** Maximum transition value between low/high frequency decoders, in Hz */
#define VST_DECODER_TRANSITION_MAX_VALUE ( 2000.0f )



/* ========================================================================== */
/*                               Main Functions                               */
/* ========================================================================== */

/**
 * Creates an instance of the vst_decoder
 *
 * @param[in] phAmbi (&) address of vst_decoder handle
 */
void vst_decoder_create(void** const phAmbi);                                           //Santi: Creates an instance of the vst_decoder

/**
 * Destroys an instance of the vst_decoder
 *
 * @param[in] phAmbi (&) address of vst_decoder handle
 */
void vst_decoder_destroy(void** const phAmbi);                                          //Santi: Destroys an instance of the vst_decoder

/**
 * Initialises an instance of vst_decoder with default settings
 *
 * @warning This should not be called while _process() is on-going!
 *
 * @param[in] hAmbi      vst_decoder handle
 * @param[in] samplerate Host samplerate.
 */
void vst_decoder_init(void* const hAmbi,                                                //Santi: Initialises an instance of vst_decoder with default settings
    int samplerate);

/**
 * Intialises the codec variables, based on current global/user parameters
 *
 * @note This function is fully threadsafe. It can even be called periodically
 *       via a timer on one thread, while calling _process() on another thread.
 *       Since, if a set function is called (that warrants a re-init), then a
 *       flag is triggered internally and the next time this function is called,
 *       it will wait until the current process() function has completed before
 *       reinitialising the relevant parameters. If the _initCodec() takes
 *       longer than the time it takes for process() to be called again, then
 *       process() is simply bypassed until the codec is ready.
 * @note This function does nothing if no re-initialisations are required.
 *
 * @param[in] hAmbi      vst_decoder handle
 */
void vst_decoder_initCodec(void* const hAmbi);                                          //Santi: Intialises the codec variables, based on current global/user parameters

/**
 * Decodes input spherical harmonic signals to the loudspeaker channels
 *
 * @param[in] hAmbi    vst_decoder handle
 * @param[in] inputs   Input channel buffers; 2-D array: nInputs x nSamples
 * @param[in] outputs  Output channel buffers; 2-D array: nOutputs x nSamples
 * @param[in] nInputs  Number of input channels
 * @param[in] nOutputs Number of output channels
 * @param[in] nSamples Number of samples in 'inputs'/'output' matrices
 */
void vst_decoder_process(void* const hAmbi,                                             //Santi: Decodes input spherical harmonic signals to the loudspeaker channels
    float** const inputs,
    float** const outputs,
    int nInputs,
    int nOutputs,
    int nSamples);


/* ========================================================================== */
/*                                Set Functions                               */
/* ========================================================================== */

/**
 * Sets all intialisation flags to 1. Re-initialising all settings/variables,
 * as vst_decoder is currently configured, at next available opportunity.
 */
void vst_decoder_refreshSettings(void* const hAmbi);                                    //Santi: Sets all intialisation flags to 1. Re-initialising all settings/variables, as vst_decoder is currently configured, at next available opportunity.

/**
 * Sets the master decoding order. However, the decoding order may be lower than
 * this for any given frequency, this is just the maximum.
 *
 * If decoding order is higher than the input signal order, the extra required
 * channels are filled with zeros. If the decoding order is lower than the input
 * signal order, the number input signals is truncated accordingly. (see
 * #SH_ORDERS enum)
 */
void vst_decoder_setMasterDecOrder(void* const hAmbi, int newValue);                    //Santi: Sets the master decoding order. However, the decoding order may be lower than this for any given frequency, this is just the maximum.

/**
 * Sets the decoding order for a given frequency band
 *
 * @note The maximum order is dictated by vst_decoder_setMasterDecOrder()
 *
 * @param[in] hAmbi    vst_decoder handle
 * @param[in] newValue New decoding order
 * @param[in] bandIdx  Frequency band index
 */
void vst_decoder_setDecOrder(void* const hAmbi, int newValue, int bandIdx);             //Santi: Sets the decoding order for a given frequency band

/**
 * Sets the decoding order for all frequency bands
 *
 * @note the maximum order is dictated by vst_decoder_setMasterDecOrder()
 *
 * @param[in] hAmbi    vst_decoder handle
 * @param[in] newValue New decoding order
 */
void vst_decoder_setDecOrderAllBands(void* const hAmbi, int newValue);                  //Santi: Sets the decoding order for all frequency bands

/**
 * Sets the azimuth of a specific loudspeaker
 *
 * @param[in] hAmbi      vst_decoder handle
 * @param[in] index      Loudspeaker index
 * @param[in] newAzi_deg New azimuth in DEGREES
 */
void vst_decoder_setLoudspeakerAzi_deg(void* const hAmbi,                               //Santi: Sets the azimuth of a specific loudspeaker
    int index,
    float newAzi_deg);

/**
 * Sets the elevation of a specific loudspeaker
 *
 * @param[in] hAmbi       vst_decoder handle
 * @param[in] index       Loudspeaker index
 * @param[in] newElev_deg New elevation in DEGREES
 */
void vst_decoder_setLoudspeakerElev_deg(void* const hAmbi,                              //Santi: Sets the elevation of a specific loudspeaker
    int index,
    float newElev_deg);

/**
 * Sets the number of loudspeakers to decode to
 */
void vst_decoder_setNumLoudspeakers(void* const hAmbi, int new_nLoudspeakers);          //Santi: Sets the number of loudspeakers to decode to

/**
 * Sets flag to dictate whether the output loudspeaker signals should be
 * binauralised
 *
 * @param[in] hAmbi    vst_decoder handle
 * @param[in] newState '0' output loudspeaker signals, '1' output binaural
 *                     signals
 */
void vst_decoder_setBinauraliseLSflag(void* const hAmbi, int newState);                 //Santi: Sets flag to dictate whether the output loudspeaker signals should be

/**
 * Sets flag to dictate whether the default HRIRs in the Spatial_Audio_Framework
 * should be used (1), or a custom HRIR set loaded via a SOFA file (0).
 *
 * @note If the custom set fails to load correctly, vst_decoder will revert to the
 *       default set. Use vst_decoder_getUseDefaultHRIRsflag() to check if loading
 *       was successful.
 *
 * @param[in] hAmbi     vst_decoder handle
 * @param[in] newState  '0' use custom HRIR set, '1' use default HRIR set
 */
void vst_decoder_setUseDefaultHRIRsflag(void* const hAmbi, int newState);               //Santi: Sets flag to dictate whether the default HRIRs in the Spatial_Audio_Framework should be used(1), or a custom HRIR set loaded via a SOFA file(0).

/**
 * Sets the file path for a .sofa file, in order to employ a custom HRIR set for
 * the decoding.
 *
 * @note If the custom set failes to load correctly, vst_decoder will revert to the
 *       default set. Use vst_decoder_getUseDefaultHRIRsflag() to check if loading
 *       was successful.
 *
 * @param[in] hAmbi vst_decoder handle
 * @param[in] path  File path to .sofa file (WITH file extension)
 */
void vst_decoder_setSofaFilePath(void* const hAmbi, const char* path);                  //Santi: Sets the file path for a .sofa file, in order to employ a custom HRIR set for the decoding.

/**
 * Enable (1) or disable (0) the pre-processing applied to the HRTFs.
 */
void vst_decoder_setEnableHRIRsPreProc(void* const hAmbi, int newState);                //Santi: Enable (1) or disable (0) the pre-processing applied to the HRTFs.

/**
 * Sets the source preset (ideal SH or SH signals derived from mic arrays)
 *
 * By default the decoder will decode at the maximum order for all frequencies.
 * However, in the case of spherical harmonic input derived from microphone
 * arrays, the available order is frequency dependent, therefore, different
 * bands require different decoding orders.
 * For conveinience, presets for a handful of comerically available microphone
 * array are included (see #MIC_PRESETS enum).
 */
void vst_decoder_setSourcePreset(void* const hAmbi, int newPresetID);                   //Santi: Sets the source preset (ideal SH or SH signals derived from mic arrays)

/**
 * Sets the output loudspeaker preset.
 *
 * For conveinience, presets for several popular arrangements are included (see
 * #LOUDSPEAKER_ARRAY_PRESETS enum).
 */
void vst_decoder_setOutputConfigPreset(void* const hAmbi, int newPresetID);             //Santi: Sets the output loudspeaker preset.

/**
 * Sets the Ambisonic channel ordering convention to decode with, in order to
 * match the convention employed by the input signals (see #CH_ORDER enum)
 */
void vst_decoder_setChOrder(void* const hAmbi, int newOrder);                           //Santi: Sets the Ambisonic channel ordering convention to decode with, in order to match the convention employed by the input signals(see #CH_ORDER enum)

/**
 * Sets the Ambisonic normalisation convention to decode with, in order to match
 * with the convention employed by the input signals (see #NORM_TYPES enum)
 */
void vst_decoder_setNormType(void* const hAmbi, int newType);                           //Santi: Sets the Ambisonic normalisation convention to decode with, in order to match with the convention employed by the input signals(see #NORM_TYPES enum)

/**
 * Sets the decoding method for a specific decoder. vst_decoder employs two
 * decoders, one for low frequencies and one for high frequencies.
 * (use vst_decoder_setTransitionFreq() to dictate the transition frequency)
 *
 * @param[in] hAmbi vst_decoder handle
 * @param[in] index Index for low (0) or high (1) frequency decoder
 * @param[in] newID New decoding method (see #vst_decoder_DECODING_METHODS enum)
 */
void vst_decoder_setDecMethod(void* const hAmbi, int index, int newID);                 //Santi: Sets the decoding method for a specific decoder.vst_decoder employs two decoders, one for low frequenciesand one for high frequencies. (use vst_decoder_setTransitionFreq() to dictate the transition frequency)

/**
 * Sets a flag to enable/disable (1 or 0) the max_rE weighting for one of the
 * decoders.
 *
 * @param[in] hAmbi vst_decoder handle
 * @param[in] index For low (0) or high (1) frequency decoder
 * @param[in] newID '0' disabled, '1' enabled
 */
void vst_decoder_setDecEnableMaxrE(void* const hAmbi, int index, int newID);            //Santi: Sets a flag to enable/disable (1 or 0) the max_rE weighting for one of the decoders.

/**
 * Sets the equalisation approach for one of the decoders. This is used to help
 * keep the perceived loudness consistent, when using mixed decoding orders
 * (i.e. different decoding orders for different frequency bands)
 * vst_decoder either to preserves amplitude or energy for each order.
 *
 * @note It is suggested to preserve amplitude at low-frequencies and energy
 *       at high-frequencies.
 *
 * @param[in] hAmbi vst_decoder handle
 * @param[in] index For low (0) or high (1) frequency decoder
 * @param[in] newID see #vst_decoder_DIFFUSE_FIELD_EQ_APPROACH enum
 */
void vst_decoder_setDecNormType(void* const hAmbi, int index, int newID);               //Santi: Sets the equalisation approach for one of the decoders. This is used to help keep the perceived loudness consistent, when using mixed decoding orders (i.e.different decoding orders for different frequency bands) vst_decoder either to preserves amplitude or energy for each order.

/**
 * Sets the frequeny at which to transition from the low frequency decoder to
 * the high frequency decoder.
 *
 * @param[in] hAmbi    vst_decoder handle
 * @param[in] newValue New transition frequency, in Hz
 */
void vst_decoder_setTransitionFreq(void* const hAmbi, float newValue);                  //Santi: Sets the frequeny at which to transition from the low frequency decoder to the high frequency decoder.


/* ========================================================================== */
/*                                Get Functions                               */
/* ========================================================================== */

/**
 * Returns the processing framesize (i.e., number of samples processed with
 * every _process() call )
 */
int vst_decoder_getFrameSize(void);                                                     //Santi: Returns the processing framesize (i.e., number of samples processed withevery _process() call )

/**
 * Returns current codec status (see #CODEC_STATUS enum)
 */
CODEC_STATUS vst_decoder_getCodecStatus(void* const hAmbi);                             //Santi: Returns current codec status (see #CODEC_STATUS enum)

/**
 * (Optional) Returns current intialisation/processing progress, between 0..1
 * - 0: intialisation/processing has started
 * - 1: intialisation/processing has ended
 */
float vst_decoder_getProgressBar0_1(void* const hAmbi);                                 //Santi: (Optional) Returns current intialisation/processing progress, between 0..1

/**
 * (Optional) Returns current intialisation/processing progress text
 *
 * @note "text" string should be (at least) of length:
 *       #PROGRESSBARTEXT_CHAR_LENGTH
 */
void vst_decoder_getProgressBarText(void* const hAmbi, char* text);                     //Santi: (Optional) Returns current intialisation/processing progress text

/**
 * Returns the master/maximum decoding order (see #SH_ORDERS enum)
 */
int vst_decoder_getMasterDecOrder(void* const hAmbi);                                   //Santi: Returns the master/maximum decoding order (see #SH_ORDERS enum)

/**
 * Returns the decoding order for a given frequency band index (see #SH_ORDERS
 * enum)
 */
int vst_decoder_getDecOrder(void* const hAmbi, int bandIdx);                            //Santi: Returns the decoding order for a given frequency band index (see #SH_ORDERS enum)

/**
 * Returns the decoding order for the first band (see #SH_ORDERS enum)
 */
int vst_decoder_getDecOrderAllBands(void* const hAmbi);                                 //Santi: Returns the decoding order for the first band (see #SH_ORDERS enum)

/**
 * Returns handles for the decoding orders and frequency vector.
 *
 * @param[in]  hAmbi     vst_decoder handle
 * @param[out] pX_vector (&) frequency vector; pNpoints x 1
 * @param[out] pY_values (&) decoding order per frequency; pNpoints x 1
 * @param[out] pNpoints  (&) number of grid points.
 */
void vst_decoder_getDecOrderHandle(void* const hAmbi,
    float** pX_vector,
    int** pY_values,
    int* pNpoints);

/**
 * Returns the number of frequency bands employed by vst_decoder.
 */
int vst_decoder_getNumberOfBands(void);

/**
 * Returns the loudspeaker azimuth in degrees for a given index.
 */
float vst_decoder_getLoudspeakerAzi_deg(void* const hAmbi, int index);

/**
 * Returns the loudspeaker elevation in degrees for a given index.
 */
float vst_decoder_getLoudspeakerElev_deg(void* const hAmbi, int index);

/**
 * Returns the number of loudspeakers in the current layout
 */
int vst_decoder_getNumLoudspeakers(void* const hAmbi);

/**
 * Returns the maximum number of loudspeakers supported by vst_decoder
 */
int vst_decoder_getMaxNumLoudspeakers(void);

/**
 * Returns the number of spherical harmonic signals required by the current
 * decoding order: (current_order + 1)^2
 */
int  vst_decoder_getNSHrequired(void* const hAmbi);

/**
 * Returns the value of a flag used to dictate whether the loudspeaker signals
 * should be binauralised (0: output loudspeaker signals, 1: output binaural
 * signals).
 */
int vst_decoder_getBinauraliseLSflag(void* const hAmbi);

/**
 * Returns the value of a flag used to dictate whether the default HRIRs in the
 * Spatial_Audio_Framework should be used (1), or a custom HRIR set loaded via a
 * SOFA file (0).
 *
 * @note If the custom set failes to load correctly, vst_decoder will revert to the
 *       default set.
 */
int vst_decoder_getUseDefaultHRIRsflag(void* const hAmbi);

/**
 * Returns the file path for a .sofa file (WITH file extension)
 *
 * @note If the custom set failes to load correctly, vst_decoder will revert to the
 *       default set. Use vst_decoder_getUseDefaultHRIRsflag() to check if loading
 *       was successful.
 */
char* vst_decoder_getSofaFilePath(void* const hAmbi);

/**
 * Returns the flag indicating whether the pre-processing applied to the HRTFs
 * is enabled (1) or disabled (0)
 */
int vst_decoder_getEnableHRIRsPreProc(void* const hAmbi);

/**
 * Returns the Ambisonic channel ordering convention currently being used to
 * decode with, which should match the convention employed by the input signals
 * (see #CH_ORDER enum)
 */
int vst_decoder_getChOrder(void* const hAmbi);

/**
 * Returns the Ambisonic normalisation convention currently being usedto decode
 * with, which should match the convention employed by the input signals (see
 * #NORM_TYPES enum).
 */
int vst_decoder_getNormType(void* const hAmbi);

/**
 * Returns the currently selected decoding method (see
 * #vst_decoder_DECODING_METHODS enum)
 */
int vst_decoder_getDecMethod(void* const hAmbi, int index);

/**
 * Returns the value of a flag used to dictate whether the max_rE weighting is
 * being applied by a given decoder
 *
 * @param[in] hAmbi vst_decoder handle
 * @param[in] index Index for low (0) or high (1) frequency decoder
 * @returns '0' if enabled, '1' if disabled
 */
int vst_decoder_getDecEnableMaxrE(void* const hAmbi, int index);

/**
 * Returns the current equalisation approach for one of the decoders (see
 * #vst_decoder_DIFFUSE_FIELD_EQ_APPROACH enum)
 *
 * @note It is suggested to preserve amplitude at low-frequencies and energy
 *       at high-frequencies
 *
 * @param[in] hAmbi vst_decoder handle
 * @param[in] index Index for low (0) or high (1) frequency decoder
 * @returns (see #vst_decoder_DIFFUSE_FIELD_EQ_APPROACH enum)
 */
int vst_decoder_getDecNormType(void* const hAmbi, int index);

/**
 * Returns the frequency (in Hz) at which to transition from the low frequency
 * decoder to the high frequency decoder.
 */
float vst_decoder_getTransitionFreq(void* const hAmbi);

/**
 * Returns the HRIR sample rate
 */
int vst_decoder_getHRIRsamplerate(void* const hAmbi);

/**
 * Returns the DAW/Host sample rate
 */
int vst_decoder_getDAWsamplerate(void* const hAmbi);

/**
 * Returns the processing delay in samples; may be used for delay compensation
 * features
 */
int vst_decoder_getProcessingDelay(void);


#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */



#endif /* __VSTDECODER_H_INCLUDED__ */