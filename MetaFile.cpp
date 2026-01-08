#include "MetaFile.h"
#include <cassert>

#include "Crypt.h"


#define ICE_KEY                   ((uint8_t *)"\x51\xF3\x0F\x11\x04\x24\x6A\x00")
#define ICE_KEY_LEN               8


namespace kukdh1 
{
    PAZTable::PAZTable(uint8_t* buffer) 
    {
        memcpy(&uiPazFileID, buffer + 0, 4);
        memcpy(&uiCRC, buffer + 4, 4);
        memcpy(&uiSize, buffer + 8, 4);
    }

    Meta::Meta( wchar_t* wpszPazFolder ) 
    {
        std::fstream file;
        std::wstring path( wpszPazFolder );

        path.append( L"\\pad00000.meta" );

        file.open( path, std::ios::in | std::ios::binary );
        if ( file.is_open() ) 
        {
            uint8_t buffer[64];

            //// Read Header
            //file.read( (char*)buffer, 8 );
            //memcpy( &m_Version, buffer + 0, 4 );
            //memcpy( &m_PAZCount, buffer + 4, 4 );

            file.read( (char*)&m_Version, 4 );
            assert( m_Version == 4907 );

            m_PAZs.clear();
            file.read( (char*)&m_PAZCount, 4 );
            for ( uint32_t idx = 0 ; idx < m_PAZCount ; idx++ ) 
            {
                file.read( (char*)buffer, 12 );

                m_PAZs.push_back( PAZTable( buffer ) );
            }

            m_Assets.clear();
            PADAsset asset;
            file.read( (char*)&m_AssetCount, 4 );
            for ( uint32_t idx = 0 ; idx < m_AssetCount ; idx++ ) 
            {
                file.read( (char*)&asset, sizeof( asset ) );
                m_Assets.push_back( asset );
            }

            file.read( (char*)&m_FolderNameSize, 4 );
            char* encFolderNames = new char[m_FolderNameSize];
            char* decFolderNames = new char[m_FolderNameSize];
            file.read( encFolderNames, m_FolderNameSize );

            file.read( (char*)&m_FileNameSize, 4 );
            char* encFileNames = new char[m_FileNameSize];
            char* decFileNames = new char[m_FileNameSize];
            file.read( encFileNames, m_FileNameSize );

            CryptICE cipher(ICE_KEY, ICE_KEY_LEN);
            cipher.decrypt( (uint8_t*)encFolderNames, m_FolderNameSize, (uint8_t**)&decFolderNames, &m_FolderNameSize );
            cipher.decrypt( (uint8_t*)encFileNames, m_FileNameSize, (uint8_t**)&decFileNames, &m_FileNameSize );

            char* stringTable = NULL;
            char* stringIter = NULL;
            int32_t remainSize = 0;

            m_FolderNames.clear();
            {
                stringTable = decFolderNames;
                stringIter  = stringTable;
                remainSize  = m_FolderNameSize;
                while ( remainSize > 0 ) 
                {
                    uint32_t indexNumber    = *(uint32_t*)( stringIter + 0 );
                    uint32_t subFolder      = *(uint32_t*)( stringIter + 4 );

                    uint32_t nameSize       = strlen( stringIter + 8 );
                    if ( nameSize <= 0 )
                    {
                        stringIter += ( nameSize + 1 + 8 );
                        remainSize -= ( nameSize + 1 + 8 );
                        continue;
                    }

                    string name = ( stringIter + 8 ) ;
                    m_FolderNames.push_back( name );

                    stringIter += ( nameSize + 1 + 8 );
                    remainSize -= ( nameSize + 1 + 8 );
                }
                
            }

            m_FileNames.clear();
            {
                stringTable = (char*)decFileNames;
                remainSize = m_FileNameSize;

                stringIter = stringTable;
                while ( remainSize > 0 )
                {
                    uint32_t nameSize = strlen( stringIter );
                    if ( nameSize <= 0 )
                    {
                        break;
                    }

                    m_FileNames.push_back( stringIter );

                    stringIter += ( nameSize + 1 );
                    remainSize -= ( nameSize + 1 );
                }
            }

            m_AssetNames.clear();
            for ( uint32_t idx = 0 ; idx < m_AssetCount ; idx++ ) 
            {
                std::string assetName = m_FolderNames[ m_Assets[idx].mFolderNameIndex ] + m_FileNames[ m_Assets[idx].mFileNameIndex ];
                m_AssetNames.push_back( assetName );
            }

            file.close();
        }
        else 
        {
            throw std::exception();
        }
    }
}
