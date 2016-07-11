
#ifndef RDHSRC_BD_BDUTILS_H
#define RDHSRC_BD_BDUTILS_H

#include <cstdint>
#include <cassert>

#define ASSERT(arg__) assert(arg__)

#define RDH_BD_BEGIN_NAMESPACE namespace RDH_BD {
#define RDH_BD_END_NAMESPACE }


RDH_BD_BEGIN_NAMESPACE

typedef bool BOOL;
typedef uint8_t BYTE;
typedef int32_t INT32;
typedef float FLOAT;
typedef double DOUBLE;

const BOOL TRUE = true;
const BOOL FALSE = false;

enum RESULT
{
    S_OK = 0,
    S_FAIL = 1,
    E_FAIL,
    E_OUTOFMEMORY,
    E_INVALIDARG,
    S_FALSE
};

bool FAILED(RESULT);

// BeatDetect Parameters
struct BDParamsType
{
    /** Initialize with default settings */
    void init();

    // Onset Detection:
    /// Onset detection minimum onset distance
    FLOAT   flOnsetDetectResetTime;
    /// Onset detection threshold top and bottom levels for hysteresis
    FLOAT   flOnsetDetectThreshLow, flOnsetDetectThreshHigh;

    // Onset Detection and Assembly:
    ///  Min onset distance
    FLOAT   flOnsetCombineMinDist;
    ///  Minimum threshold for onset output
    FLOAT   flOnsetCombineMinOnset;

    /// Onset Stream Output Sampling Rate
    INT32   nOnsetSamplingRate;

    /// Maximum difference in node periods that consitutes an identical node
    FLOAT   flNodeMaxDiff;
    
    // Variable Sampler
    ///   Starting Sample Period
    ///   Ratio of previous error vs current error for input to PD Controller
    ///   Proportional and Differential Gains for the PD Controller
    BOOL    fEnableVarSampler;
    FLOAT   flVarSamplerStartPeriod;
    FLOAT   flVarSamplerMaxErrorTime;
    FLOAT   flExpectationStdDevSamples;
    FLOAT   flExpectationDeviancePercent;
    FLOAT   flVarSamplerGainProp, flVarSamplerGainDiff;
    ///  Fuzzy onset triangle distribution width
    FLOAT   flFuzzyOnsetWidth;

    // Timing Nets:
    ///  Loop initial/min value
    FLOAT   flLoopInitValue;
    ///  Loop max value
    FLOAT   flLoopMaxValue;

    // CSN:
    /// Alpha and Beta constants for the CSN linkage parameters
    FLOAT   flCSNAlpha;
    FLOAT   flCSNMinLink, flCSNMaxLink;
    FLOAT   flCSNInputLink;
    /// CSN decay rate
    FLOAT   flCSNDecay;
    /// Minimum and maximum activation allowed
    FLOAT   flCSNMinAct, flCSNMaxAct;
    /// Threshold multiplier for determining the top output from the CSN - Option 1
    /// Threshold in seconds for how long a loop must have max CSN
    /// output to be selected - Option 2
    FLOAT   flCSNOutputTimeThresh;  // Option 2

    // Beat Detection Logic
    /// Threshold percentage between mean and max loop value to output beats (Option 1)
    FLOAT   flBeatOutputMinThresh;

    // IOI Statistics Collector: 
    /// max onset tracking time, histogram halflife
    FLOAT   flIOIMaxOnsetTime, flIOIHistHalflife;
    /// dominant IOI list threshold low and high
    FLOAT   flIOIDomThreshRatio;

    // Performance Measures
    BOOL    fTrackPerformance;
    FLOAT   flTrackBeginOffset;
    INT32   nTrackChangeNode;

};



RDH_BD_END_NAMESPACE

#endif // RDHSRC_BD_BDUTILS_H
