// BDRealTimeStage.h: interface for the CBDRealTimeStage class.
//
//////////////////////////////////////////////////////////////////////

#ifndef RDHSRC_BDREALTIMESTAGE_H
#define RDHSRC_BDREALTIMESTAGE_H

#include "AudioStream.h"

RDH_BD_BEGIN_NAMESPACE

class CBDRealTimeStage  
{
public:
    CBDRealTimeStage(const BDParamsType& params);
	virtual ~CBDRealTimeStage();

    RESULT CreateBeatStream( CDataStream *pStrmIn,         // Onset stream, constant tempo
                              CDataStream *pStrmOut,        // Beat steram, constant tempo
                              CDataStream *pStrmTempo,      // Sampling period, constant tempo
                              CDataStream *pStrmBeatPeriod, // Winning loop period, constant tempo
                              CDataStream *pStrmBeatInfo    // Extra Info
                            );

    BDParamsType m_params;
};

RDH_BD_END_NAMESPACE

#endif // RDHSRC_BDREALTIMESTAGE_H
