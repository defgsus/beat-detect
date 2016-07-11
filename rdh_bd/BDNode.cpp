// BDNode.cpp: implementation of the CBDNode class.
//
//////////////////////////////////////////////////////////////////////

#include <cmath>

#include "BDNode.h"
#include "BDNodeTimingNet.h"
#include "BDNodeCSN.h"
#include "BDNodeVarSampler.h"


RDH_BD_BEGIN_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBDNode::CBDNode()
    : m_fSelected(FALSE)
    , m_flPreviousBeatStrength(0)
{

}

CBDNode::~CBDNode()
{
    if( nullptr != m_pNet )
        delete m_pNet;
    if( nullptr != m_pCSN )
        delete m_pCSN;
    if( nullptr != m_pVarSampler )
        delete m_pVarSampler;
}

FLOAT CBDNode::CSNOutput()                 
{ 
    return CSN()->CSNOutput(); 
}

FLOAT CBDNode::BeatOutput()                
{ 
    return TimingNet()->BeatOutputPrediction(); 
}

BOOL CBDNode::LoopComplete()                
{ 
    return TimingNet()->LoopComplete(); 
}


RESULT CBDNode::Initialize(const BDParamsType& p, FLOAT flPeriod)
{
    m_params = p;

    RESULT hr = S_OK;

    ///////////////////////
    // Create subcomponents
    m_pNet = new CBDNodeTimingNet( this );
    if( m_pNet == nullptr )
        return E_OUTOFMEMORY;

    m_pCSN = new CBDNodeCSN( this );
    if( m_pCSN == nullptr )
        return E_OUTOFMEMORY;

    m_pVarSampler = new CBDNodeVarSampler( this );
    if( m_pVarSampler == nullptr )
        return E_OUTOFMEMORY;
    
    /////////////////////////
    // Calculate initial loop length and sampling rate
    INT32 nLoopLen = (INT32)(flPeriod / m_params.flVarSamplerStartPeriod);
    FLOAT flSamplerPeriod = flPeriod / nLoopLen;
    // Set the loop period based on initial stats
    m_flPeriod = nLoopLen * flSamplerPeriod;
    // Set ideal period too
    SetIdealPeriod( m_flPeriod );

    ///////////////////////////
    // Initialize Subcomponents
    hr = m_pNet->Initialize( nLoopLen );
    if( FAILED(hr) )
        return hr;

    hr = m_pVarSampler->Initialize( flSamplerPeriod );
    if( FAILED(hr) )
        return hr;

    hr = m_pCSN->Initialize();
    if( FAILED(hr) )
        return hr;

    ////////////////////////////
    // Track Performance
    m_flSelectedTime = 0;
    m_nBeatReEvaluations = 0;
    m_flAvgPeriod = m_flPeriod;
    m_flPredictionError = 0;
    m_nSelectedBeats = 0;

    return hr;
}


RESULT CBDNode::ExecuteStep
( 
    FLOAT * pflInputBuffer
)
{
    RESULT hr = S_OK;

    ///////
    // Pass input to variable sampler
    BOOL fComplete;
    FLOAT flSample;

    hr = m_pVarSampler->ProcessInput( pflInputBuffer, &fComplete, &flSample );
    if( FAILED(hr) )
        return hr;

    ///////
    // Sample is complete and now flSample is valid, process rest of circuitry
    if( fComplete )
    {
        // Generate next beat output before we modify the loop in any way
        m_pNet->GenerateBeatOutput();

        // Pass sample to TimingNet
        hr = m_pNet->ExecuteStep( flSample ); 

        // Update CSN
        hr = m_pCSN->UpdateCSN( m_pNet->NetEnergy() );
    }

    if( m_fSelected && m_params.fTrackPerformance )
        m_flSelectedTime++;

    return hr;
}


RESULT CBDNode::CommitStep()
{
    return m_pCSN->CommitCSN();
}


RESULT CBDNode::AdjustPeriod
( 
)
{   

    FLOAT flNewPeriod;
    
    //////////////////////////////////////////////////////////////////////
    // PD Period Variation from Ideal Period
    if( m_params.fEnableVarSampler )
        // New period is that which is dictated by variable sampler
        flNewPeriod = VarSampler()->IdealSamplePeriod()*m_pNet->LoopLength();
    else
        // No VS, use the IOI period as the new period
        flNewPeriod = m_flIdealPeriod;

    // Adjustment strength is inversely proportional to the weighting from the VS expectation window
    FLOAT flAdjustStrength = 0.5f * (1 - VarSampler()->IdealPeriodWeight());
    // Adjust new ideal period by AdjustStrength
    FLOAT flIdealPeriodDiff = (flNewPeriod - m_flIdealPeriod) * flAdjustStrength;

    flNewPeriod = m_flIdealPeriod + flIdealPeriodDiff;
    //////////////////////////////////////////////////////////////////////

    // Set the new period
    m_flPeriod = flNewPeriod;

    FLOAT flNewSamplingPeriod = m_flPeriod / m_pNet->LoopLength();

    // Set new sampling rate
    m_pVarSampler->SamplePeriod() = flNewSamplingPeriod;

    // Track Performance
    m_flAvgPeriod = 0.9f * m_flAvgPeriod + 0.1f * m_flPeriod;

    return S_OK;
}



RESULT CBDNode::CalculatePerformanceMeasures()
{
    m_flPredictionError /= m_nSelectedBeats;
    m_flSelectedTime /= m_params.nOnsetSamplingRate;

    return S_OK;
}

RDH_BD_END_NAMESPACE
