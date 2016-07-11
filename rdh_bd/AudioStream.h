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
    
    RESULT LoadFromFile( const char* );
    RESULT SaveToFile( const char* );
        
protected:

};

RDH_BD_END_NAMESPACE

#endif // RDHSRC_AUDIOSTREAM_H
