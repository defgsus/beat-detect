// BDOnsetDetect.h: interface for the CBDOnsetDetect class.
//
//////////////////////////////////////////////////////////////////////

#ifndef RDHSRC_BDONSETDETECT_H
#define RDHSRC_BDONSETDETECT_H

#include "AudioStream.h"

RDH_BD_BEGIN_NAMESPACE

class CBDOnsetDetect  
{
public:
    CBDOnsetDetect(const BDParamsType& params);
	virtual ~CBDOnsetDetect();

    RESULT CreateOnsetStream( CAudioStream *pStrmIn, CDataStream *pStrmOut, CDataStream *pStrmInternal );

protected:
	RESULT ThresholdStream( CDataStream *pStrmIn, CDataStream *pStrmEnv, CDataStream *pStrmOut );
	
	RESULT ProcessEnvelope( CDataStream *pStrmIn, CDataStream *pStrmOut );


    BDParamsType m_params;
};

RDH_BD_END_NAMESPACE

#endif // RDHSRC_BDONSETDETECT_H
