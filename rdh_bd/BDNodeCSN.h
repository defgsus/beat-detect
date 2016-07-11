// BDNodeCSN.h: interface for the CBDNodeCSN class.
//
//////////////////////////////////////////////////////////////////////

#ifndef BDHSRC_NODECSN_H
#define BDHSRC_NODECSN_H


#include "BDNode.h"

RDH_BD_BEGIN_NAMESPACE

class CBDNodeCSN  
{
public:
	CBDNodeCSN( CBDNode * pNode );
	virtual ~CBDNodeCSN();

    RESULT     Initialize();

    RESULT     AddCSNLink( CBDNode * pNode );
    RESULT     RemoveCSNLink( CBDNode * pNode );
    RESULT     FlushCSNLinks();

    RESULT     UpdateCSN( FLOAT flNetEnergy );
    RESULT     CommitCSN();

    FLOAT       CSNOutput() const       { return m_flCSNOutput; }


protected:
    BDParamsType        m_params;
    NodeList            m_lstLinks;

    FLOAT               m_flCSNOutput;
    FLOAT               m_flCSNOutputNew;

    CBDNode           * m_pNode;
};

RDH_BD_END_NAMESPACE

#endif // BDHSRC_NODECSN_H
