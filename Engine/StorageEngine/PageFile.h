#ifndef HUSHDB_PAGEFILE_H
#define HUSHDB_PAGEFILE_H

#include <fstream>
using namespace std;

#include <memory.h>

#include "..\..\Common\BasicType.h"
#include "Page.h"

using namespace Hush;

namespace HushDB
{
    class PageFile
    {
    public:
        typedef shared_ptr<PageFile> Ptr;

        PageFile(const String& fileName)
            :fileName(fileName)
        {
            
        }
        void CreateIfNotExist()
        {
            FILE* f = _wfopen(fileName.c_str(), L"a");
            fclose(f);
        }
        void Open()
        {
            FILE* f = _wfopen(fileName.c_str(), L"a");
            fclose(f);   

            file = _wfopen(fileName.c_str(), L"r+b");
            nextPageId = this->Size() / PageSize;
        }

        UInt32 Size()
        {
            int fileNo = _fileno(file);
            struct stat st;
            fstat(fileNo, &st);
            return st.st_size;
        }

        UInt32 PageCount()
        {
            return this->Size() / PageSize;
        }

        void Flush()
        {
            fflush(file);
        }

        void Close()
        {
            fflush(file);
            fclose(file);
        }
        
        template <typename TPage>
        void AllocatePage(TPage* page)
        {
            UInt32 pageSize = sizeof(TPage);
            if (page->GetPageId() != nextPageId)
            {
                throw InvalidOperationException(T("Page id is not set correctly"));
            }            
            nextPageId++;
            fseek(file, 0, SEEK_END);            
            fwrite((Byte*)page, sizeof(Byte), pageSize, file);
        }


        template <typename TPage>
        TPage* ReadPageAs(PageId pageId)
        {
            UInt32 pageSize = sizeof(TPage);            
            unique_ptr<Byte[]> buffer(new Byte[pageSize]);
            
            fseek(file, pageId*pageSize, SEEK_SET);
            fread(buffer.get(), sizeof(Byte), pageSize, file);

            return (TPage*)buffer.release();
        }

        void ReadPageTo(Page* page, PageId pageId)
        {
            UInt32 pageSize = sizeof(Page);
            fseek(file, pageId*pageSize, SEEK_SET);
            fread(page, sizeof(Byte), pageSize, file);
        }

        template <typename TPage>
        void WritePage(TPage* page)
        {
            PageId pageId = page->GetPageId();
            if (pageId >= nextPageId)
            {
                throw OutOfRangeException(T("Page id is too large."));
            }

            UInt32 pageSize = sizeof(TPage);            
            fseek(file, pageId*pageSize, SEEK_SET);
            fwrite((Byte*)page, sizeof(Byte), pageSize, file);
        }

        PageId GetNextPageId() { return this->nextPageId; }

    private:
        FILE* file;
        String fileName;
        PageId nextPageId;
    };
}

#endif