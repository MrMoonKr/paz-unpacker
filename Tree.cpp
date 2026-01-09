#include "Tree.h"

namespace kukdh1 
{
    bool TreeCompare( Tree* a, Tree* b )
    {
        if ( a->GetName().compare( b->GetName() ) <= 0 )
        {
            return true;
        }

        return false;
    }

    Tree::Tree( TREE_TYPE type )
    {
        m_pParent = NULL;
        m_Type = type;
        m_hTreeItem = NULL;
        m_Capacity.QuadPart = 0;
        m_Added = FALSE;

        m_AssetInfo;
    }

    Tree::~Tree() 
    {
        for (auto iter = vChildFiles.begin(); iter != vChildFiles.end(); iter++) {
            delete (*iter);
        }
        for (auto iter = vChildFolders.begin(); iter != vChildFolders.end(); iter++) 
        {
            delete (*iter);
        }
    }

    void Tree::AddToTree( HWND hTree ) 
    {
        if ( m_Added ) // 이미 추가됨
        {
            return;
        }

        if ( m_Type == TREE_TYPE_ROOT ) 
        {
            m_hTreeItem = AddTreeItem( hTree, NULL, TVI_ROOT, L"/", (LPARAM)this );
        }
        else if ( m_pParent != NULL )
        {
            std::wstring temp;

            ConvertWidechar( m_Name, temp );
            m_hTreeItem = AddTreeItem( hTree, m_pParent->GetHandle(), TVI_LAST, (WCHAR*)temp.c_str(), (LPARAM)this );
        }
        else
        {
            // Root가 아닌데 부모가 없음
            return;
        }

        m_Added = TRUE;
    }

    void Tree::AddChildsToTree( HWND hTree )
    {
        if ( this->m_Type != TREE_TYPE_FILE )
        {
            for ( auto iter = vChildFolders.begin(); iter != vChildFolders.end(); iter++)
            {
                (*iter)->AddToTree( hTree );
            }
            for ( auto iter = vChildFiles.begin(); iter != vChildFiles.end(); iter++ )
            {
                (*iter)->AddToTree( hTree );
            }
        }
    }

    void Tree::AddGrandchildsToTree( HWND hTree, LPVOID arg, std::function<void(LPVOID, size_t, size_t)> callback ) 
    {
        if (m_Type != TREE_TYPE_FILE) {
            size_t stAll = 0;
            size_t i = 0;

            for (auto iter = vChildFolders.begin(); iter != vChildFolders.end(); iter++) {
                stAll += (*iter)->GetFileCount() + (*iter)->GetFolderCount();
            }

            callback(arg, i, stAll);

            for (auto iter = vChildFolders.begin(); iter != vChildFolders.end(); iter++) {
                (*iter)->AddChildsToTree(hTree);
                i += (*iter)->GetFileCount() + (*iter)->GetFolderCount();
                callback(arg, i, stAll);
            }
        }
    }

    void Tree::SetFileInfo( Tree* pParent, std::string& name, FileInfo& fiFile )
    {
        this->m_pParent = pParent;
        m_FileInfo = fiFile;
        m_Name = name;

        m_Type = TREE_TYPE_FILE;
    }

    void Tree::SetFileInfo(Tree* pParent, std::string& name, PADAsset& fiFile) 
    {
        this->m_pParent = pParent;
        this->m_AssetInfo = fiFile;
        m_Name = name;

        m_Type = TREE_TYPE_FILE;
    }

    void Tree::SetFolderInfo( Tree* pParent, std::string& name )
    {
        this->m_pParent = pParent;
        m_Name = name;

        m_Type = TREE_TYPE_FOLDER;
    }

    void Tree::AddChild(Tree* pChild) {
        std::string insertString = pChild->GetName();
        bool bInserted = false;

        if (pChild->GetType() == TREE_TYPE_FILE) {
            vChildFiles.push_back(pChild);
        }
        else {
            vChildFolders.push_back(pChild);
        }
    }

    void Tree::SortChild() {
        // Can be parallelize
        if (m_Type != TREE_TYPE_FILE) {
            for (auto iter = vChildFolders.begin(); iter != vChildFolders.end(); iter++) {
                (*iter)->SortChild();
            }

            std::sort(vChildFolders.begin(), vChildFolders.end(), TreeCompare);
            std::sort(vChildFiles.begin(), vChildFiles.end(), TreeCompare);
        }
    }

    LARGE_INTEGER Tree::UpdateCapacity() {
        m_Capacity.QuadPart = 0;

        if (m_Type == TREE_TYPE_FILE) {
            m_Capacity.QuadPart = m_FileInfo.uiOriginalSize;
        }
        else {
            for (auto iter = vChildFiles.begin(); iter != vChildFiles.end(); iter++) {
                m_Capacity.QuadPart += (*iter)->UpdateCapacity().QuadPart;
            }

            for (auto iter = vChildFolders.begin(); iter != vChildFolders.end(); iter++) {
                m_Capacity.QuadPart += (*iter)->UpdateCapacity().QuadPart;
            }
        }

        return m_Capacity;
    }

    Tree* Tree::GetChildFolderWithName( std::string name ) 
    {
        Tree* ptr = NULL;
        for ( auto iter = vChildFolders.begin(); iter != vChildFolders.end(); ++iter ) 
        {
            if ( (*iter)->GetName().compare( name ) == 0 )
            {
                ptr = (*iter);
                break;
            }
        }

        return ptr;
    }

    Tree::TREE_TYPE Tree::GetType() {
        return m_Type;
    }

    FileInfo Tree::GetFileInfo() {
        return m_FileInfo;
    }

    HTREEITEM Tree::GetHandle() {
        return m_hTreeItem;
    }

    BOOL Tree::IsAdded() {
        return m_Added;
    }

    BOOL Tree::IsChildAdded() {
        if (m_Type != TREE_TYPE_FILE) {
            if (vChildFiles.size() > 0) {
                return vChildFiles.at(0)->IsAdded();
            }
            else if (vChildFolders.size() > 0) {
                return vChildFolders.at(0)->IsAdded();
            }
        }

        return TRUE;
    }

    BOOL Tree::IsGrandchildAdded() 
    {
        if ( m_Type != TREE_TYPE_FILE ) 
        {
            if ( vChildFolders.size() > 0 ) 
            {
                return vChildFolders.at(0)->IsChildAdded();
            }
        }

        return TRUE;
    }

    std::string Tree::GetName() {
        return m_Name;
    }

    LARGE_INTEGER Tree::GetCapacity() {
        return m_Capacity;
    }

    size_t Tree::GetFileCount() {
        return vChildFiles.size();
    }

    size_t Tree::GetFolderCount() {
        return vChildFolders.size();
    }

    void Tree::GetFileList(std::vector<kukdh1::FileInfo>& vList) {
        if (m_Type == TREE_TYPE_FILE) {
            vList.push_back(m_FileInfo);
        }
        else {
            for (auto iter = vChildFolders.begin(); iter != vChildFolders.end(); iter++) {
                (*iter)->GetFileList(vList);
            }
            for (auto iter = vChildFiles.begin(); iter != vChildFiles.end(); iter++) {
                (*iter)->GetFileList(vList);
            }
        }
    }

    void Tree::GetFileList(std::vector<kukdh1::PADAsset>& vList) {
        if (m_Type == TREE_TYPE_FILE) {
            vList.push_back(m_AssetInfo);
        }
        else {
            for (auto iter = vChildFolders.begin(); iter != vChildFolders.end(); iter++) {
                (*iter)->GetFileList(vList);
            }
            for (auto iter = vChildFiles.begin(); iter != vChildFiles.end(); iter++) {
                (*iter)->GetFileList(vList);
            }
        }
    }
}
