// BDTimingNetControl.h: interface for the CBDNodeControl class.
//
//////////////////////////////////////////////////////////////////////

#ifndef BDHSRC_BDNODECONTROL_H
#define BDHSRC_BDNODECONTROL_H


#include <list>

#include "BDNode.h"
#include "BDIOIStatCollector.h"

RDH_BD_BEGIN_NAMESPACE

class CBDNodeControl  
{
public:
	CBDNodeControl();
	virtual ~CBDNodeControl();

    RESULT     Initialize(const BDParamsType& params);
    RESULT     ExecuteStep( FLOAT * pflInputBuffer, sIOIStats *pStats );

    CBDNode   * BestNode()                  { return m_pNodeBest; }

    NodeList        m_lstNodes;

protected:
    RESULT     AddNode( FLOAT flNodePeriod );

    BDParamsType    m_params;
    CBDNode *       m_pNodeBest;
    // Option 1 (also required for 2)
    FLOAT           m_flMaxCSN;
    // Option 2
    FLOAT           m_flMaxCSNTimeout;

};

RDH_BD_END_NAMESPACE

#endif // BDHSRC_BDNODECONTROL_H
