// BDRealTimeStage.cpp: implementation of the CBDRealTimeStage class.
//
//////////////////////////////////////////////////////////////////////

#include "BDRealTimeStage.h"
#include "BDIOIStatCollector.h"
#include "BDNodeControl.h"
#include "BDNodeVarSampler.h"
#include "BDUtils.h"

RDH_BD_BEGIN_NAMESPACE

#define OUTPUT_ONSETS       0
#define OUTPUT_ACTUALBEATS  0
#define FIND_BEATS          1


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBDRealTimeStage::CBDRealTimeStage(const BDParamsType& p)
    : m_params      (p)
{

}

CBDRealTimeStage::~CBDRealTimeStage()
{

}


RESULT CBDRealTimeStage::CreateBeatStream
( 
    CDataStream *pStrmIn,         // Onset stream, constant tempo
    CDataStream *pStrmOut,        // Beat steram, constant tempo
    CDataStream *pStrmTempo,      // Sampling period, constant tempo
    CDataStream *pStrmBeatPeriod,  // Winning loop period, constant tempo
    CDataStream *pStrmBeatInfo
)
{
    RESULT hr = S_OK;

    /////////
    // Debug
#if OUTPUT_ONSETS
    CMFileWriter<FLOAT> MWriter;
    MWriter.Open( "Onsets.m", TRUE );
#elif OUTPUT_ACTUALBEATS
    CMFileWriter<FLOAT> MWriter;
    MWriter.Open( "ActualBeats.m", TRUE );
#endif

    ////////
    // Create streams with same info as input stream
    hr = pStrmOut->CreateData( pStrmIn );
    if( FAILED(hr) )
        return hr;
    hr = pStrmTempo->CreateData( pStrmIn );
    if( FAILED(hr) )
        return hr;
    hr = pStrmBeatPeriod->CreateData( pStrmIn );
    if( FAILED(hr) )
        return hr;
    hr = pStrmBeatInfo->CreateData( pStrmIn );
    if( FAILED(hr) )
        return hr;

    // Setup Param specifying onset input sampling rate
    ASSERT( m_params.nOnsetSamplingRate == pStrmIn->GetSampleRate() );


#if FIND_BEATS
    ////////
    // Components Setup
    sIOIStats   IOIStats;

    CBDIOIStatCollector IOICollector(m_params);
    hr = IOICollector.Initialize( &IOIStats );
    if( FAILED(hr) )
        return hr;


    CBDNodeControl NodeControl;
    hr = NodeControl.Initialize(m_params);
    if( FAILED(hr) )
        return hr;


    ////////
    // Execute

    // Create sample "buffer" - points to actual onset stream data but could theoretically be
    // a finite length buffer (10 ms maybe) used by the fuzzy onset creation for look ahead
    FLOAT * pflSampleBuffer = ((FLOAT *)pStrmIn->GetData());
    // Current input sample #
    INT32 iCurSam = 0;
    CBDNode * pNodeBest = nullptr;

    while( iCurSam < pStrmIn->GetNumSamples() )
    {
        /////////////////
        // Realtime Step

        // Track Performance
        if( !m_params.fTrackPerformance && ((FLOAT)iCurSam/m_params.nOnsetSamplingRate > m_params.flTrackBeginOffset) )
            m_params.fTrackPerformance = TRUE;

        
        // IOI Stats Collector - pass in only current sample
        hr = IOICollector.ExecuteStep( pflSampleBuffer[0], &IOIStats );

        
        // Node Control
        hr = NodeControl.ExecuteStep( pflSampleBuffer, &IOIStats );
        // Find best node
        pNodeBest = NodeControl.BestNode();

        /////////////////////
        // Update and Output
        if( nullptr != pNodeBest )
        {
            // Output Samples
            ((FLOAT *)pStrmOut->GetData())[iCurSam] = pNodeBest->BeatOutput();
            ((FLOAT *)pStrmTempo->GetData())[iCurSam] = (pNodeBest->VarSampler()->SamplePeriod() - m_params.flVarSamplerStartPeriod)*1000;//(pNodeBest->VarSampler()->SamplePeriod()-m_params.flVarSamplerStartPeriod)*1000;//
            ((FLOAT *)pStrmBeatPeriod->GetData())[iCurSam] = pNodeBest->Period();//pNodeBest->VarSampler()->flE*1000;//pNodeBest->VarSampler()->m_flOffset*100;//
            ((FLOAT *)pStrmBeatInfo->GetData())[iCurSam] = pNodeBest->LoopComplete()*10;//pNodeBest->VarSampler()->fldE*1000;//(pNodeBest->VarSampler()->IdealSamplePeriod()- m_params.flVarSamplerStartPeriod)*1000;//pNodeBest->CSNOutput();//(pNodeBest->VarSampler()->IdealSamplePeriod() - pNodeBest->VarSampler()->SamplePeriod())*1000;//
        }
        else
        {
            ((FLOAT *)pStrmOut->GetData())[iCurSam] = -1;
            ((FLOAT *)pStrmTempo->GetData())[iCurSam] = 0;
            ((FLOAT *)pStrmBeatPeriod->GetData())[iCurSam] = 0;
            ((FLOAT *)pStrmBeatInfo->GetData())[iCurSam] = 0;
        }
        
        /////////////////////


        #if 0
        if( iCurSam % 500 == 0 )
        {
            CHAR szLabel[50];
            sprintf( szLabel, "IOIHist_%d", iCurSam );
            MWriter.WriteFloatArray( szLabel, IOIStats.aflIOIHist, 800 );
        }
        #endif
        

        pflSampleBuffer++;
        iCurSam++;
    }

    ///////////////////////////////////////////////////
    // Calculate Performance Measures
    if( (nullptr != pNodeBest) )
    {
        CBDNode * pNodeLongest = pNodeBest;
        for( NodeList::iterator iNode = NodeControl.m_lstNodes.begin(); 
             iNode != NodeControl.m_lstNodes.end(); 
             iNode++ )
        {
            if( (*iNode)->m_flSelectedTime > pNodeLongest->m_flSelectedTime )
            {
                pNodeLongest = *iNode;
            }
        }
        pNodeLongest->CalculatePerformanceMeasures();

#ifdef XXX
        if( !((CBeatDetectApp *)AfxGetApp())->m_fAutomate )
        {
            FLOAT flBMP = 60/pNodeLongest->m_flAvgPeriod;
            FLOAT flPercentTime = pNodeLongest->m_flSelectedTime / (pStrmIn->GetDuration() - m_params.flTrackBeginOffset);
            CString strMsg;
            strMsg.Format( "%% Time = %.2f\n%.2f BPM\n%.2f Error\n%d Beat Re-eval\n%d Node Changes", flPercentTime*100, flBMP, sqrt(pNodeLongest->m_flPredictionError), pNodeLongest->m_nBeatReEvaluations, m_params.nTrackChangeNode );
            AfxMessageBox( strMsg );
        }
#endif
    }

    ///////////////////////////////////////////////////

#if 0
    CMFileWriter<TimingLoopList> MWriter2;
    MWriter2.Open( "Beats.m", TRUE );
    //MWriter2.WriteFloatArray( "SamplePeriod", (FLOAT *)pStrmTempo->GetData(), pStrmTempo->GetNumSamples() );
    //MWriter2.WriteFloatArray( "MaxLoopPeriod", (FLOAT *)pStrmBeatPeriod->GetData(), pStrmBeatPeriod->GetNumSamples() );
    MWriter2.WriteFloatArray( "BeatOutput", (FLOAT *)pStrmOut->GetData(), pStrmOut->GetNumSamples() );
    //MWriter2.WriteFloatArray( "BeatInfo", (FLOAT *)pStrmBeatInfo->GetData(), pStrmBeatInfo->GetNumSamples() );
    //MWriter2.WriteTimingLoops( NodeControl.m_lstNodes );
    MWriter2.Close();
#endif
#endif


#if OUTPUT_ONSETS
    //MWriter.WriteFloatArray( "IOIHist_End", IOIStats.aflIOIHist, 800 );
    //MWriter.WriteFloatList( "IOIDom", IOIStats.lstDominantIOI );
    MWriter.WriteFloatArray( "Onsets", (FLOAT *)pStrmIn->GetData(), pStrmIn->GetNumSamples() );
    MWriter.Close();
#elif OUTPUT_ACTUALBEATS
    MWriter.WriteFloatArray( "ActualBeats", (FLOAT *)pStrmIn->GetData(), pStrmIn->GetNumSamples() );
    MWriter.Close();
#endif
   
    //WriteFloatArrayToMFile("OnsetStream.m", "OnsetStream", (FLOAT *)pStrmIn->GetData(), pStrmIn->GetNumSamples() );



    return hr;
}

RDH_BD_END_NAMESPACE
