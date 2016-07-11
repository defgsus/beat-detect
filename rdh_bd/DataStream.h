// DataStream.h: interface for the CPCMAudio class.
//
//////////////////////////////////////////////////////////////////////

#ifndef RDHSRC_DATASTREAM_H
#define RDHSRC_DATASTREAM_H

#include "BDUtils.h"

RDH_BD_BEGIN_NAMESPACE

class CDataStream 
{
public:
    CDataStream();
    virtual ~CDataStream();

    // Create stream from existing stream or data
    virtual RESULT CreateData( INT32 nBitsPerSample, INT32 nSampleRate, INT32 nSamples, BOOL fNormalized = FALSE );
    virtual RESULT CreateData( CDataStream const *pDataStreamCopyFrom );

    void    ReleaseData();

    // Normalize and Denormalize - assumes use of FLOAT for normalized version
    RESULT Normalize();
    RESULT DeNormalize( INT32 nBitsPerSample );
       
    // Accessors
    INT32     GetNumChannels() const           { return m_nChannels; }
    INT32     GetBitsPerSample() const         { return m_nBitsPerSample; }
    INT32     GetSampleRate() const            { return m_nSampleRate; }
    INT32     GetNumSamples() const            { return m_nSamples; }
    DOUBLE    GetDuration() const              { return (DOUBLE)m_nSamples/m_nSampleRate; }
    BOOL      IsNormalized() const             { return m_fNormalized; }

    void *  GetData()                        { return m_pData; }
    bool    IsValid()                        { return m_pData != nullptr; }

    RESULT         Reallocate( INT32 nSamples );
        
protected:

    INT32 m_nChannels;
    INT32 m_nBitsPerSample;
    INT32 m_nSampleRate;
    INT32 m_nSamples;
    BOOL  m_fNormalized;
    
    void *m_pData;

};

RDH_BD_END_NAMESPACE

#endif // RDHSRC_DATASTREAM_H
