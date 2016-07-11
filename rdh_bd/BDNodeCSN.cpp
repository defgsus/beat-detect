// BDNodeCSN.cpp: implementation of the CBDNodeCSN class.
//
//////////////////////////////////////////////////////////////////////

#include <cmath>

#include "BDNodeTimingNet.h"
#include "BDNodeCSN.h"

RDH_BD_BEGIN_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBDNodeCSN::CBDNodeCSN( CBDNode * pNode )
    : m_pNode(pNode)
{
    if (m_pNode)
        m_params = m_pNode->parameters();
}

CBDNodeCSN::~CBDNodeCSN()
{
    m_lstLinks.clear();
}


RESULT CBDNodeCSN::Initialize()
{
    //////
    // CSN
    m_lstLinks.clear();

    m_flCSNOutput = m_params.flCSNMinAct;
    m_flCSNOutputNew = m_params.flCSNMinAct;

    return S_OK;
}   


/////////////
//CSN Methods
// Add specified net to link list and calculate link weight
RESULT CBDNodeCSN::AddCSNLink
( 
    CBDNode * pNode
)
{
    /*
    if( 1 ) //pNet->LoopLength() < LoopLength() )
    {
        // The loop to add is shorter and therefore can affect this loop's activation

        // Formula = 
        // Dif = 0.5 - abs((LA/LB mod 1)-0.5)
        // Weight = (Max-Min)*((1-2*Dif)^Alpha) + Min
        FLOAT flLBig = max( pNet->LoopLength(), LoopLength() );
        FLOAT flLSmall = min( pNet->LoopLength(), LoopLength() );

        FLOAT flDif = 0.5 - fabs( fmod(flLBig/flLSmall, 1) - 0.5 );
    
        LinkInfo.flLink = (m_params.flCSNMaxLink-m_params.flCSNMinLink) * pow(1-2*flDif, m_params.flCSNAlpha) 
                          + m_params.flCSNMinLink;
    }
    else
    {
        // The loop to add is longer than this and therefore cannot affect this loop's activation
        LinkInfo.flLink = 0;
    }
    */

    m_lstLinks.push_back( pNode );

    return S_OK;
}

// Remove specified net from link list
RESULT CBDNodeCSN::RemoveCSNLink
( 
    CBDNode * pNode
)
{
    RESULT hr = E_FAIL;

    for( NodeList::iterator iter = m_lstLinks.begin(); iter != m_lstLinks.end(); iter++ )
    {
        if( (*iter) == pNode )
        {
            // Found the one to remove
            m_lstLinks.erase( iter );
            hr = S_OK;
            break;
        }
    }

    return hr;
}

// Tell all linked nets to remove this net from their lists
RESULT CBDNodeCSN::FlushCSNLinks()
{
    for( NodeList::iterator iter = m_lstLinks.begin(); iter != m_lstLinks.end(); iter++ )
    {
        // Remove this Netlist from all other nets' links
        (*iter)->CSN()->RemoveCSNLink( m_pNode );
    }

    return S_OK;
}


RESULT CBDNodeCSN::UpdateCSN
(
    FLOAT flNetEnergy
)
{
    //////
    // Decay the activation
    m_flCSNOutputNew = m_flCSNOutput * m_params.flCSNDecay;

    //////
    // Calculate the linked weighting contributions
    FLOAT flCSNChange = 0;
    for( NodeList::iterator iter = m_lstLinks.begin(); iter != m_lstLinks.end(); iter++ )
    {
        // Calculate link strength
        FLOAT flLCMThem = (*iter)->Period();
        FLOAT flLCMUs = m_pNode->Period();

        while( fabs(flLCMThem - flLCMUs) > 0.05 )
        {
            if( flLCMThem < flLCMUs )
                flLCMThem += (*iter)->Period();
            else
                flLCMUs += m_pNode->Period();
        }
        FLOAT flX = flLCMUs / m_pNode->Period();

        //FLOAT flLink = (pow(0.93f, flX*flX) / 0.93f) * 
        //               (m_params.flCSNMaxLink-m_params.flCSNMinLink) + m_params.flCSNMinLink;

        //FLOAT flLink = (1.0f/sqrt(flX)) * (m_params.flCSNMaxLink-m_params.flCSNMinLink) + m_params.flCSNMinLink;
        FLOAT flLink = (1.0f/flX) * (m_params.flCSNMaxLink-m_params.flCSNMinLink) + m_params.flCSNMinLink;

        flCSNChange += flLink * (*iter)->TimingNet()->NetEnergy();
        //flCSNChange += flLink * (*iter)->CSNOutput();
    }
    // Add the leaky integrator input to the change
    flCSNChange += flNetEnergy * m_params.flCSNInputLink;

    //////
    // Weight the activation change by the distance between current value and
    // the max or min allowed activation level
    if( flCSNChange > 0 )
        flCSNChange *= m_params.flCSNMaxAct - m_flCSNOutput;
    else
        flCSNChange *= m_flCSNOutput - m_params.flCSNMinAct;
    
    //////
    // Calculate new CSN unit activation
    m_flCSNOutputNew = std::max(m_params.flCSNMinAct,std::max(m_params.flCSNMaxAct,
                                m_flCSNOutputNew + flCSNChange ));

    return S_OK;
}


RESULT CBDNodeCSN::CommitCSN()
{
    m_flCSNOutput = m_flCSNOutputNew;
    return S_OK;
}


RDH_BD_END_NAMESPACE
