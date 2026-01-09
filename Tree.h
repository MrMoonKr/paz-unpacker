#pragma once

#ifndef _TREE_H_
#define _TREE_H_

#include <Windows.h>
#include <CommCtrl.h>
#include <vector>
#include <string>
#include <algorithm>
#include <functional>

#include "MetaFile.h"
#include "PazFile.h"
#include "Helper.h"

namespace kukdh1
{
    class Tree
    {
    public:
        enum TREE_TYPE
        {
            TREE_TYPE_ROOT,
            TREE_TYPE_FOLDER,
            TREE_TYPE_FILE
        };

    private:
        Tree*               m_pParent;
        std::vector<Tree*>  vChildFolders;
        std::vector<Tree*>  vChildFiles;
        std::string         m_Name;

        TREE_TYPE           m_Type;
        BOOL                m_Added;

        HTREEITEM           m_hTreeItem;
        FileInfo            m_FileInfo;
        PADAsset            m_AssetInfo;
        LARGE_INTEGER       m_Capacity;


    public:
        Tree( TREE_TYPE type );
        ~Tree();

        void AddToTree(HWND hTree);
        void AddChildsToTree(HWND hTree);
        void AddGrandchildsToTree(HWND hTree, LPVOID arg, std::function<void(LPVOID, size_t, size_t)> callback);
        void SetFileInfo(Tree* pParent, std::string& name, FileInfo& fiFile);
        void SetFileInfo(Tree* pParent, std::string& name, PADAsset& asset);
        void SetFolderInfo(Tree* pParent, std::string& name);
        void AddChild(Tree* pChild);
        void SortChild();
        LARGE_INTEGER UpdateCapacity();

        TREE_TYPE GetType();
        FileInfo GetFileInfo();
        HTREEITEM GetHandle();
        BOOL IsAdded();
        BOOL IsChildAdded();
        BOOL IsGrandchildAdded();
        std::string GetName();
        LARGE_INTEGER GetCapacity();
        size_t GetFileCount();
        size_t GetFolderCount();
        Tree* GetChildFolderWithName(std::string name);
        void GetFileList(std::vector<kukdh1::FileInfo>& vList);
        void GetFileList(std::vector<kukdh1::PADAsset>& vList);
    };
}

#endif
