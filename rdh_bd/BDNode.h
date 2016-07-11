// BDNode.h: interface for the CBDNode class.
//
//////////////////////////////////////////////////////////////////////

#ifndef BDHSRC_BDNODE_H
#define BDHSRC_BDNODE_H

#include <list>

#include "BDUtils.h"

RDH_BD_BEGIN_NAMESPACE

class CBDNode;
class CBDNodeTimingNet;
class CBDNodeCSN;
class CBDNodeVarSampler;

typedef std::list<CBDNode *> NodeList;

class CBDNode  
{
public:
    CBDNode();
	virtual ~CBDNode();

    const BDParamsType& parameters() const { return m_params; }

    RESULT      Initialize(const BDParamsType& param, FLOAT flPeriod );

    RESULT      ExecuteStep( FLOAT * pflInputBuffer );
    RESULT      CommitStep();

    void        SetIdealPeriod( FLOAT flIdealPeriod )
                { m_flIdealPeriod = 0.5f*m_flIdealPeriod + 0.5f*flIdealPeriod; }
    RESULT      AdjustPeriod();

    // Accessors per sae
    FLOAT       Period() const              { return m_flPeriod; }
    FLOAT       CSNOutput();
    FLOAT       BeatOutput();
    BOOL        LoopComplete();

    // Semi-Public
    CBDNodeTimingNet    *   TimingNet()     { return m_pNet; }
    CBDNodeCSN          *   CSN()           { return m_pCSN; }
    CBDNodeVarSampler   *   VarSampler()    { return m_pVarSampler; }

    FLOAT                   m_flIdealPeriod;

    /////////////////////////////////////////////////
    // Performance Measures
    RESULT                 CalculatePerformanceMeasures();
    BOOL                    m_fSelected;
    FLOAT                   m_flSelectedTime;
    INT32                   m_nBeatReEvaluations;
    FLOAT                   m_flAvgPeriod;
    FLOAT                   m_flPredictionError;
    INT32                   m_nSelectedBeats;
    /////////////////////////////////////////////////

protected:
    BDParamsType            m_params;
    CBDNodeTimingNet    *   m_pNet;
    CBDNodeCSN          *   m_pCSN;
    CBDNodeVarSampler   *   m_pVarSampler;

    FLOAT                   m_flPeriod;
    FLOAT                   m_flPreviousBeatStrength;
    
};

RDH_BD_END_NAMESPACE

#endif // BDHSRC_BDNODE_H
