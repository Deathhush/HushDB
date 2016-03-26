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
#include "DataRow.h"


namespace HushDB
{
    class SimpleHeapHeaderPageData
    {
    public:
        static const UInt16 PageFixedDataSize = sizeof(SlotInfo) + sizeof(Int32) + 4 * sizeof(PageId) + 2 * sizeof(Int16) + sizeof(SlotId);
        static const UInt16 DataRegionSize = PageSize - PageFixedDataSize;

        SimpleHeapHeaderPageData()
            : pageId(InvalidPageId), slotCount(0), freeSpaceOffset(0), maxSlotId(InvalidSlotId), nextPage(EndOfPage), FirstDataPageId(InvalidPageId), LastDataPageId(InvalidPageId)
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
        typedef shared_ptr<SimpleHeap> Ptr;

        SimpleHeap(BufferManager* bufferManager)
            : bufferManager(bufferManager)
        {
            SimpleHeapHeaderPage* headerPage = bufferManager->AllocatePage<SimpleHeapHeaderPage>();
            this->headerPageId = headerPage->GetPageId();

            DataPage* newDataPage = AllocateNewDataPage(headerPage);
            headerPage->FirstDataPageId = newDataPage->GetPageId();            

            bufferManager->ReleasePage(headerPage->GetPageId(), true);
            bufferManager->ReleasePage(newDataPage->GetPageId(), true);
        }

        SimpleHeap(BufferManager* bufferManager, PageId headerPageId)
            : bufferManager(bufferManager)
        {
            SimpleHeapHeaderPage* headerPage = bufferManager->GetPageAs<SimpleHeapHeaderPage>(headerPageId);
            this->headerPageId = headerPage->GetPageId();
            this->currentPageId = headerPage->FirstDataPageId;

            bufferManager->ReleasePage(headerPage->GetPageId());            
        }

        PageId GetHeaderPageId() { return this->headerPageId; }

        RowId InsertRowPtr(const RowPtr& rowPtr)
        {            
            DataPage* currentPage = bufferManager->GetPageAs<DataPage>(currentPageId);

            if (currentPage->GetAvailableSpace() < rowPtr.length)
            {
                SimpleHeapHeaderPage* headerPage = bufferManager->GetPageAs<SimpleHeapHeaderPage>(headerPageId);
                
                bufferManager->ReleasePage(currentPageId);
                DataPage* newDataPage = AllocateNewDataPage(headerPage);
                currentPage = newDataPage;

                bufferManager->ReleasePage(headerPage->GetPageId(), true);
            }

            RowId result = currentPage->InsertRowPtr(rowPtr);            
            bufferManager->ReleasePage(currentPage->GetPageId(), true);

            return result;
        }

        RowId InsertDataRow(const vector<DbValue::Ptr>& fields)
        {
            DataPage* currentPage = bufferManager->GetPageAs<DataPage>(currentPageId);

            Int32 rowSize = DataRow::CalculateRowSize(fields);

            if (currentPage->GetAvailableSpace() < rowSize)
            {
                SimpleHeapHeaderPage* headerPage = bufferManager->GetPageAs<SimpleHeapHeaderPage>(headerPageId);
                bufferManager->ReleasePage(currentPageId);

                DataPage* newDataPage = AllocateNewDataPage(headerPage);
                currentPage = newDataPage;

                bufferManager->ReleasePage(headerPage->GetPageId(), true);
            }

            RowId result = currentPage->InsertEmptyRow(rowSize);
            RowPtr rowPtr = currentPage->GetRowPtr(result);
            DataRow::CopyDataRow(rowPtr.data, fields);
            
            bufferManager->ReleasePage(currentPage->GetPageId(), true);

            return result;
        }

        template <typename T>
        RowId InsertRowValue(const T& value)
        {
            RowPtr ptr{ (Byte*)(&value), sizeof(T) };
            return InsertRowPtr(ptr);
        }

        IEnumerator<RowPtr>::Ptr GetEnumerator()
        {
            return make_shared<Enumerator>(this);
        }

    private:
        PageId headerPageId;
        PageId currentPageId;
        BufferManager* bufferManager;

        DataPage* AllocateNewDataPage(SimpleHeapHeaderPage* headerPage)
        {
            DataPage* newDataPage = bufferManager->AllocatePage<DataPage>();
            newDataPage->SetNextPageId(EndOfPage);
            headerPage->LastDataPageId = newDataPage->GetPageId();
            currentPageId = newDataPage->GetPageId();

            return newDataPage;
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

            ~Enumerator()
            {
                if (this->currentPageId != InvalidPageId && this->currentPageId != EndOfPage)
                {
                    this->heap->bufferManager->ReleasePage(this->currentPageId);
                }
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

                    // We don't release data page during the enumeration
                    currentDataPage = heap->bufferManager->GetPageAs<DataPage>(currentPageId);
                    this->currentPageEnumerator = currentDataPage->GetEnumerator();

                    this->heap->bufferManager->ReleasePage(headerPage->GetPageId());                    
                }

                bool hasNext = this->currentPageEnumerator->MoveNext();

                if (!hasNext)
                {
                    currentPageId = currentDataPage->GetNextPageId();
                    // About to move to the next page. Release the current page.
                    heap->bufferManager->ReleasePage(currentDataPage->GetPageId());
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

            virtual const RowPtr Current() override
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
    };
}

#endif