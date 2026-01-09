#pragma once

#ifndef _META_FILE_H_
#define _META_FILE_H_

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <exception>

using std::string;

namespace kukdh1 
{
    #pragma ( pack( push, 1 ) )
    /*
    * .paz file information in meta file
    */
    struct PADChunk
    {
        uint32_t            mFileNumber ;           ///< paz archive name
        uint32_t            mChecksum;              ///< hash 
        uint32_t            mFileSize;              ///< paz archive size

        PADChunk()
            : mFileNumber(0), mChecksum(0), mFileSize(0)
        {
        }

    };

    struct PADAsset
    {
        uint32_t            mHash;
        uint32_t            mFolderNameIndex;
        uint32_t            mFileNameIndex;
        uint32_t            mPazIndex;
        uint32_t            mOffset;
        uint32_t            mCompressedSize;
        uint32_t            mUncompressedSize;

        PADAsset()
            : mHash(0), mFolderNameIndex(0), mFileNameIndex(0), mPazIndex(0), mOffset(0), mCompressedSize(0), mUncompressedSize(0)
        {
        }

    };
    #pragma ( pack( pop ) )
    
    //------------------------------------------------------------------------

    /*
    * .paz file information in meta file
    */
    struct PAZTable 
    {
        uint32_t            uiPazFileID;
        uint32_t            uiCRC;             // CRC of PAZ (same as first 4bytes of PAZ file)
        uint32_t            uiSize;            // PAZ File Size

        PAZTable( uint8_t* buffer );
    };

    struct Meta 
    {
        uint32_t            m_Version;

        uint32_t            m_PAZCount;
        std::vector<PAZTable> m_PAZs;

        uint32_t            m_AssetCount;
        std::vector<PADAsset> m_Assets;

        uint32_t            m_FolderNameSize;
        std::vector<string> m_FolderNames;

        uint32_t            m_FileNameSize;
        std::vector<string> m_FileNames;

        std::vector<string> m_AssetNames;

        Meta( wchar_t* wpszPazFolder );
    };
}

#endif
