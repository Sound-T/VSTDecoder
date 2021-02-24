/*
  ==============================================================================

    vst_decoder.cpp
    Created: 9 Feb 2021 3:29:01am
    Author:  SANTI

  ==============================================================================
*/

#include "vst_decoder.h"
#include "vst_decoder_internal.h"
#include "complex.h"


/* ========================================================================== */
/*                                Main Functions                               */
/* ========================================================================== */

void vst_decoder_create
(
    void** const phAmbi
)
{
    vst_decoder_data* pData = (vst_decoder_data*)malloc1d(sizeof(vst_decoder_data));
    *phAmbi = (void*)pData;
    int i, j, ch, band;

    printf(SAF_VERSION_LICENSE_STRING);

    /* default user parameters */
    loadLoudspeakerArrayPreset(LOUDSPEAKER_ARRAY_PRESET_T_DESIGN_24, pData->loudpkrs_dirs_deg, &(pData->new_nLoudpkrs), &(pData->loudpkrs_nDims));
    pData->masterOrder = pData->new_masterOrder = 1;
    for (band = 0; band < HYBRID_BANDS; band++)
        pData->orderPerBand[band] = 1;
    pData->useDefaultHRIRsFLAG = 1; /* pars->sofa_filepath must be valid to set this to 0 */
    pData->enableHRIRsPreProc = 1;
    pData->nLoudpkrs = pData->new_nLoudpkrs;
    pData->chOrdering = CH_ACN;
    pData->norm = NORM_SN3D;
    pData->dec_method[0] = DECODING_METHOD_ALLRAD;
    pData->dec_method[1] = DECODING_METHOD_ALLRAD;
    pData->rE_WEIGHT[0] = 1;
    pData->rE_WEIGHT[1] = 1;
    pData->diffEQmode[0] = ENERGY_PRESERVING;
    pData->diffEQmode[1] = ENERGY_PRESERVING;
    pData->transitionFreq = 800.0f;

    /* afSTFT stuff and audio buffers */
    pData->hSTFT = NULL;
    pData->SHFrameTD = (float**)malloc2d(MAX_NUM_SH_SIGNALS, FRAME_SIZE, sizeof(float));
    pData->outputFrameTD = (float**)malloc2d(MAX(MAX_NUM_LOUDSPEAKERS, NUM_EARS), FRAME_SIZE, sizeof(float));
    pData->SHframeTF = (float_complex***)malloc3d(HYBRID_BANDS, MAX_NUM_SH_SIGNALS, TIME_SLOTS, sizeof(float_complex));
    pData->outputframeTF = (float_complex***)malloc3d(HYBRID_BANDS, MAX_NUM_LOUDSPEAKERS, TIME_SLOTS, sizeof(float_complex));
    pData->binframeTF = (float_complex***)malloc3d(HYBRID_BANDS, NUM_EARS, TIME_SLOTS, sizeof(float_complex));

    /* codec data */
    pData->progressBar0_1 = 0.0f;
    pData->progressBarText = (char*)malloc1d(PROGRESSBARTEXT_CHAR_LENGTH * sizeof(char));
    strcpy(pData->progressBarText, "");
    pData->codecStatus = CODEC_STATUS_NOT_INITIALISED;
    pData->pars = (vst_decoder_codecPars*)malloc1d(sizeof(vst_decoder_codecPars));
    vst_decoder_codecPars* pars = pData->pars;
    for (i = 0; i < NUM_DECODERS; i++) {
        for (j = 0; j < MAX_SH_ORDER; j++) {
            pars->M_dec[i][j] = NULL;
            pars->M_dec_cmplx[i][j] = NULL;
            pars->M_dec_maxrE[i][j] = NULL;
            pars->M_dec_cmplx_maxrE[i][j] = NULL;
        }
    }
    pars->sofa_filepath = NULL;
    pars->hrirs = NULL;
    pars->hrir_dirs_deg = NULL;
    pars->hrtf_vbap_gtableIdx = NULL;
    pars->hrtf_vbap_gtableComp = NULL;
    pars->itds_s = NULL;
    pars->hrtf_fb = NULL;
    pars->hrtf_fb_mag = NULL;
    pars->weights = NULL;

    /* internal parameters */
    pData->binauraliseLS = pData->new_binauraliseLS = 0;

    /* flags */
    pData->procStatus = PROC_STATUS_NOT_ONGOING;
    pData->reinit_hrtfsFLAG = 1;
    for (ch = 0; ch < MAX_NUM_LOUDSPEAKERS; ch++)
        pData->recalc_hrtf_interpFLAG[ch] = 1;
}

void vst_decoder_destroy
(
    void** const phAmbi
)
{
    vst_decoder_data* pData = (vst_decoder_data*)(*phAmbi);
    vst_decoder_codecPars* pars;
    int i, j;

    if (pData != NULL) {
        /* not safe to free memory during intialisation/processing loop */
        while (pData->codecStatus == CODEC_STATUS_INITIALISING ||
            pData->procStatus == PROC_STATUS_ONGOING) {
            SAF_SLEEP(10);
        }

        /* free afSTFT and buffers */
        if (pData->hSTFT != NULL)
            afSTFT_destroy(&(pData->hSTFT));
        free(pData->SHFrameTD);
        free(pData->outputFrameTD);
        free(pData->SHframeTF);
        free(pData->outputframeTF);
        free(pData->binframeTF);

        /* free codec data */
        pars = pData->pars;
        free(pars->hrtf_vbap_gtableComp);
        free(pars->hrtf_vbap_gtableIdx);
        free(pars->hrtf_fb);
        free(pars->hrtf_fb_mag);
        free(pars->itds_s);
        free(pars->hrirs);
        free(pars->hrir_dirs_deg);
        free(pars->weights);
        for (i = 0; i < NUM_DECODERS; i++) {
            for (j = 0; j < MAX_SH_ORDER; j++) {
                free(pars->M_dec[i][j]);
                free(pars->M_dec_cmplx[i][j]);
                free(pars->M_dec_maxrE[i][j]);
                free(pars->M_dec_cmplx_maxrE[i][j]);
            }
        }
        free(pData->progressBarText);
        free(pData);
        pData = NULL;
    }
}

void vst_decoder_init
(
    void* const hAmbi,
    int          sampleRate
)
{
    vst_decoder_data* pData = (vst_decoder_data*)(hAmbi);

    /* define frequency vector */
    pData->fs = sampleRate;
    afSTFT_getCentreFreqs(pData->hSTFT, (float)sampleRate, HYBRID_BANDS, pData->freqVector);
}

void vst_decoder_initCodec
(
    void* const hAmbi
)
{
    vst_decoder_data* pData = (vst_decoder_data*)(hAmbi);
    vst_decoder_codecPars* pars = pData->pars;
    int i, ch, d, j, n, ng, nGrid_dirs, masterOrder, nSH_order, max_nSH, nLoudspeakers;
    float* grid_dirs_deg, * Y, * M_dec_tmp, * g, * a, * e, * a_n, * hrtf_vbap_gtable;;
    float a_avg[MAX_SH_ORDER], e_avg[MAX_SH_ORDER], azi_incl[2], sum_elev;
    SAF_SOFA_ERROR_CODES error;
    saf_sofa_container sofa;

    if (pData->codecStatus != CODEC_STATUS_NOT_INITIALISED)
        return; /* re-init not required, or already happening */
    while (pData->procStatus == PROC_STATUS_ONGOING) {
        /* re-init required, but we need to wait for the current processing loop to end */
        pData->codecStatus = CODEC_STATUS_INITIALISING; /* indicate that we want to init */
        SAF_SLEEP(10);
    }

    /* for progress bar */
    pData->codecStatus = CODEC_STATUS_INITIALISING;
    strcpy(pData->progressBarText, "Initialising");
    pData->progressBar0_1 = 0.0f;

    /* reinit afSTFT */
    masterOrder = pData->new_masterOrder;
    max_nSH = (masterOrder + 1) * (masterOrder + 1);
    nLoudspeakers = pData->new_nLoudpkrs;
    if (pData->hSTFT == NULL) {
        if (pData->new_binauraliseLS)
            afSTFT_create(&(pData->hSTFT), max_nSH, NUM_EARS, HOP_SIZE, 0, 1, AFSTFT_BANDS_CH_TIME);
        else
            afSTFT_create(&(pData->hSTFT), max_nSH, nLoudspeakers, HOP_SIZE, 0, 1, AFSTFT_BANDS_CH_TIME);
        afSTFT_clearBuffers(pData->hSTFT);
    }
    else {
        if (pData->new_binauraliseLS)
            afSTFT_channelChange(pData->hSTFT, max_nSH, NUM_EARS);
        else
            afSTFT_channelChange(pData->hSTFT, max_nSH, nLoudspeakers);
        afSTFT_clearBuffers(pData->hSTFT);
    }
    pData->binauraliseLS = pData->new_binauraliseLS;
    pData->nLoudpkrs = nLoudspeakers;

    /* Quick and dirty check to find loudspeaker dimensionality */
    strcpy(pData->progressBarText, "Computing decoder");
    pData->progressBar0_1 = 0.2f;
    sum_elev = 0.0f;
    for (ch = 0; ch < nLoudspeakers; ch++)
        sum_elev += fabsf(pData->loudpkrs_dirs_deg[ch][1]);
    if ((((sum_elev < 5.0f) && (sum_elev > -5.0f))) || (nLoudspeakers < 4))
        pData->loudpkrs_nDims = 2;
    else
        pData->loudpkrs_nDims = 3;

    /* add virtual loudspeakers for 2D case */
    if (pData->loudpkrs_nDims == 2) {
        assert(nLoudspeakers <= MAX_NUM_LOUDSPEAKERS - 2);
        pData->loudpkrs_dirs_deg[nLoudspeakers][0] = 0.0f;
        pData->loudpkrs_dirs_deg[nLoudspeakers][1] = -90.0f;
        pData->loudpkrs_dirs_deg[nLoudspeakers + 1][0] = 0.0f;
        pData->loudpkrs_dirs_deg[nLoudspeakers + 1][1] = 90.0f;
        nLoudspeakers += 2;
    }

    /* prep */
    nGrid_dirs = 480; /* Minimum t-design of degree 30, has 480 points */
    g = (float*)malloc1d(nLoudspeakers * sizeof(float));
    a = (float*)malloc1d(nGrid_dirs * sizeof(float));
    e = (float*)malloc1d(nGrid_dirs * sizeof(float));

    /* calculate loudspeaker decoding matrices */
    for (d = 0; d < NUM_DECODERS; d++) {
        M_dec_tmp = (float*)malloc1d(nLoudspeakers * max_nSH * sizeof(float));
        switch (pData->dec_method[d]) {
        case DECODING_METHOD_SAD:
            getLoudspeakerDecoderMtx((float*)pData->loudpkrs_dirs_deg, nLoudspeakers, LOUDSPEAKER_DECODER_SAD, masterOrder, 0, M_dec_tmp);
            break;
        case DECODING_METHOD_MMD:
            getLoudspeakerDecoderMtx((float*)pData->loudpkrs_dirs_deg, nLoudspeakers, LOUDSPEAKER_DECODER_MMD, masterOrder, 0, M_dec_tmp);
            break;
        case DECODING_METHOD_EPAD:
            getLoudspeakerDecoderMtx((float*)pData->loudpkrs_dirs_deg, nLoudspeakers, LOUDSPEAKER_DECODER_EPAD, masterOrder, 0, M_dec_tmp);
            break;
        case DECODING_METHOD_ALLRAD:
            getLoudspeakerDecoderMtx((float*)pData->loudpkrs_dirs_deg, nLoudspeakers, LOUDSPEAKER_DECODER_ALLRAD, masterOrder, 0, M_dec_tmp);
            break;
        }

        /* diffuse-field EQ for orders 1..masterOrder */
        for (n = 1; n <= masterOrder; n++) {
            /* truncate M_dec for each order */
            nSH_order = (n + 1) * (n + 1);
            free(pars->M_dec[d][n - 1]);
            pars->M_dec[d][n - 1] = (float*)malloc1d(nLoudspeakers * nSH_order * sizeof(float));
            free(pars->M_dec_cmplx[d][n - 1]);
            pars->M_dec_cmplx[d][n - 1] = (float_complex*)malloc1d(nLoudspeakers * nSH_order * sizeof(float_complex));
            for (i = 0; i < nLoudspeakers; i++) {
                for (j = 0; j < nSH_order; j++) {
                    pars->M_dec[d][n - 1][i * nSH_order + j] = M_dec_tmp[i * max_nSH + j]; /* for applying in the time domain, and... */
                    pars->M_dec_cmplx[d][n - 1][i * nSH_order + j] = cmplxf(pars->M_dec[d][n - 1][i * nSH_order + j], 0.0f); /* for the time-frequency domain */
                }
            }

            /* create dedicated maxrE weighted versions */
            a_n = (float*)malloc1d(nSH_order * nSH_order * sizeof(float));
            getMaxREweights(n, 1, a_n); /* weights returned as diagonal matrix */
            free(pars->M_dec_maxrE[d][n - 1]);
            pars->M_dec_maxrE[d][n - 1] = (float*)malloc1d(nLoudspeakers * nSH_order * sizeof(float));
            free(pars->M_dec_cmplx_maxrE[d][n - 1]);
            pars->M_dec_cmplx_maxrE[d][n - 1] = (float_complex*)malloc1d(nLoudspeakers * nSH_order * sizeof(float_complex));
            cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, nLoudspeakers, nSH_order, nSH_order, 1.0f,
                pars->M_dec[d][n - 1], nSH_order,
                a_n, nSH_order, 0.0f,
                pars->M_dec_maxrE[d][n - 1], nSH_order); /* for applying in the time domain */
            for (i = 0; i < nLoudspeakers * nSH_order; i++)
                pars->M_dec_cmplx_maxrE[d][n - 1][i] = cmplxf(pars->M_dec_maxrE[d][n - 1][i], 0.0f); /* for the time-frequency domain */

            /* fire a plane-wave from each grid direction to find the total energy/amplitude (using non-maxrE weighted versions) */
            Y = (float*)malloc1d(nSH_order * sizeof(float));
            grid_dirs_deg = (float*)(&__Tdesign_degree_30_dirs_deg[0][0]);
            for (ng = 0; ng < nGrid_dirs; ng++) {
                azi_incl[0] = grid_dirs_deg[ng * 2] * M_PI / 180.0f;
                azi_incl[1] = M_PI / 2.0f - grid_dirs_deg[ng * 2 + 1] * M_PI / 180.0f;
                getSHreal(n, (float*)azi_incl, 1, Y);
                cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasTrans, nLoudspeakers, 1, nSH_order, 1.0f,
                    pars->M_dec[d][n - 1], nSH_order,
                    Y, nSH_order, 0.0f,
                    g, 1);
                a[ng] = e[ng] = 0.0f;
                for (i = 0; i < nLoudspeakers; i++) {
                    a[ng] += g[i];
                    e[ng] += powf(g[i], 2.0f);
                }
            }

            /* determine the order+decoder dependent normalisation factor for energy+amplitude preserving decoding */
            a_avg[n - 1] = e_avg[n - 1] = 0.0f;
            for (ng = 0; ng < nGrid_dirs; ng++) {
                a_avg[n - 1] += a[ng];
                e_avg[n - 1] += e[ng];
            }
            a_avg[n - 1] /= (float)nGrid_dirs;
            e_avg[n - 1] /= (float)nGrid_dirs;
            pars->M_norm[d][n - 1][0] = 1.0f / (a_avg[n - 1] + 2.23e-6f); /* use this to preserve omni amplitude */
            pars->M_norm[d][n - 1][1] = sqrtf(1.0f / (e_avg[n - 1] + 2.23e-6f));  /* use this to preserve omni energy */
            free(a_n);
            free(Y);

            /* remove virtual loudspeakers from the decoder */
            if (pData->loudpkrs_nDims == 2) {
                pars->M_dec[d][n - 1] = (float*)realloc1d(pars->M_dec[d][n - 1], pData->nLoudpkrs * nSH_order * sizeof(float));
                pars->M_dec_cmplx[d][n - 1] = (float_complex*)realloc1d(pars->M_dec_cmplx[d][n - 1], pData->nLoudpkrs * nSH_order * sizeof(float_complex));
                pars->M_dec_maxrE[d][n - 1] = (float*)realloc1d(pars->M_dec_maxrE[d][n - 1], pData->nLoudpkrs * nSH_order * sizeof(float));
                pars->M_dec_cmplx_maxrE[d][n - 1] = (float_complex*)realloc1d(pars->M_dec_cmplx_maxrE[d][n - 1], pData->nLoudpkrs * nSH_order * sizeof(float_complex));
            }
        }
        free(M_dec_tmp);
    }

    /* update order */
    pData->masterOrder = pData->new_masterOrder;

    /* Binaural-related initialisations */
    if (pData->reinit_hrtfsFLAG) {
        strcpy(pData->progressBarText, "Computing VBAP gain table");
        pData->progressBar0_1 = 0.4f;

        /* load sofa file or load default hrir data */
        if (!pData->useDefaultHRIRsFLAG && pars->sofa_filepath != NULL) {
            /* Load SOFA file */
            error = saf_sofa_open(&sofa, pars->sofa_filepath);

            /* Load defaults instead */
            if (error != SAF_SOFA_OK || sofa.nReceivers != NUM_EARS)
                pData->useDefaultHRIRsFLAG = 1;
            else {
                /* Copy SOFA data */
                pars->hrir_fs = (int)sofa.DataSamplingRate;
                pars->hrir_len = sofa.DataLengthIR;
                pars->N_hrir_dirs = sofa.nSources;
                pars->hrirs = (float*)realloc1d(pars->hrirs, pars->N_hrir_dirs * NUM_EARS * (pars->hrir_len) * sizeof(float));
                memcpy(pars->hrirs, sofa.DataIR, pars->N_hrir_dirs * NUM_EARS * (pars->hrir_len) * sizeof(float));
                pars->hrir_dirs_deg = (float*)realloc1d(pars->hrir_dirs_deg, pars->N_hrir_dirs * 2 * sizeof(float));
                cblas_scopy(pars->N_hrir_dirs, sofa.SourcePosition, 3, pars->hrir_dirs_deg, 2); /* azi */
                cblas_scopy(pars->N_hrir_dirs, &sofa.SourcePosition[1], 3, &pars->hrir_dirs_deg[1], 2); /* elev */
            }

            /* Clean-up */
            saf_sofa_close(&sofa);
        }
        if (pData->useDefaultHRIRsFLAG) {
            /* Copy default HRIR data */
            pars->hrir_fs = __default_hrir_fs;
            pars->hrir_len = __default_hrir_len;
            pars->N_hrir_dirs = __default_N_hrir_dirs;
            pars->hrirs = (float*)realloc1d(pars->hrirs, pars->N_hrir_dirs * NUM_EARS * (pars->hrir_len) * sizeof(float));
            memcpy(pars->hrirs, (float*)__default_hrirs, pars->N_hrir_dirs * NUM_EARS * (pars->hrir_len) * sizeof(float));
            pars->hrir_dirs_deg = (float*)realloc1d(pars->hrir_dirs_deg, pars->N_hrir_dirs * 2 * sizeof(float));
            memcpy(pars->hrir_dirs_deg, (float*)__default_hrir_dirs_deg, pars->N_hrir_dirs * 2 * sizeof(float));
        }

        /* estimate the ITDs for each HRIR */
        pars->itds_s = (float*)realloc1d(pars->itds_s, pars->N_hrir_dirs * sizeof(float));
        estimateITDs(pars->hrirs, pars->N_hrir_dirs, pars->hrir_len, pars->hrir_fs, pars->itds_s);

        /* generate VBAP gain table for the hrir_dirs */
        hrtf_vbap_gtable = NULL;
        pars->hrtf_vbapTableRes[0] = 2; /* azimuth resolution in degrees */
        pars->hrtf_vbapTableRes[1] = 5; /* elevation resolution in degrees */
        generateVBAPgainTable3D(pars->hrir_dirs_deg, pars->N_hrir_dirs, pars->hrtf_vbapTableRes[0], pars->hrtf_vbapTableRes[1], 1, 0, 0.0f,
            &hrtf_vbap_gtable, &(pars->N_hrtf_vbap_gtable), &(pars->hrtf_nTriangles));
        if (hrtf_vbap_gtable == NULL) {
            /* if generating vbap gain tabled failed, re-calculate with default HRIR set (which is known to triangulate correctly) */
            pData->useDefaultHRIRsFLAG = 1;
            vst_decoder_initCodec(hAmbi);
        }

        /* compress VBAP table (i.e. remove the zero elements) */
        pars->hrtf_vbap_gtableComp = (float*)realloc1d(pars->hrtf_vbap_gtableComp, pars->N_hrtf_vbap_gtable * 3 * sizeof(float));
        pars->hrtf_vbap_gtableIdx = (int*)realloc1d(pars->hrtf_vbap_gtableIdx, pars->N_hrtf_vbap_gtable * 3 * sizeof(int));
        compressVBAPgainTable3D(hrtf_vbap_gtable, pars->N_hrtf_vbap_gtable, pars->N_hrir_dirs, pars->hrtf_vbap_gtableComp, pars->hrtf_vbap_gtableIdx);

        /* convert hrirs to filterbank coefficients */
        strcpy(pData->progressBarText, "Preparing HRIRs");
        pData->progressBar0_1 = 0.85f;
        pars->hrtf_fb = (float_complex*)realloc1d(pars->hrtf_fb, HYBRID_BANDS * NUM_EARS * (pars->N_hrir_dirs) * sizeof(float_complex));
        HRIRs2HRTFs_afSTFT(pars->hrirs, pars->N_hrir_dirs, pars->hrir_len, HOP_SIZE, 1, pars->hrtf_fb);
        /* HRIR pre-processing */
        if (pData->enableHRIRsPreProc) {
            /* get integration weights */
            strcpy(pData->progressBarText, "Applying HRIR Pre-Processing");
            pData->progressBar0_1 = 0.95f;
            if (pars->N_hrir_dirs <= 3600) {
                pars->weights = (float*)realloc1d(pars->weights, pars->N_hrir_dirs * sizeof(float));
                getVoronoiWeights(pars->hrir_dirs_deg, pars->N_hrir_dirs, 0, pars->weights);
            }
            else {
                pars->weights = (float*)realloc1d(pars->weights, pars->N_hrir_dirs * sizeof(float));
                for (int idx = 0; idx < pars->N_hrir_dirs; idx++)
                    pars->weights[idx] = 4.f * SAF_PI / (float)pars->N_hrir_dirs;
            }
            diffuseFieldEqualiseHRTFs(pars->N_hrir_dirs, pars->itds_s, pData->freqVector, HYBRID_BANDS, pars->weights, 1, 0, pars->hrtf_fb);
        }

        /* calculate magnitude responses */
        pars->hrtf_fb_mag = (float*)realloc1d(pars->hrtf_fb_mag, HYBRID_BANDS * NUM_EARS * (pars->N_hrir_dirs) * sizeof(float));
        for (i = 0; i < HYBRID_BANDS * NUM_EARS * (pars->N_hrir_dirs); i++)
            pars->hrtf_fb_mag[i] = cabsf(pars->hrtf_fb[i]);

        /* clean-up */
        free(hrtf_vbap_gtable);
        pData->reinit_hrtfsFLAG = 0;
    }

    /* done! */
    strcpy(pData->progressBarText, "Done!");
    pData->progressBar0_1 = 1.0f;
    pData->codecStatus = CODEC_STATUS_INITIALISED;

    free(g);
    free(a);
    free(e);
}

void vst_decoder_process
(
    void* const hAmbi,
    float** const inputs,
    float** const outputs,
    int            nInputs,
    int            nOutputs,
    int            nSamples
)
{
    vst_decoder_data* pData = (vst_decoder_data*)(hAmbi);
    vst_decoder_codecPars* pars = pData->pars;
    int t, ch, ear, i, band, orderBand, nSH_band, decIdx, nSH;
    const float_complex calpha = cmplxf(1.0f, 0.0f), cbeta = cmplxf(0.0f, 0.0f);

    /* local copies of user parameters */
    int nLoudspeakers, binauraliseLS, masterOrder;
    int orderPerBand[HYBRID_BANDS], rE_WEIGHT[NUM_DECODERS];
    float transitionFreq;
    VST_DECODER_DIFFUSE_FIELD_EQ_APPROACH diffEQmode[NUM_DECODERS];
    NORM_TYPES norm;
    CH_ORDER chOrdering;
    masterOrder = pData->masterOrder;
    nSH = ORDER2NSH(masterOrder);
    nLoudspeakers = pData->nLoudpkrs;
    memcpy(orderPerBand, pData->orderPerBand, HYBRID_BANDS * sizeof(int));
    transitionFreq = pData->transitionFreq;
    memcpy(diffEQmode, pData->diffEQmode, NUM_DECODERS * sizeof(int));
    binauraliseLS = pData->binauraliseLS;
    norm = pData->norm;
    chOrdering = pData->chOrdering;
    memcpy(rE_WEIGHT, pData->rE_WEIGHT, NUM_DECODERS * sizeof(int));

    /* Process frame */
    if (nSamples == FRAME_SIZE && (pData->codecStatus == CODEC_STATUS_INITIALISED)) {
        pData->procStatus = PROC_STATUS_ONGOING;

        /* Load time-domain data */
        for (i = 0; i < MIN(nSH, nInputs); i++)
            utility_svvcopy(inputs[i], FRAME_SIZE, pData->SHFrameTD[i]);
        for (; i < nSH; i++)
            memset(pData->SHFrameTD[i], 0, FRAME_SIZE * sizeof(float)); /* fill remaining channels with zeros */

        /* account for channel order convention */
        switch (chOrdering) {
        case CH_ACN: /* already ACN */
            break;
        case CH_FUMA:
            convertHOAChannelConvention(FLATTEN2D(pData->SHFrameTD), masterOrder, FRAME_SIZE, HOA_CH_ORDER_FUMA, HOA_CH_ORDER_ACN);
            break;
        }

        /* account for input normalisation scheme */
        switch (norm) {
        case NORM_N3D:  /* already in N3D, do nothing */
            break;
        case NORM_SN3D: /* convert to N3D */
            convertHOANormConvention(FLATTEN2D(pData->SHFrameTD), masterOrder, FRAME_SIZE, HOA_NORM_SN3D, HOA_NORM_N3D);
            break;
        case NORM_FUMA: /* only for first-order, convert to N3D */
            convertHOANormConvention(FLATTEN2D(pData->SHFrameTD), masterOrder, FRAME_SIZE, HOA_NORM_FUMA, HOA_NORM_N3D);
            break;
        }

        /* Apply time-frequency transform (TFT) */
        afSTFT_forward(pData->hSTFT, pData->SHFrameTD, FRAME_SIZE, pData->SHframeTF);

        /* Main processing: */
        /* Decode to loudspeaker set-up */
        memset(FLATTEN3D(pData->outputframeTF), 0, HYBRID_BANDS * MAX_NUM_LOUDSPEAKERS * TIME_SLOTS * sizeof(float_complex));
        for (band = 0; band < HYBRID_BANDS; band++) {
            orderBand = MAX(MIN(orderPerBand[band], masterOrder), 1);
            nSH_band = (orderBand + 1) * (orderBand + 1);
            decIdx = pData->freqVector[band] < transitionFreq ? 0 : 1; /* different decoder for low (0) and high (1) frequencies */
            if (rE_WEIGHT[decIdx]) {
                cblas_cgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, nLoudspeakers, TIME_SLOTS, nSH_band, &calpha,
                    pars->M_dec_cmplx_maxrE[decIdx][orderBand - 1], nSH_band,
                    FLATTEN2D(pData->SHframeTF[band]), TIME_SLOTS, &cbeta,
                    FLATTEN2D(pData->outputframeTF[band]), TIME_SLOTS);
            }
            else {
                cblas_cgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, nLoudspeakers, TIME_SLOTS, nSH_band, &calpha,
                    pars->M_dec_cmplx[decIdx][orderBand - 1], nSH_band,
                    FLATTEN2D(pData->SHframeTF[band]), TIME_SLOTS, &cbeta,
                    FLATTEN2D(pData->outputframeTF[band]), TIME_SLOTS);
            }
            for (i = 0; i < nLoudspeakers; i++) {
                for (t = 0; t < TIME_SLOTS; t++) {
                    if (diffEQmode[decIdx] == AMPLITUDE_PRESERVING)
                        pData->outputframeTF[band][i][t] = crmulf(pData->outputframeTF[band][i][t], pars->M_norm[decIdx][orderBand - 1][0]);
                    else
                        pData->outputframeTF[band][i][t] = crmulf(pData->outputframeTF[band][i][t], pars->M_norm[decIdx][orderBand - 1][1]);
                }
            }
        }

        /* binauralise the loudspeaker signals */
        if (binauraliseLS) {
            memset(FLATTEN3D(pData->binframeTF), 0, HYBRID_BANDS * NUM_EARS * TIME_SLOTS * sizeof(float_complex));
            /* interpolate hrtfs and apply to each source */
            for (ch = 0; ch < nLoudspeakers; ch++) {
                if (pData->recalc_hrtf_interpFLAG[ch]) {
                    vst_decoder_interpHRTFs(hAmbi, pData->loudpkrs_dirs_deg[ch][0], pData->loudpkrs_dirs_deg[ch][1], pars->hrtf_interp[ch]);
                    pData->recalc_hrtf_interpFLAG[ch] = 0;
                }
                for (band = 0; band < HYBRID_BANDS; band++)
                    for (ear = 0; ear < NUM_EARS; ear++)
                        for (t = 0; t < TIME_SLOTS; t++)
                            pData->binframeTF[band][ear][t] = ccaddf(pData->binframeTF[band][ear][t], ccmulf(pData->outputframeTF[band][ch][t], pars->hrtf_interp[ch][band][ear]));
            }

            /* scale by sqrt(number of loudspeakers) */
            for (band = 0; band < HYBRID_BANDS; band++)
                for (ear = 0; ear < NUM_EARS; ear++)
                    for (t = 0; t < TIME_SLOTS; t++)
                        pData->binframeTF[band][ear][t] = crmulf(pData->binframeTF[band][ear][t], 1.0f / sqrtf((float)nLoudspeakers));
        }

        /* inverse-TFT */
        if (binauraliseLS)
            afSTFT_backward(pData->hSTFT, pData->binframeTF, FRAME_SIZE, pData->outputFrameTD);
        else
            afSTFT_backward(pData->hSTFT, pData->outputframeTF, FRAME_SIZE, pData->outputFrameTD);

        /* Copy to output buffer */
        for (ch = 0; ch < MIN(binauraliseLS == 1 ? NUM_EARS : nLoudspeakers, nOutputs); ch++)
            utility_svvcopy(pData->outputFrameTD[ch], FRAME_SIZE, outputs[ch]);
        for (; ch < nOutputs; ch++)
            memset(outputs[ch], 0, FRAME_SIZE * sizeof(float));
    }
    else
        for (ch = 0; ch < nOutputs; ch++)
            memset(outputs[ch], 0, FRAME_SIZE * sizeof(float));

    pData->procStatus = PROC_STATUS_NOT_ONGOING;
}


/* ========================================================================== */
/*                                Set Functions                               */
/* ========================================================================== */

/* Set Functions */

void vst_decoder_refreshSettings(void* const hAmbi)
{
    vst_decoder_data* pData = (vst_decoder_data*)(hAmbi);
    int ch;
    for (ch = 0; ch < MAX_NUM_LOUDSPEAKERS; ch++)
        pData->recalc_hrtf_interpFLAG[ch] = 1;
    pData->reinit_hrtfsFLAG = 1;
    vst_decoder_setCodecStatus(hAmbi, CODEC_STATUS_NOT_INITIALISED);
}

void vst_decoder_setMasterDecOrder(void* const hAmbi, int newValue)
{
    vst_decoder_data* pData = (vst_decoder_data*)(hAmbi);
    pData->new_masterOrder = MIN(MAX(newValue, 1), MAX_SH_ORDER);
    vst_decoder_setCodecStatus(hAmbi, CODEC_STATUS_NOT_INITIALISED);
    /* FUMA only supports 1st order */
    if (pData->new_masterOrder != SH_ORDER_FIRST && pData->chOrdering == CH_FUMA)
        pData->chOrdering = CH_ACN;
    if (pData->new_masterOrder != SH_ORDER_FIRST && pData->norm == NORM_FUMA)
        pData->norm = NORM_SN3D;
}

void vst_decoder_setDecOrder(void* const hAmbi, int newValue, int bandIdx)
{
    vst_decoder_data* pData = (vst_decoder_data*)(hAmbi);
    pData->orderPerBand[bandIdx] = MIN(MAX(newValue, 1), pData->new_masterOrder);
}

void vst_decoder_setDecOrderAllBands(void* const hAmbi, int newValue)
{
    vst_decoder_data* pData = (vst_decoder_data*)(hAmbi);
    int band;

    for (band = 0; band < HYBRID_BANDS; band++)
        pData->orderPerBand[band] = MIN(MAX(newValue, 1), pData->new_masterOrder);
}

void vst_decoder_setLoudspeakerAzi_deg(void* const hAmbi, int index, float newAzi_deg)
{
    vst_decoder_data* pData = (vst_decoder_data*)(hAmbi);
    if (newAzi_deg > 180.0f)
        newAzi_deg = -360.0f + newAzi_deg;
    newAzi_deg = MAX(newAzi_deg, -180.0f);
    newAzi_deg = MIN(newAzi_deg, 180.0f);
    if (pData->loudpkrs_dirs_deg[index][0] != newAzi_deg) {
        pData->loudpkrs_dirs_deg[index][0] = newAzi_deg;
        pData->recalc_hrtf_interpFLAG[index] = 1;
        vst_decoder_setCodecStatus(hAmbi, CODEC_STATUS_NOT_INITIALISED);
    }
}

void vst_decoder_setLoudspeakerElev_deg(void* const hAmbi, int index, float newElev_deg)
{
    vst_decoder_data* pData = (vst_decoder_data*)(hAmbi);
    newElev_deg = MAX(newElev_deg, -90.0f);
    newElev_deg = MIN(newElev_deg, 90.0f);
    if (pData->loudpkrs_dirs_deg[index][1] != newElev_deg) {
        pData->loudpkrs_dirs_deg[index][1] = newElev_deg;
        pData->recalc_hrtf_interpFLAG[index] = 1;
        vst_decoder_setCodecStatus(hAmbi, CODEC_STATUS_NOT_INITIALISED);
    }
}

void vst_decoder_setNumLoudspeakers(void* const hAmbi, int new_nLoudspeakers)
{
    vst_decoder_data* pData = (vst_decoder_data*)(hAmbi);
    int ch;
    pData->new_nLoudpkrs = new_nLoudspeakers > MAX_NUM_LOUDSPEAKERS ? MAX_NUM_LOUDSPEAKERS : new_nLoudspeakers;
    pData->new_nLoudpkrs = MAX(MIN_NUM_LOUDSPEAKERS, pData->new_nLoudpkrs);
    if (pData->nLoudpkrs != pData->new_nLoudpkrs) {
        for (ch = 0; ch < MAX_NUM_LOUDSPEAKERS; ch++)
            pData->recalc_hrtf_interpFLAG[ch] = 1;
        vst_decoder_setCodecStatus(hAmbi, CODEC_STATUS_NOT_INITIALISED);
    }
}

void vst_decoder_setBinauraliseLSflag(void* const hAmbi, int newState)
{
    vst_decoder_data* pData = (vst_decoder_data*)(hAmbi);

    pData->new_binauraliseLS = newState;
    if (pData->new_binauraliseLS != pData->binauraliseLS)
        vst_decoder_setCodecStatus(hAmbi, CODEC_STATUS_NOT_INITIALISED);
}

void vst_decoder_setUseDefaultHRIRsflag(void* const hAmbi, int newState)
{
    vst_decoder_data* pData = (vst_decoder_data*)(hAmbi);

    if ((!pData->useDefaultHRIRsFLAG) && (newState)) {
        pData->useDefaultHRIRsFLAG = newState;
        vst_decoder_refreshSettings(hAmbi);  // re-init and re-calc
    }
}

void vst_decoder_setSofaFilePath(void* const hAmbi, const char* path)
{
    vst_decoder_data* pData = (vst_decoder_data*)(hAmbi);
    vst_decoder_codecPars* pars = pData->pars;

    pars->sofa_filepath = (char*)malloc1d(strlen(path) + 1);
    strcpy(pars->sofa_filepath, path);
    pData->useDefaultHRIRsFLAG = 0;
    vst_decoder_refreshSettings(hAmbi);  // re-init and re-calc
}

void vst_decoder_setEnableHRIRsPreProc(void* const hAmbi, int newState)
{
    vst_decoder_data* pData = (vst_decoder_data*)(hAmbi);
    if (newState != pData->enableHRIRsPreProc) {
        pData->enableHRIRsPreProc = newState;
        vst_decoder_refreshSettings(hAmbi);  // re-init and re-calc
    }
}

void vst_decoder_setOutputConfigPreset(void* const hAmbi, int newPresetID)
{
    vst_decoder_data* pData = (vst_decoder_data*)(hAmbi);
    int ch;

    loadLoudspeakerArrayPreset((LOUDSPEAKER_ARRAY_PRESETS)newPresetID, pData->loudpkrs_dirs_deg, &(pData->new_nLoudpkrs), &(pData->loudpkrs_nDims));
    for (ch = 0; ch < MAX_NUM_LOUDSPEAKERS; ch++)
        pData->recalc_hrtf_interpFLAG[ch] = 1;
    vst_decoder_setCodecStatus(hAmbi, CODEC_STATUS_NOT_INITIALISED);
}

void vst_decoder_setSourcePreset(void* const hAmbi, int newPresetID)
{
    vst_decoder_data* pData = (vst_decoder_data*)(hAmbi);
    int band, rangeIdx, curOrder, reverse;

    rangeIdx = 0;
    curOrder = 1;
    reverse = 0;
    switch (newPresetID) {
        /* Ideal spherical harmonics will have SH_ORDER at all frequencies */
    case MIC_PRESET_IDEAL:
        for (band = 0; band < HYBRID_BANDS; band++)
            pData->orderPerBand[band] = pData->masterOrder;
        break;

        /* For real microphone arrays, the maximum usable spherical harmonic order will depend on frequency  */
    case MIC_PRESET_ZYLIA:
        for (band = 0; band < HYBRID_BANDS; band++) {
            if (rangeIdx < 2 * (__Zylia_maxOrder - 1)) {
                if (pData->freqVector[band] > __Zylia_freqRange[rangeIdx]) {
                    if (!reverse)
                        curOrder++;
                    else
                        curOrder--;
                    reverse = (curOrder == __Zylia_maxOrder) || (reverse) ? 1 : 0;
                    rangeIdx++;
                }
            }
            pData->orderPerBand[band] = MIN(pData->masterOrder, curOrder);
        }
        break;

    case MIC_PRESET_EIGENMIKE32:
        for (band = 0; band < HYBRID_BANDS; band++) {
            if (rangeIdx < 2 * (__Eigenmike32_maxOrder - 1)) {
                if (pData->freqVector[band] > __Eigenmike32_freqRange[rangeIdx]) {
                    if (!reverse)
                        curOrder++;
                    else
                        curOrder--;
                    reverse = (curOrder == __Eigenmike32_maxOrder) || (reverse) ? 1 : 0;
                    rangeIdx++;
                }
            }
            pData->orderPerBand[band] = MIN(pData->masterOrder, curOrder);
        }
        break;

    case MIC_PRESET_DTU_MIC:
        for (band = 0; band < HYBRID_BANDS; band++) {
            if (rangeIdx < 2 * (__DTU_mic_maxOrder - 1)) {
                if (pData->freqVector[band] > __DTU_mic_freqRange[rangeIdx]) {
                    if (!reverse)
                        curOrder++;
                    else
                        curOrder--;
                    reverse = (curOrder == __DTU_mic_maxOrder) || (reverse) ? 1 : 0;
                    rangeIdx++;
                }
            }
            pData->orderPerBand[band] = MIN(pData->masterOrder, curOrder);
        }
        break;
    }
}

void vst_decoder_setChOrder(void* const hAmbi, int newOrder)
{
    vst_decoder_data* pData = (vst_decoder_data*)(hAmbi);
    if ((CH_ORDER)newOrder != CH_FUMA || pData->new_masterOrder == SH_ORDER_FIRST) /* FUMA only supports 1st order */
        pData->chOrdering = (CH_ORDER)newOrder;
}

void vst_decoder_setNormType(void* const hAmbi, int newType)
{
    vst_decoder_data* pData = (vst_decoder_data*)(hAmbi);
    if ((NORM_TYPES)newType != NORM_FUMA || pData->new_masterOrder == SH_ORDER_FIRST) /* FUMA only supports 1st order */
        pData->norm = (NORM_TYPES)newType;
}

void vst_decoder_setDecMethod(void* const hAmbi, int index, int newID)
{
    vst_decoder_data* pData = (vst_decoder_data*)(hAmbi);
    pData->dec_method[index] = (VST_DECODER_DECODING_METHODS)newID;
    vst_decoder_setCodecStatus(hAmbi, CODEC_STATUS_NOT_INITIALISED);
}

void vst_decoder_setDecEnableMaxrE(void* const hAmbi, int index, int newID)
{
    vst_decoder_data* pData = (vst_decoder_data*)(hAmbi);
    pData->rE_WEIGHT[index] = newID;
}

void vst_decoder_setDecNormType(void* const hAmbi, int index, int newID)
{
    vst_decoder_data* pData = (vst_decoder_data*)(hAmbi);
}

void vst_decoder_setTransitionFreq(void* const hAmbi, float newValue)
{
    vst_decoder_data* pData = (vst_decoder_data*)(hAmbi);
    pData->transitionFreq = CLAMP(newValue, VST_DECODER_TRANSITION_MIN_VALUE, VST_DECODER_TRANSITION_MAX_VALUE);
}


/* ========================================================================== */
/*                                Get Functions                               */
/* ========================================================================== */

/* Get Functions */

int vst_decoder_getFrameSize(void)
{
    return FRAME_SIZE;
}

CODEC_STATUS vst_decoder_getCodecStatus(void* const hAmbi)
{
    vst_decoder_data* pData = (vst_decoder_data*)(hAmbi);
    return pData->codecStatus;
}

float vst_decoder_getProgressBar0_1(void* const hAmbi)
{
    vst_decoder_data* pData = (vst_decoder_data*)(hAmbi);
    return pData->progressBar0_1;
}

void vst_decoder_getProgressBarText(void* const hAmbi, char* text)
{
    vst_decoder_data* pData = (vst_decoder_data*)(hAmbi);
    memcpy(text, pData->progressBarText, PROGRESSBARTEXT_CHAR_LENGTH * sizeof(char));
}

int vst_decoder_getMasterDecOrder(void* const hAmbi)
{
    vst_decoder_data* pData = (vst_decoder_data*)(hAmbi);
    return pData->new_masterOrder;
}

int vst_decoder_getDecOrder(void* const hAmbi, int bandIdx)
{
    vst_decoder_data* pData = (vst_decoder_data*)(hAmbi);
    return pData->orderPerBand[bandIdx];
}

int vst_decoder_getDecOrderAllBands(void* const hAmbi)
{
    vst_decoder_data* pData = (vst_decoder_data*)(hAmbi);
    return pData->orderPerBand[0];
}

void vst_decoder_getDecOrderHandle
(
    void* const hAmbi,
    float** pX_vector,
    int** pY_values,
    int* pNpoints
)
{
    vst_decoder_data* pData = (vst_decoder_data*)(hAmbi);
    (*pX_vector) = &pData->freqVector[0];
    (*pY_values) = &pData->orderPerBand[0];
    (*pNpoints) = HYBRID_BANDS;
}

int vst_decoder_getNumberOfBands(void)
{
    return HYBRID_BANDS;
}

float vst_decoder_getLoudspeakerAzi_deg(void* const hAmbi, int index)
{
    vst_decoder_data* pData = (vst_decoder_data*)(hAmbi);
    return pData->loudpkrs_dirs_deg[index][0];
}

float vst_decoder_getLoudspeakerElev_deg(void* const hAmbi, int index)
{
    vst_decoder_data* pData = (vst_decoder_data*)(hAmbi);
    return pData->loudpkrs_dirs_deg[index][1];
}

int vst_decoder_getNumLoudspeakers(void* const hAmbi)
{
    vst_decoder_data* pData = (vst_decoder_data*)(hAmbi);
    return pData->new_nLoudpkrs;
}

int vst_decoder_getMaxNumLoudspeakers()
{
    return MAX_NUM_LOUDSPEAKERS;
}

int  vst_decoder_getNSHrequired(void* const hAmbi)
{
    vst_decoder_data* pData = (vst_decoder_data*)(hAmbi);
    return (pData->masterOrder + 1) * (pData->masterOrder + 1);
}

int vst_decoder_getBinauraliseLSflag(void* const hAmbi)
{
    vst_decoder_data* pData = (vst_decoder_data*)(hAmbi);
    return pData->new_binauraliseLS;
}

int vst_decoder_getUseDefaultHRIRsflag(void* const hAmbi)
{
    vst_decoder_data* pData = (vst_decoder_data*)(hAmbi);
    return pData->useDefaultHRIRsFLAG;
}

char* vst_decoder_getSofaFilePath(void* const hAmbi)
{
    vst_decoder_data* pData = (vst_decoder_data*)(hAmbi);
    vst_decoder_codecPars* pars = pData->pars;
    if (pars->sofa_filepath != NULL)
        return pars->sofa_filepath;
    else
        return "no_file";
}

int vst_decoder_getEnableHRIRsPreProc(void* const hAmbi)
{
    vst_decoder_data* pData = (vst_decoder_data*)(hAmbi);
    return pData->enableHRIRsPreProc;
}

int vst_decoder_getChOrder(void* const hAmbi)
{
    vst_decoder_data* pData = (vst_decoder_data*)(hAmbi);
    return (int)pData->chOrdering;
}

int vst_decoder_getNormType(void* const hAmbi)
{
    vst_decoder_data* pData = (vst_decoder_data*)(hAmbi);
    return (int)pData->norm;
}

int vst_decoder_getDecMethod(void* const hAmbi, int index)
{
    vst_decoder_data* pData = (vst_decoder_data*)(hAmbi);
    return pData->dec_method[index];
}

int vst_decoder_getDecEnableMaxrE(void* const hAmbi, int index)
{
    vst_decoder_data* pData = (vst_decoder_data*)(hAmbi);
    return pData->rE_WEIGHT[index];
}

int vst_decoder_getDecNormType(void* const hAmbi, int index)
{
    vst_decoder_data* pData = (vst_decoder_data*)(hAmbi);
    return pData->diffEQmode[index];
}

float vst_decoder_getTransitionFreq(void* const hAmbi)
{
    vst_decoder_data* pData = (vst_decoder_data*)(hAmbi);
    return pData->transitionFreq;
}

int vst_decoder_getHRIRsamplerate(void* const hAmbi)
{
    vst_decoder_data* pData = (vst_decoder_data*)(hAmbi);
    vst_decoder_codecPars* pars = pData->pars;
    return pars->hrir_fs;
}

int vst_decoder_getDAWsamplerate(void* const hAmbi)
{
    vst_decoder_data* pData = (vst_decoder_data*)(hAmbi);
    return pData->fs;
}

int vst_decoder_getProcessingDelay()
{
    return 12 * HOP_SIZE;
}