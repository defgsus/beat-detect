// BDTimingNet.h: interface for the CBDNodeTimingNet class.
//
//////////////////////////////////////////////////////////////////////

#ifndef BDHSRC_BDNODETIMINGNET_H
#define BDHSRC_BDNODETIMINGNET_H

#include <list>

#include "BDUtils.h"

RDH_BD_BEGIN_NAMESPACE

typedef std::list<FLOAT> TimingLoopList;

class   CBDNode;


class CBDNodeTimingNet  
{
public:
	CBDNodeTimingNet( CBDNode * pNode );
	virtual ~CBDNodeTimingNet();

    RESULT     Initialize( INT32 nLoopLen );
    
    // Timing Net Execution
    RESULT     ExecuteStep( FLOAT flInput );

    // Beat Output Methods
    FLOAT       BeatOutputPrediction()      { return m_flBeatOutputPrediction; }
    BOOL        LoopComplete()              { return (m_iLoopLoc==LoopLength()); }

    // Accessors
    FLOAT           NetEnergy() const       { return m_flIntegrator; }
    INT32           LoopLength() const      { return m_nLoopLen; }

    // Semi-Public
    TimingLoopList  Net()                   { return m_lstNet; }
    void            GenerateBeatOutput();

    FLOAT               m_flBeatStrength;

protected:
    
    void            UpdateLoopStats();

    BDParamsType        m_params;

    // Timing Net
    TimingLoopList      m_lstNet;
    INT32               m_nLoopLen;

    // Integrator
    FLOAT               m_flIntegrator;

    // Beat Output   
    INT32               m_iLoopLoc;
    FLOAT               m_flBeatThresh;
    FLOAT               m_flLoopMean;
    FLOAT               m_flLoopEnergy;
    FLOAT               m_flLoopMax;

    FLOAT               m_flBeatOutputPrediction;

    // Beat Output Version 2
    INT32               m_iBeatLoc;
    INT32               m_iCandidateLoc;
    INT32               m_nBeatLife;
    

    CBDNode           * m_pNode;
    
};

RDH_BD_END_NAMESPACE

#endif // BDHSRC_BDNODETIMINGNET_H
