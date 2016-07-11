// DSP.h: interface for the CDSP class.
//
//////////////////////////////////////////////////////////////////////

#ifndef RDHSRC_DSP_H
#define RDHSRC_DSP_H

#include "BDUtils.h"
#include "DataStream.h"

RDH_BD_BEGIN_NAMESPACE

typedef struct
{
    DOUBLE dA[7];
    DOUBLE dB[7];
    INT32  nSamplesDelay;
} FILTER_6TH_COEFF;



class CDSP  
{
public:
	
	CDSP();
	virtual ~CDSP();

    // IIR Filter with A,B coefficients
    static  RESULT DF2_Filter6( CDataStream* pStrmIn, CDataStream* pStrmOut,
                                 FILTER_6TH_COEFF* pCoeff, BOOL fReverseGroupDelay = FALSE );
    
    static RESULT RMSDecimate( CDataStream *pStrmIn, CDataStream *pStrmOut, INT32 nDec );
	
    static RESULT Convolve( CDataStream *pStrmIn, CDataStream *pStrmOut, const FLOAT aflKernel[], const INT32 nKernelLen );

    static RESULT Mix( CDataStream *pStrmIn1, FLOAT flVol1, CDataStream *pStrmIn2, FLOAT flVol2, CDataStream *pStrmOut );
 
};


RDH_BD_END_NAMESPACE

#endif // RDHSRC_DSP_H
