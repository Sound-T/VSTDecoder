/*
  ==============================================================================

    vst_decoder_intermal.h
    Created: 10 Feb 2021 10:51:45pm
    Author:  SANTI

  ==============================================================================
*/

#pragma once

 /**
  * @file vst_decoder_internal.h
  * @brief A frequency-dependent Ambisonic decoder for reproducing Ambisonic
  *        sound scenes over loudspeakers
  *
  * Different decoder settings can be specified for the low and high frequencies.
  * A number of decoding options are also offered, including [1,2]. When
  * utilising spherical harmonic signals derived from real microphone arrays,
  * this implementation also allows the decoding order to be specified per
  * frequency band; of course, this may also be used creatively. An optional,
  * loudspeaker channel binauraliser is included, along with with SOFA file
  * loading, for headphone listening.
  *
  * The algorithms utilised in this Ambisonic decoder were pieced together and
  * developed in collaboration with Archontis Politis.
  *
  * @test test__saf_example_vst_decoder()
  *
  * @see [1] Zotter F, Pomberger H, Noisternig M. Energy--preserving ambisonic
  *          decoding. Acta Acustica united with Acustica. 2012 Jan 1;
  *          98(1):37-47.
  * @see [2] Zotter F, Frank M. All-round ambisonic panning and decoding. Journal
  *          of the audio engineering society. 2012 Nov 26; 60(10):807-20.
  *
  * @author Leo McCormack
  * @date 07.12.2017
  */

#ifndef __VST_DECODER_INTERNAL_H_INCLUDED__
#define __VST_DECODER_INTERNAL_H_INCLUDED__

#include <stdio.h>
#include <math.h>
#include <string.h>
#include "vst_decoder.h" 
#include "saf.h"
#include "saf_externals.h" /* to also include saf dependencies (cblas etc.) */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

    /* ========================================================================== */
    /*                            Internal Parameters                             */
    /* ========================================================================== */

#ifndef FRAME_SIZE
# define FRAME_SIZE ( 128 ) 
#endif
#define HOP_SIZE ( 128 )                      /* STFT hop size = nBands */
#define HYBRID_BANDS ( HOP_SIZE + 5 )         /* hybrid mode incurs an additional 5 bands  */
#define TIME_SLOTS ( FRAME_SIZE / HOP_SIZE )  /* 4/8/16 */ 
#define MAX_NUM_LOUDSPEAKERS ( MAX_NUM_OUTPUTS ) /* Maximum permitted channels for the VST standard */
#define MIN_NUM_LOUDSPEAKERS ( 4 )            /* To help avoid traingulation errors when using AllRAD */ 
#define NUM_DECODERS ( 2 )                    /* one for low-frequencies and another for high-frequencies */
#if (FRAME_SIZE % HOP_SIZE != 0)
# error "FRAME_SIZE must be an integer multiple of HOP_SIZE"
#endif

/* ========================================================================== */
/*                                 Structures                                 */
/* ========================================================================== */

/**
 * Contains variables for sofa file loading, HRTF interpolation, and the
 * loudspeaker decoders.
 */
typedef struct _vst_decoder_codecPars
{
    /* decoders */
    float* M_dec[NUM_DECODERS][MAX_SH_ORDER];   /**< ambisonic decoding matrices ([0] for low-freq, [1] for high-freq); FLAT: nLoudspeakers x nSH */
    float_complex* M_dec_cmplx[NUM_DECODERS][MAX_SH_ORDER]; /**< complex ambisonic decoding matrices ([0] for low-freq, [1] for high-freq); FLAT: nLoudspeakers x nSH */
    float* M_dec_maxrE[NUM_DECODERS][MAX_SH_ORDER]; /**< ambisonic decoding matrices with maxrE weighting ([0] for low-freq, [1] for high-freq); FLAT: nLoudspeakers x nSH */
    float_complex* M_dec_cmplx_maxrE[NUM_DECODERS][MAX_SH_ORDER]; /**< complex ambisonic decoding matrices with maxrE weighting ([0] for low-freq, [1] for high-freq); FLAT: nLoudspeakers x nSH */
    float M_norm[NUM_DECODERS][MAX_SH_ORDER][2]; /**< norm coefficients to preserve omni energy/amplitude between different orders and decoders */

    /* sofa file info */
    char* sofa_filepath;                        /**< absolute/relevative file path for a sofa file */
    float* hrirs;                               /**< time domain HRIRs; N_hrir_dirs x 2 x hrir_len */
    float* hrir_dirs_deg;                       /**< directions of the HRIRs in degrees [azi elev]; N_hrir_dirs x 2 */
    int N_hrir_dirs;                            /**< number of HRIR directions in the current sofa file */
    int hrir_len;                               /**< length of the HRIRs, this can be truncated, see "saf_sofa_reader.h" */
    int hrir_fs;                                /**< sampling rate of the HRIRs, should ideally match the host sampling rate, although not required */

    /* vbap gain table for panning the HRIRs */
    int hrtf_vbapTableRes[2];                   /**< [azi elev] step sizes in degrees */
    int N_hrtf_vbap_gtable;                     /**< number of interpolation directions */
    int hrtf_nTriangles;                        /**< number of triangle groups after triangulation */
    int* hrtf_vbap_gtableIdx;                   /**< N_hrtf_vbap_gtable x 3 */
    float* hrtf_vbap_gtableComp;                /**< N_hrtf_vbap_gtable x 3 */

    /* hrir filterbank coefficients */
    float* itds_s;                              /**< interaural-time differences for each HRIR (in seconds); N_hrirs x 1 */
    float_complex* hrtf_fb;                     /**< HRTF filterbank coefficients; nBands x nCH x N_hrirs */
    float* hrtf_fb_mag;                         /**< magnitudes of the HRTF filterbank coefficients; nBands x nCH x N_hrirs */
    float_complex hrtf_interp[MAX_NUM_LOUDSPEAKERS][HYBRID_BANDS][NUM_EARS]; /* interpolated HRTFs */

    /* integration weights */
    float* weights;         /**< grid integration weights of hrirs; N_hrirs x 1 */

}vst_decoder_codecPars;

/**
 * Main structure for vst_decoder. Contains variables for audio buffers, afSTFT,
 * internal variables, flags, user parameters
 */
typedef struct _vst_decoder
{
    /* audio buffers + afSTFT time-frequency transform handle */
    float** SHFrameTD;
    float** outputFrameTD;
    float_complex*** SHframeTF;
    float_complex*** outputframeTF;
    float_complex*** binframeTF;
    void* hSTFT;                         /**< afSTFT handle */
    int afSTFTdelay;                     /**< for host delay compensation */
    int fs;                              /**< host sampling rate */
    float freqVector[HYBRID_BANDS];      /**< frequency vector for time-frequency transform, in Hz */

    /* our codec configuration */
    CODEC_STATUS codecStatus;
    float progressBar0_1;
    char* progressBarText;
    vst_decoder_codecPars* pars;            /**< codec parameters */

    /* internal variables */
    int loudpkrs_nDims;                  /**< dimensionality of the current loudspeaker set-up */
    int new_nLoudpkrs;                   /**< if new_nLoudpkrs != nLoudpkrs, afSTFT is reinitialised */
    int new_binauraliseLS;               /**< if new_binauraliseLS != binauraliseLS, vst_decoder is reinitialised */
    int new_masterOrder;

    /* flags */
    PROC_STATUS procStatus;
    int reinit_hrtfsFLAG; /**< 0: no init required, 1: init required */
    int recalc_hrtf_interpFLAG[MAX_NUM_LOUDSPEAKERS]; /**< 0: no init required, 1: init required */

    /* user parameters */
    int masterOrder;
    int orderPerBand[HYBRID_BANDS];      /**< Ambisonic decoding order per frequency band 1..SH_ORDER */
    VST_DECODER_DECODING_METHODS dec_method[NUM_DECODERS]; /**< decoding methods for each decoder, see "DECODING_METHODS" enum */
    int rE_WEIGHT[NUM_DECODERS];         /**< 0:disabled, 1: enable max_rE weight */
    VST_DECODER_DIFFUSE_FIELD_EQ_APPROACH diffEQmode[NUM_DECODERS]; /**< diffuse-field EQ approach; see "DIFFUSE_FIELD_EQ_APPROACH" enum */
    float transitionFreq;                /**< transition frequency for the 2 decoders, in Hz */
    int nLoudpkrs;                       /**< number of loudspeakers/virtual loudspeakers */
    float loudpkrs_dirs_deg[MAX_NUM_LOUDSPEAKERS][NUM_DECODERS]; /* loudspeaker directions in degrees [azi, elev] */
    int useDefaultHRIRsFLAG;             /**< 1: use default HRIRs in database, 0: use those from SOFA file */
    int enableHRIRsPreProc;              /**< flag to apply pre-processing to the currently loaded HRTFs */
    int binauraliseLS;                   /**< 1: convolve loudspeaker signals with HRTFs, 0: output loudspeaker signals */
    CH_ORDER chOrdering;                 /**< only ACN is supported */
    NORM_TYPES norm;                     /**< N3D or SN3D */

} vst_decoder_data;


/* ========================================================================== */
/*                             Internal Functions                             */
/* ========================================================================== */

/**
 * Sets codec status (see #CODEC_STATUS enum)
 */
void vst_decoder_setCodecStatus(void* const hCmp, CODEC_STATUS newStatus);

/**
 * Interpolates between the 3 nearest HRTFs using amplitude-preserving VBAP
 * gains. The HRTF magnitude responses and HRIR ITDs are interpolated seperately
 * before being re-combined.
 *
 * @param[in]  hAmbi         vst_decoder handle
 * @param[in]  azimuth_deg   Interpolation direction azimuth in DEGREES
 * @param[in]  elevation_deg Interpolation direction elevation in DEGREES
 * @param[out] h_intrp       Interpolated HRTF
 */
void vst_decoder_interpHRTFs(void* const hAmbi,
    float azimuth_deg,
    float elevation_deg,
    float_complex h_intrp[HYBRID_BANDS][NUM_EARS]);

/**
 * Returns the loudspeaker directions for a specified loudspeaker array preset.
 *
 * The function also returns the number of loudspeakers in the array, and a
 * crude estimate of whether it is 2D or 3D (based on whether the sum of the
 * absolute loudspeaker elevation angle is not zero).
 * Note: 2-D setups which are not on the horizontal plane (i.e.
 * all elevations==0) will be registered as a 3-D setup.
 * Further Note: default uniformly distributed points are used to pad the
 * dirs_deg matrix up to the #MAX_NUM_LOUDSPEAKERS, if nCH is less than
 * this. This can help avoid scenarios of many sources being panned in the same
 * direction, or triangulations errors.
 *
 * @param[in]  preset   See #LOUDSPEAKER_ARRAY_PRESETS enum
 * @param[out] dirs_deg Loudspeaker directions, [azimuth elevation] convention, in
 *                      DEGREES;
 * @param[out] nCH      (&) number of loudspeaker directions in the array
 * @param[out] nDims    (&) number of dimensions (2 or 3)
 */
void loadLoudspeakerArrayPreset(LOUDSPEAKER_ARRAY_PRESETS preset,
    float dirs_deg[MAX_NUM_LOUDSPEAKERS][2],
    int* nCH,
    int* nDims);






#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __vst_decoder_INTERNAL_H_INCLUDED__ */
