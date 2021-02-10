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
 * @param[in] phAmbi (&) address of ambi_dec handle
 */
    void vst_decoder_create(void** const phAmbi);






#endif /* __VSTDECODER_H_INCLUDED__ */