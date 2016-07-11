// AudioStream.h: interface for the CPCMAudio class.
//
//////////////////////////////////////////////////////////////////////

#ifndef RDHSRC_AUDIOSTREAM_H
#define RDHSRC_AUDIOSTREAM_H

#include "DataStream.h"

RDH_BD_BEGIN_NAMESPACE

class CAudioStream : public CDataStream
{
public:
    CAudioStream();
    virtual ~CAudioStream();
    
    RESULT LoadFromWaveFile( const char* );
    RESULT SaveToWaveFile( const char* );
        
protected:

    //RESULT LoadReadWaveData( HMMIO hmmioFile, LPMMCKINFO pckInRIFF );
    //RESULT SaveWriteWaveData( HMMIO hmmioFile );
};

RDH_BD_END_NAMESPACE

#endif // RDHSRC_AUDIOSTREAM_H
