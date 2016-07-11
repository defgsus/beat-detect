
#include <cmath>

#include "BDUtils.h"

RDH_BD_BEGIN_NAMESPACE

bool FAILED(RESULT r)
{
    return r != S_OK;
}


void BDParamsType::init()
{
    // Onset Detection:
    flOnsetDetectResetTime = 0.1f;        // 100 ms
    flOnsetDetectThreshHigh = 0.06f;      // 0.06 is From Sepannen
    flOnsetDetectThreshLow = -0.035f;     // From Sepannen

    nOnsetSamplingRate = 441;

    // Onset Detection and Assembly:
    flOnsetCombineMinDist = 0.05f;         // 50 ms
    flOnsetCombineMinOnset = 0.1f;

    // Variable Sampler
    fEnableVarSampler = TRUE;
    flVarSamplerStartPeriod = 0.02f;      // 50 Hz
    flVarSamplerMaxErrorTime = 0.06f;     // DETERMINE EXPERIMENTALLY:
                                                     // Max jitter offset = ~3 samples

    flExpectationStdDevSamples = 2;       // From Cemgil et al
    flExpectationDeviancePercent = 0.08f; // From Krumhansl, 2000
    flVarSamplerGainProp = 1.0f;          // Proportional Gain
    flVarSamplerGainDiff = 1.0f;          // Differential Gain
    //  Fuzzy onset triangle width
    flFuzzyOnsetWidth = flVarSamplerStartPeriod;

    // Maximum difference in node periods that consitutes an identical node
    flNodeMaxDiff = 0.01f;
    
    // Timing Nets:
    flLoopInitValue = (FLOAT)pow(2, -5);
    flLoopMaxValue = 1 - flLoopInitValue;

    // CSN within Timing Nets
    flCSNAlpha   = 5.0f;
    flCSNMinLink = -0.03f;
    flCSNMaxLink = 0.04f;
    flCSNInputLink = 0.2f;
    flCSNDecay   = 0.8f;
    flCSNMinAct  = -1;
    flCSNMaxAct  = 1;
    flCSNOutputTimeThresh = 1.0f;        // One second as best required
                                                    // to be selected

    // Beat Detection Logic for within a loop
    // Threshold weight between max and loop mean
    // Minimum threshold allowed
    flBeatOutputMinThresh  = 0.0f;       //

    // IOI Statistics Collector: 
    //   max onset tracking time, histogram halflife
    //   dominant IOI list threshold low and high
    flIOIMaxOnsetTime = 1.2f;
    flIOIHistHalflife = 2.0f;
    flIOIDomThreshRatio = 0.25f;

    // Performance Measures
    fTrackPerformance = FALSE;
    flTrackBeginOffset = 8.0f;
    nTrackChangeNode = 0;
}


RDH_BD_END_NAMESPACE

