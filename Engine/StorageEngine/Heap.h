#ifndef HUSHDB_HEAP_H
#define HUSHDB_HEAP_H

#include <memory.h>
#include <vector>
using namespace std;

#include "..\..\Common\BasicType.h"
using namespace Hush;

#include "..\Client\Client.h"
#include "Page.h"
#include "BufferManager.h"


namespace HushDB
{
    class SimpleHeapHeaderPageData
    {
    public:
        static const UInt16 PageFixedDataSize = sizeof(SlotInfo) + sizeof(Int32) + 4 * sizeof(PageId) + 2 * sizeof(Int16) + sizeof(SlotId);
        static const UInt16 DataRegionSize = PageSize - PageFixedDataSize;

        SimpleHeapHeaderPageData()
            : pageId(InvalidPageId), slotCount(0), freeSpaceOffset(0), maxSlotId(InvalidSlotId), nextPage(EndOfPage)
        {
            memset(this->data, 0, DataPageData::DataRegionSize);
        }

    public:
        // SimpleHeapHeaderPage
        PageId FirstDataPageId;
        PageId LastDataPageId;       
        
        // DataPage layout
        Byte data[DataRegionSize]; // data region
        SlotInfo slots[1]; // grows backwards to data        
        Int32 slotCount;
        Int16 freeSpaceOffset; // the starting offset of free space.
        Int16 padding; // this is for memory alignment
        SlotId maxSlotId;
        PageId nextPage;

        // Page layout
        PageId pageId;
    };

    typedef DataPageImpl<SimpleHeapHeaderPageData> SimpleHeapHeaderPage;

    class SimpleHeap
    {        
    public:
        SimpleHeap(BufferManager* bufferManager)
            : bufferManager(bufferManager)
        {
            SimpleHeapHeaderPage* headerPage = bufferManager->AllocatePage<SimpleHeapHeaderPage>();
            DataPage* dataPage = bufferManager->AllocatePage<DataPage>();
            dataPage->SetNextPageId(EndOfPage);
            headerPage->FirstDataPageId = dataPage->GetPageId();
            headerPage->LastDataPageId = dataPage->GetPageId();

            this->headerPageId = headerPage->GetPageId();
        }

        PageId GetHeaderPageId() { return this->headerPageId; }

        RowId InsertRowPtr(const RowPtr& rowPtr)
        {
            SimpleHeapHeaderPage* headerPage = bufferManager->GetPageAs<SimpleHeapHeaderPage>(headerPageId);
            DataPage* currentPage = bufferManager->GetPageAs<DataPage>(headerPage->LastDataPageId);

            if (currentPage->GetAvailableSpace() < rowPtr.length)
            {

                DataPage* newDataPage = this->bufferManager->AllocatePage<DataPage>();
                newDataPage->SetNextPageId(EndOfPage);
                currentPage->SetNextPageId(newDataPage->GetPageId());
                headerPage->LastDataPageId = newDataPage->GetPageId();

                currentPage = newDataPage;
            }

            return currentPage->InsertRowPtr(rowPtr);
        }

        template <typename T>
        RowId InsertRowValue(const T& value)
        {
            RowPtr ptr{ (Byte*)(&value), sizeof(T) };
            return InsertRowPtr(ptr);
        }

        class Enumerator : public IEnumerator<RowPtr>
        {
        public:
            typedef shared_ptr<Enumerator> Ptr;

            Enumerator(SimpleHeap* heap)
                :heap(heap)
            {
                currentPageId = InvalidPageId;                
            }

            virtual bool MoveNext() override
            {
                if (currentPageId == EndOfPage)
                {
                    return false;
                }
            
                if (currentPageId == InvalidPageId)
                {
                    SimpleHeapHeaderPage* headerPage = heap->bufferManager->GetPageAs<SimpleHeapHeaderPage>(this->heap->headerPageId);
                    currentPageId = headerPage->FirstDataPageId;
                    lastPageId = headerPage->LastDataPageId;
                    currentDataPage = heap->bufferManager->GetPageAs<DataPage>(currentPageId);
                    this->currentPageEnumerator = currentDataPage->GetEnumerator();
                }

                bool hasNext = this->currentPageEnumerator->MoveNext();

                if (!hasNext)
                {
                    currentPageId = currentDataPage->GetNextPageId();
                    currentDataPage = nullptr;
                    currentPageEnumerator = nullptr;
                    if (currentPageId != EndOfPage)
                    {                        
                        currentDataPage = heap->bufferManager->GetPageAs<DataPage>(currentPageId);
                        this->currentPageEnumerator = currentDataPage->GetEnumerator();
                        hasNext = this->currentPageEnumerator->MoveNext();
                    }                    
                }

                return hasNext;                
            }

            virtual const RowPtr& Current() override
            {
                if (currentPageId == InvalidPageId)
                {
                    throw InvalidOperationException(T("Need to call MoveNext first."));
                }

                if (currentPageId == EndOfPage)
                {
                    throw OutOfRangeException(T("Reached the end."));
                }

                return this->currentPageEnumerator->Current();                
            }

        private:
            SimpleHeap* heap;
            IEnumerator<RowPtr>::Ptr currentPageEnumerator;
            PageId currentPageId;
            DataPage* currentDataPage;
            PageId lastPageId;
        };

        IEnumerator<RowPtr>::Ptr GetEnumerator()
        {
            return make_shared<Enumerator>(this);
        }

    private:
        PageId headerPageId;
        BufferManager* bufferManager;
    };
}

#endif