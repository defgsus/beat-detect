// BDOnsetStage.h: interface for the CBDOnsetStage class.
//
//////////////////////////////////////////////////////////////////////

#ifndef RDHSRC_BDONSETSTAGE_H
#define RDHSRC_BDONSETSTAGE_H

#include "AudioStream.h"

RDH_BD_BEGIN_NAMESPACE

static const int NUM_BANDS = 8;

class CBDOnsetStage  
{
public:
    CBDOnsetStage(const BDParamsType& params);
	virtual ~CBDOnsetStage();

    RESULT CreateOnsetStream(
            CAudioStream *pStrmIn, CDataStream *pStrmOut, CDataStream *pStrmInternal );

protected:
    // Split the input signal into its separate frequency bands
    RESULT BandSplitInput( CAudioStream *pStrmIn );
    // Reassemble onset streams into one output stream
    RESULT ReassembleOnsets( CDataStream *pStrmOut );

protected:
    BDParamsType    m_params;

    CAudioStream    m_aStrmBandInput[NUM_BANDS];
    CDataStream     m_aStrmOnset[NUM_BANDS];

};

RDH_BD_END_NAMESPACE

#endif // RDHSRC_BDONSETSTAGE_H
