// BDNodeControl.cpp: implementation of the CBDNodeControl class.
//
//////////////////////////////////////////////////////////////////////

#include <cmath>

#include "BDNodeControl.h"
#include "BDNode.h"
#include "BDNodeCSN.h"
#include "BDUtils.h"


RDH_BD_BEGIN_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBDNodeControl::CBDNodeControl()
    : m_pNodeBest           (nullptr)
    , m_flMaxCSN            (-1)
    , m_flMaxCSNTimeout     (0)
{

}

CBDNodeControl::~CBDNodeControl()
{
    NodeList::iterator iter = m_lstNodes.begin();
    while( iter != m_lstNodes.end() )
    {
        if( nullptr != *iter )
            delete *iter;
        iter++;
    }
    m_lstNodes.clear();
}


RESULT CBDNodeControl::Initialize(const BDParamsType& params)
{
    m_params = params;

    // Reset list of nets
    m_lstNodes.clear();

    return S_OK;
}


//
RESULT CBDNodeControl::ExecuteStep
( 
    FLOAT *     pflInputBuffer, 
    sIOIStats * pStats
)
{
    RESULT hr = S_OK;

    //////////////////////////////////////////////////////////////////
    // REMOVE, ADJUST and MARK
    // Compare Dominant IOI list with current timing net periods
    // Remove, adjust period, and reference count as necessary
    NodeList::iterator iNode = m_lstNodes.begin();
    while( iNode != m_lstNodes.end() )
    {
        BOOL  fFound = FALSE;
        FLOAT flIdealPeriod;

        // Search dominant list for this loop length
        for( IOIPeriodsList::iterator iDom = pStats->lstDominantIOI.begin();
             iDom != pStats->lstDominantIOI.end();
             iDom++ )
        {
            // If found a Node with a similar period (within tolerance)
            // we've found a match, so don't remove this node and clear
            // the similar node period entry from the dominant list
            if( fabs((*iNode)->Period() - (*iDom).flPeriod) < m_params.flNodeMaxDiff )
            {
                // Mark found, also find highest csn output
                fFound = TRUE;
                (*iDom).nRefCount++;
                if( (*iNode)->CSNOutput() > (*iDom).flHighestEnergy )
                    (*iDom).flHighestEnergy = (*iNode)->CSNOutput();

                // Remember ideal period
                flIdealPeriod = (*iDom).flPeriod;
                break;
            }
        }

        if( fFound )
        {
            // This node closely exists in the dominant IOI list, adjust the node to be
            // closer to the IOI histogram peak
            (*iNode)->SetIdealPeriod( flIdealPeriod );
            iNode++;
        }
        else if( ((*iNode)->CSNOutput() <= -0.5) && (m_pNodeBest != (*iNode)) )
        {
             // Remove this Node
            NodeList::iterator iterTemp = iNode;
            iNode++;

            // Tell Loop to remove itself
            (*iterTemp)->CSN()->FlushCSNLinks();
            if( m_pNodeBest == (*iterTemp) )
            {
                m_pNodeBest = nullptr;
            }

            delete *iterTemp;
            m_lstNodes.erase( iterTemp );
        }
        else
        {
            iNode++;
        }
        
    }


    //////////////////////////////////////////////////////////////////////
    // ADD
    // If there is anything left in the dominant IOI list, it should be added
    for( IOIPeriodsList::iterator iDom = pStats->lstDominantIOI.begin();
         iDom != pStats->lstDominantIOI.end();
         iDom++ )
    {
        if( (*iDom).nRefCount == 0 )
        {
            ////////////
            // No references, add a node
            FLOAT flPeriod = (*iDom).flPeriod;
            hr = AddNode( (*iDom).flPeriod );
        }
        else if( (*iDom).nRefCount > 1 )
        {
            // Debug
            FLOAT flP = (*iDom).flPeriod;
            INT32 iRef = (*iDom).nRefCount;
            FLOAT flE = (*iDom).flHighestEnergy;

            ///////////
            // More than one reference, remove all but most energetic node
            for( iNode = m_lstNodes.begin(); iNode != m_lstNodes.end(); )
            {
                if( fabs((*iNode)->Period() - (*iDom).flPeriod) < m_params.flNodeMaxDiff )
                {
                    // Matching node to dominant IOI, is highest energy?
                    if( (*iNode)->CSNOutput() < (*iDom).flHighestEnergy )
                    {
                        /////////////////////////////////
                        // No, so remove this weaker node
                        NodeList::iterator iterTemp = iNode;
                        iNode++;

                        // Tell Loop to remove itself
                        if( m_pNodeBest != (*iterTemp) )
                        {
                            (*iterTemp)->CSN()->FlushCSNLinks();
                            delete *iterTemp;
                            m_lstNodes.erase( iterTemp );
                        }                        
                    }
                    else
                    {
                        iNode++;
                    }
                }
                else 
                {
                    iNode++;
                }
            }
        }

        // Reset Stats
        (*iDom).nRefCount = 0;
        (*iDom).flHighestEnergy = 0;
    }

  

    /////////////////////////////////////////////////////////////////////////
    // UPDATE
    // Update Loops
    for( iNode = m_lstNodes.begin(); iNode != m_lstNodes.end() ; iNode++ )
    {   
        hr = (*iNode)->ExecuteStep( pflInputBuffer );
    }

    
    // Commit Step - Lock in CSN updated values, find top CSN Output
    CBDNode *  pNodeBestCandidate = nullptr;
    FLOAT flMaxCSN = m_params.flCSNMinAct;

    for( iNode = m_lstNodes.begin(); iNode != m_lstNodes.end() ; iNode++ )
    {   
        // Commit
        hr = (*iNode)->CommitStep();

        FLOAT flCSNOutput = (*iNode)->CSNOutput();


        // Option 2
        // Update statistics   
        if( flCSNOutput > flMaxCSN )
        {
            // A new best is found!
            pNodeBestCandidate = (*iNode);
            // Max CSN for now is just the CSN value - we want to find the best!
            flMaxCSN = flCSNOutput;
        }
/*
    
        // Testing/Debugging
        if( (fabs((*iNode)->Period() - 0.27f) < 0.01) )
        {
            pNodeBestCandidate = (*iNode);
        }
  */    
    }


    ////////////
    // Option 2
    ////////////
    if( nullptr != pNodeBestCandidate && nullptr == m_pNodeBest )
    {
        m_pNodeBest = pNodeBestCandidate;
    }

    // Output CSN results to parent
    if( (nullptr != m_pNodeBest) && (nullptr != pNodeBestCandidate) )
    {
        if( m_pNodeBest != pNodeBestCandidate )
        {
            // Current max has been superceded, increment timeout
            m_flMaxCSNTimeout += (FLOAT)1 / m_params.nOnsetSamplingRate;
            // Timeout - new top candidate becomes max loop
            if( m_flMaxCSNTimeout > m_params.flCSNOutputTimeThresh )
            {
                m_pNodeBest->m_fSelected = FALSE;
                m_pNodeBest = pNodeBestCandidate;
                m_flMaxCSNTimeout = 0;
                
                // Count the number of times we choose a new node
                if( m_params.fTrackPerformance )
                    m_params.nTrackChangeNode++;    
            }
        }
        else
        {   
            // Current loop is still the max, reset timeout
            m_flMaxCSNTimeout = 0;
        }
    }

    if( m_pNodeBest != nullptr && m_params.fTrackPerformance )
        m_pNodeBest->m_fSelected = TRUE;

    return hr;
}


// Insert new timing net and create links to all other nets
RESULT CBDNodeControl::AddNode
( 
    FLOAT flNodePeriod
)
{
    RESULT hr = E_OUTOFMEMORY;

    CBDNode *pNode = new CBDNode;
    if( nullptr != pNode )
    {
        // Init
        hr = pNode->Initialize( m_params, flNodePeriod );

        // Create Links
        for( NodeList::iterator iter = m_lstNodes.begin();
             iter != m_lstNodes.end() ; iter++ )
        {
            // Add link between new Net and existing Nets
            pNode->CSN()->AddCSNLink( *iter );
            (*iter)->CSN()->AddCSNLink( pNode );
        }

        // Add new net eo list
        m_lstNodes.push_back( pNode );
    }

    return hr;
}



RDH_BD_END_NAMESPACE
