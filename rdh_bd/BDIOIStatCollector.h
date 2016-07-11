// BDIOIStatCollector.h: interface for the CBDIOIStatCollector class.
//
//////////////////////////////////////////////////////////////////////

#ifndef BDHSRC_BD_BDIOISTATCOLLECTOR_H
#define BDHSRC_BD_BDIOISTATCOLLECTOR_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <list>
#include "BDUtils.h"

RDH_BD_BEGIN_NAMESPACE

const int IOISTATS_PARZEN_HALF_WINDOW_SIZE = 5;
const int IOISTATS_PARZEN_WINDOW_SIZE = IOISTATS_PARZEN_HALF_WINDOW_SIZE * 2 + 1;


struct sIOIPeriodEntry
{
    FLOAT           flPeriod;
    INT32           nRefCount;
    FLOAT           flHighestEnergy;
};


typedef std::list<sIOIPeriodEntry> IOIPeriodsList;
typedef std::list<INT32> OnsetList;

const int IOISTATS_HISTLEN = 2000;

struct sIOIStats
{
    FLOAT           aflIOIHist[IOISTATS_HISTLEN];
    
    IOIPeriodsList  lstDominantIOI;
};



class CBDIOIStatCollector  
{
public:
    CBDIOIStatCollector(const BDParamsType&);
	virtual ~CBDIOIStatCollector();

    RESULT     Initialize( sIOIStats * pStats );

    RESULT     ExecuteStep( FLOAT flSample, sIOIStats * pStats );

protected:
    RESULT     FindDominantIOIs( FLOAT flPeriod, sIOIStats * pStats );

    BDParamsType m_params;
    OnsetList   m_lstOnset;

    INT32       m_nLastOnsetDelay;
    INT32       m_nMaxIOI;

    FLOAT       m_aflParzenWindow[IOISTATS_PARZEN_WINDOW_SIZE];

};

RDH_BD_END_NAMESPACE

#endif // BDHSRC_BD_BDIOISTATCOLLECTOR_H
