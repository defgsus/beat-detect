// AudioStream.cpp: implementation of the CAudioStream class.
//
//////////////////////////////////////////////////////////////////////

#include <iostream>

#include <sndfile.h>

#include "AudioStream.h"

RDH_BD_BEGIN_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAudioStream::CAudioStream()
{
}

CAudioStream::~CAudioStream()
{
}


RESULT CAudioStream::LoadFromFile(const char* fn)
{
    // Cannot load wave file if memory already allocated for another wave
    if( nullptr != m_pData )
        return E_FAIL;

    SF_INFO info;
    info.format = 0;
    SNDFILE *f = sf_open(fn, SFM_READ, &info);
    if (!f)
    {
        std::cerr << "Could not open '" << fn << "'\n"
                    << sf_strerror((SNDFILE*)0);
        return S_FAIL;
    }

    m_nChannels = info.channels;
    m_nSampleRate = info.samplerate;
    m_nBitsPerSample = 32;
    m_nSamples = info.frames;
    m_fNormalized = true;

    m_pData = malloc(m_nSamples * m_nChannels * sizeof(FLOAT));

    sf_readf_float(f, (FLOAT*)m_pData, m_nSamples);

    sf_close(f);

    return S_OK;
}

#if 0
RESULT CAudioStream::LoadReadWaveData
( 
  HMMIO hmmioFile, 
  LPMMCKINFO pckInRIFF
)
{
    RESULT hr = S_OK;

    // Descend into data chunk
    MMCKINFO ckIn;
    ckIn.ckid = mmioFOURCC('d', 'a', 't', 'a');     
    if( mmioDescend(hmmioFile, &ckIn, pckInRIFF, MMIO_FINDCHUNK) == 0 ) 
    {
        MMIOINFO    mmioinfoIn;
        if( mmioGetInfo(hmmioFile, &mmioinfoIn, 0) == 0) 
        {
            // Allocate Buffer
            m_pData = new BYTE[ckIn.cksize];
            if( m_pData != nullptr )
            {
                // Copy all the data bytes
                for (unsigned int cT = 0; cT < ckIn.cksize; cT++) 
                { 
                    if (mmioinfoIn.pchNext == mmioinfoIn.pchEndRead) 
                    {     
                        if ( mmioAdvance(hmmioFile, &mmioinfoIn, MMIO_READ) != 0) 
                        {
                            hr = E_FAIL;
                            break;
                        }
                        if (mmioinfoIn.pchNext == mmioinfoIn.pchEndRead) 
                        {
                            hr = E_FAIL;
                            break;
                        }
                    }  
                    // Actual copy. 
                    *((BYTE*)m_pData+cT) = *((BYTE*)mmioinfoIn.pchNext);
                    (BYTE*)mmioinfoIn.pchNext++;
                }

                
                if( SUCCEEDED(hr) )
                {
                    // Calculate number of samples = numbytes / (bytespersample * numchannels)
                    m_nSamples = ckIn.cksize / ((m_nBitsPerSample/8) * m_nChannels);
                }
                else
                {
                    // Failed, delete buffer
                    delete[] m_pData;
                }
            }
            else
            { 
                hr = E_OUTOFMEMORY;
            }
        }
        else
        {
            hr = E_FAIL;
        }
    }
    else
    {
        hr = E_FAIL;
    }

    return hr;
}
#endif

RESULT CAudioStream::SaveToFile(const char*)
{
    RESULT hr = S_OK;
#if 0
    // If no data, cannot save
    if( nullptr == m_pData )
        return E_FAIL;
    
    HMMIO hmmioFile = mmioOpen( (LPTSTR)lpszFilename, nullptr, MMIO_ALLOCBUF | MMIO_WRITE | MMIO_CREATE );
    if( hmmioFile == nullptr )
        return E_FAIL;
    
    MMCKINFO ckOutRiff;
    ckOutRiff.fccType = mmioFOURCC('W', 'A', 'V', 'E');
    ckOutRiff.cksize = 0;
    
    // Create main chunk
    if( mmioCreateChunk( hmmioFile, &ckOutRiff, MMIO_CREATERIFF ) != 0 )
    {
        hr = E_FAIL;
    }

    // Fill in format chunk
    if( SUCCEEDED(hr) )
    {
        MMCKINFO ckOut;
        ckOut.ckid = mmioFOURCC('f', 'm', 't', ' ');     
        ckOut.cksize = sizeof(PCMWAVEFORMAT);   
        if( mmioCreateChunk( hmmioFile, &ckOut, 0) == 0)
        {
            WAVEFORMATEX wfx;
            wfx.wFormatTag = WAVE_FORMAT_PCM;
            wfx.nChannels = GetNumChannels();
            wfx.nSamplesPerSec = GetSampleRate();
            wfx.nAvgBytesPerSec = GetNumChannels() * GetSampleRate() * GetBitsPerSample()/8;
            wfx.nBlockAlign = GetNumChannels() * GetBitsPerSample()/8;
            wfx.wBitsPerSample = GetBitsPerSample();
            if( mmioWrite( hmmioFile, (HPSTR)&wfx, sizeof(PCMWAVEFORMAT)) == sizeof(PCMWAVEFORMAT)) 
            {
                if( mmioAscend( hmmioFile, &ckOut, 0) != 0)  
                {
                    hr = E_FAIL;
                }
            }
            else
            {
                hr = E_FAIL;
            }
        }
        else
        {
            hr = E_FAIL;
        }        
    }

    // Write wave file data chunk
    if( SUCCEEDED(hr) )
    {
        hr = SaveWriteWaveData( hmmioFile );
    }
        
    // Ascend to top of file
    if( SUCCEEDED(hr) )
    {
        if( mmioAscend( hmmioFile, &ckOutRiff, 0) != 0)
        {
            hr = E_FAIL;
        }
    }

    // Close the final file.
    if( hmmioFile != nullptr )
    {
        mmioClose( hmmioFile, 0 ); 
        hmmioFile = nullptr;
    }
#endif
    return hr;
}


#if 0
RESULT CAudioStream::SaveWriteWaveData
( 
  HMMIO hmmioFile
)
{
    RESULT hr = S_OK;
    
    // Descend into data chunk
    MMCKINFO ckOut;
    ckOut.ckid = mmioFOURCC('d', 'a', 't', 'a'); 
    ckOut.cksize = 0;     
    if( mmioCreateChunk(hmmioFile, &ckOut, 0) == 0) 
    {
        MMIOINFO    mmioinfoOut;
        if( mmioGetInfo(hmmioFile, &mmioinfoOut, 0) == 0) 
        {
            int cbWrite = GetNumChannels() * GetBitsPerSample()/8 * GetNumSamples();
            int cbActualWrite = 0;
            
            // Write wave data
            for (int cT=0; cT < cbWrite; cT++) 
            {        
                if (mmioinfoOut.pchNext == mmioinfoOut.pchEndWrite) 
                { 
                    mmioinfoOut.dwFlags |= MMIO_DIRTY; 
                    if( mmioAdvance(hmmioFile, &mmioinfoOut, MMIO_WRITE) != 0) 
                    {     
                        break; 
                    } 
                } 
                *((BYTE*)mmioinfoOut.pchNext) = *((BYTE*)m_pData+cT); 
                (BYTE*)mmioinfoOut.pchNext++;
                cbActualWrite++; 
            }     
            
            // Check wrote all we have to write
            if( cbActualWrite < cbWrite )
            {
                hr = E_FAIL;
            }

            mmioinfoOut.dwFlags |= MMIO_DIRTY;     
            if( mmioSetInfo(hmmioFile, &mmioinfoOut, 0) == 0) 
            {
                if( mmioAscend( hmmioFile, &ckOut, 0) != 0)
                {
                    hr = E_FAIL;
                }
            }
            else
            {
                hr = E_FAIL;
            }
        }
        else
        {
            hr = E_FAIL;
        }
    }
    else
    {
        hr = E_FAIL;
    }
    
    return hr;
}
#endif


RDH_BD_END_NAMESPACE
