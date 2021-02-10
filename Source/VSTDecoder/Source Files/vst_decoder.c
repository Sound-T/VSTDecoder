/*
  ==============================================================================

    vst_decoder.cpp
    Created: 9 Feb 2021 3:29:01am
    Author:  SANTI

  ==============================================================================
*/

#include "vst_decoder.h"


/* ========================================================================== */
/*                                Main Functions                               */
/* ========================================================================== */

void vst_decoder_create
(
    void** const phAmbi
)
{
    //ambi_dec_data* pData = (ambi_dec_data*)malloc1d(sizeof(ambi_dec_data));
    //*phAmbi = (void*)pData;
    //int i, j, ch, band;

    //printf(SAF_VERSION_LICENSE_STRING);

    ///* default user parameters */
    //loadLoudspeakerArrayPreset(LOUDSPEAKER_ARRAY_PRESET_T_DESIGN_24, pData->loudpkrs_dirs_deg, &(pData->new_nLoudpkrs), &(pData->loudpkrs_nDims));
    //pData->masterOrder = pData->new_masterOrder = 1;
    //for (band = 0; band < HYBRID_BANDS; band++)
    //    pData->orderPerBand[band] = 1;
    //pData->useDefaultHRIRsFLAG = 1; /* pars->sofa_filepath must be valid to set this to 0 */
    //pData->enableHRIRsPreProc = 1;
    //pData->nLoudpkrs = pData->new_nLoudpkrs;
    //pData->chOrdering = CH_ACN;
    //pData->norm = NORM_SN3D;
    //pData->dec_method[0] = DECODING_METHOD_ALLRAD;
    //pData->dec_method[1] = DECODING_METHOD_ALLRAD;
    //pData->rE_WEIGHT[0] = 1;
    //pData->rE_WEIGHT[1] = 1;
    //pData->diffEQmode[0] = ENERGY_PRESERVING;
    //pData->diffEQmode[1] = ENERGY_PRESERVING;
    //pData->transitionFreq = 800.0f;

    ///* afSTFT stuff and audio buffers */
    //pData->hSTFT = NULL;
    //pData->SHFrameTD = (float**)malloc2d(MAX_NUM_SH_SIGNALS, FRAME_SIZE, sizeof(float));
    //pData->outputFrameTD = (float**)malloc2d(MAX(MAX_NUM_LOUDSPEAKERS, NUM_EARS), FRAME_SIZE, sizeof(float));
    //pData->SHframeTF = (float_complex***)malloc3d(HYBRID_BANDS, MAX_NUM_SH_SIGNALS, TIME_SLOTS, sizeof(float_complex));
    //pData->outputframeTF = (float_complex***)malloc3d(HYBRID_BANDS, MAX_NUM_LOUDSPEAKERS, TIME_SLOTS, sizeof(float_complex));
    //pData->binframeTF = (float_complex***)malloc3d(HYBRID_BANDS, NUM_EARS, TIME_SLOTS, sizeof(float_complex));

    ///* codec data */
    //pData->progressBar0_1 = 0.0f;
    //pData->progressBarText = malloc1d(PROGRESSBARTEXT_CHAR_LENGTH * sizeof(char));
    //strcpy(pData->progressBarText, "");
    //pData->codecStatus = CODEC_STATUS_NOT_INITIALISED;
    //pData->pars = (ambi_dec_codecPars*)malloc1d(sizeof(ambi_dec_codecPars));
    //ambi_dec_codecPars* pars = pData->pars;
    //for (i = 0; i < NUM_DECODERS; i++) {
    //    for (j = 0; j < MAX_SH_ORDER; j++) {
    //        pars->M_dec[i][j] = NULL;
    //        pars->M_dec_cmplx[i][j] = NULL;
    //        pars->M_dec_maxrE[i][j] = NULL;
    //        pars->M_dec_cmplx_maxrE[i][j] = NULL;
    //    }
    //}
    //pars->sofa_filepath = NULL;
    //pars->hrirs = NULL;
    //pars->hrir_dirs_deg = NULL;
    //pars->hrtf_vbap_gtableIdx = NULL;
    //pars->hrtf_vbap_gtableComp = NULL;
    //pars->itds_s = NULL;
    //pars->hrtf_fb = NULL;
    //pars->hrtf_fb_mag = NULL;
    //pars->weights = NULL;

    ///* internal parameters */
    //pData->binauraliseLS = pData->new_binauraliseLS = 0;

    ///* flags */
    //pData->procStatus = PROC_STATUS_NOT_ONGOING;
    //pData->reinit_hrtfsFLAG = 1;
    //for (ch = 0; ch < MAX_NUM_LOUDSPEAKERS; ch++)
    //    pData->recalc_hrtf_interpFLAG[ch] = 1;
}


/* ========================================================================== */
/*                                Get Functions                               */
/* ========================================================================== */