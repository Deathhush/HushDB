#ifndef HUSHDB_DATAFILE_H
#define HUSHDB_DATAFILE_H

#include <memory.h>
#include <vector>
using namespace std;

#include "..\..\Common\BasicType.h"
using namespace Hush;

#include "..\Client\Client.h"
#include "Page.h"
#include "BufferManager.h"
#include "PageFile.h"
#include "Heap.h"

namespace HushDB
{
    class DataFileHeaderPageData
    {
    public:
        static const UInt16 PageFixedDataSize = 3 * sizeof(PageId);
        static const UInt16 DataRegionSize = PageSize - PageFixedDataSize;

        DataFileHeaderPageData()
            : pageId(InvalidPageId), ObjectDefPageId(InvalidPageId), ColumnDefPageId(InvalidPageId)
        {
            memset(this->data, 0, DataPageData::DataRegionSize);
        }

    public:
        // SimpleHeapHeaderPage
        PageId ObjectDefPageId;
        PageId ColumnDefPageId;

        // DataPage layout
        Byte data[DataRegionSize]; // data region

        // Page layout
        PageId pageId;
    };

    typedef PageImpl<DataFileHeaderPageData> DataFileHeaderPage;

    class DataFile
    {
    public:
        static void CreateEmptyDataFile(const String& fileName)
        {
            BufferManager bufferManager(T("test_data.hdf"));
            DataFileHeaderPage* headerPage = bufferManager.AllocatePage<DataFileHeaderPage>();
            SimpleHeap objectDefHeap(&bufferManager);
            headerPage->ObjectDefPageId = objectDefHeap.GetHeaderPageId();
            SimpleHeap columnDefHeap(&bufferManager);
            headerPage->ColumnDefPageId = columnDefHeap.GetHeaderPageId();
            bufferManager.ReleasePage(headerPage->GetPageId(), true);
        }
    };
}

#endif