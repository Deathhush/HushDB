#ifndef HUSHDB_PAGE_H
#define HUSHDB_PAGE_H

#include <algorithm>
using namespace std;

#include <memory.h>

#include "..\..\Common\BasicType.h"
#include "..\Client\Client.h"
using namespace Hush;

namespace HushDB
{
    typedef Int32 PageId;
    typedef Int32 SlotId;
    const UInt32 PageSize = 8192;
    const PageId InvalidPageId = -1;
    const PageId EndOfPage = -2;
    const SlotId InvalidSlotId = 1; // slotId should be 0 or negative. 1 indicates invalid slot id.
    const SlotId EndOfSlot = 2;

    struct RowId
    {
        RowId(PageId pageId = InvalidPageId, SlotId slotId = InvalidSlotId)
            : pageId(pageId), slotId(slotId)
        {}
        PageId pageId;
        SlotId slotId;
    };

    struct SlotInfo
    {
        static const Int16 EmptySlotOffset = -1;
        static const SlotInfo EmptySlot;

        SlotInfo(int offset= SlotInfo::EmptySlot.offset, int length=SlotInfo::EmptySlot.length)
            : offset(offset), length(length)
        {
        }        
        
        bool operator==(const SlotInfo& slot) const
        {
            return (this->offset == slot.offset && this->length == slot.length);
        }

        bool operator!=(const SlotInfo& slot) const
        {
            return !this->operator==(slot);
        }

        Int16 offset; // offset within a page
        Int16 length; // length of a row
    };

    struct RowPtr
    {
        RowPtr(Byte* data = nullptr, Int32 length = 0)
            : data(data), length(length)
        {
        }
        Byte* data;
        Int32 length;
    };
    
    // Layout requirements:
    //  - The data region starts from the beginning.
    //  - Fields starts from the end.
    //  - The first field should be PageId;
    class PageData
    {
    public:
        static const UInt16 PageFixedDataSize = sizeof(PageId);
        static const UInt16 DataRegionSize = PageSize - PageFixedDataSize;

        PageData()
            : pageId(InvalidPageId)
        {
            memset(this->data, 0, PageData::DataRegionSize);
        }

    protected:
        Byte data[DataRegionSize];
        PageId pageId;
    };

    // The layout of DataPage's data should be compatible with PageData so that
    // a DataPage can be casted to a Page.
    class DataPageData
    {
    public:
        static const UInt16 PageFixedDataSize = sizeof(SlotInfo) + sizeof(Int32) + 2 * sizeof(PageId) + 2 * sizeof(Int16) + sizeof(SlotId);
        static const UInt16 DataRegionSize = PageSize - PageFixedDataSize;

        DataPageData()
            : pageId(InvalidPageId), slotCount(0), freeSpaceOffset(0), maxSlotId(InvalidSlotId), nextPage(EndOfPage)
        {
            memset(this->data, 0, DataPageData::DataRegionSize);
        }

    protected:
        // HeapPage layout
        Byte data[DataRegionSize]; // data region
        SlotInfo slots[1]; // grows backwards to data
        Int32 slotCount;
        Int16 freeSpaceOffset; // the starting offset of free space.
        Int16 padding; // this is for memory alignment
        SlotId maxSlotId;
        PageId nextPage;

        // This is from Page, should be at the end.
        PageId pageId;
    };

    template <typename TData>
    class PageImpl : public TData
    {
    public:
        PageImpl(PageId pageId = InvalidPageId)
        {
            this->pageId = pageId;
        }

        PageId GetPageId() const { return pageId; }
        void SetPageId(PageId pageId) { this->pageId = pageId; }

        bool operator==(const PageImpl& page) const
        {
            const PageImpl* pagePtr = &page;
            return memcmp((Byte*)this, (Byte*)pagePtr, PageSize) == 0;
        }

        bool operator!=(const PageImpl& page) const
        {
            return !this->operator==(page);
        }
    };

    typedef PageImpl<PageData> Page;    

    template <typename TData>
    class DataPageImpl : public PageImpl<TData>
    {
    public:
        DataPageImpl(PageId pageId = InvalidPageId)
            :PageImpl(pageId)
        {            
        }

        Int32 GetRowCount() const { return slotCount; }        
        Int16 GetFreeSpaceOffset() const { return freeSpaceOffset; }

        // When inserting new row, the needed space is length of data + length of a SlotInfo
        UInt16 GetAvailableSpace() const { return DataRegionSize - freeSpaceOffset - ((-this->maxSlotId) + 1)*sizeof(SlotInfo); }

        RowId InsertEmptyRow(Int32 length)
        {
            SlotId slotId = InvalidSlotId;
            // Try to find an empty slot id
            for (int i = 0; i >= this->maxSlotId; i--)
            {
                if (slots[i] == SlotInfo::EmptySlot)
                {
                    slotId = i;
                    break;
                }
            }

            if (slotId == InvalidSlotId)
            {
                this->maxSlotId--;
                slotId = this->maxSlotId;
            }

            this->slotCount++;

            this->slots[slotId].length = length;
            this->slots[slotId].offset = freeSpaceOffset;

            this->freeSpaceOffset += length;

            if (slotId < this->maxSlotId)
            {
                this->maxSlotId = slotId;
            }

            return RowId(this->pageId, slotId);

        }

        //TODO: Need to handle not enough free space scenario
        RowId InsertRowPtr(const RowPtr& rowPtr)
        {
            RowId rowId = InsertEmptyRow(rowPtr.length);
            memcpy((void*)&this->data[slots[rowId.slotId].offset], rowPtr.data, rowPtr.length);

            return rowId;
        }

        template <typename T>
        RowId InsertRowValue(const T& value)
        {
            RowPtr ptr{ (Byte*)(&value), sizeof(T) };            
            return InsertRowPtr(ptr);            
        }

        RowPtr GetRowPtr(const SlotId& slotId)
        {
            return RowPtr(&data[slots[slotId].offset], slots[slotId].length);
        }

        RowPtr GetRowPtr(const RowId& rowId)
        {
            return GetRowPtr(rowId.slotId);
        }

        Int16 GetRowOffset(const SlotId& slotId) const
        {
            return slots[slotId].offset;
        }

        Int16 GetRowOffset(const RowId& rowId) const
        {
            return slots[rowId.slotId].offset;
        }

        void DeleteRow(const SlotId& slotId)
        {
            Int16 startOffset = slots[slotId].offset;
            Int16 length = slots[slotId].length;
            Int16 endOffset = startOffset + length;


            //We need to move the data after endOffset to startOffset if there is any
            if (endOffset < this->freeSpaceOffset)
            {
                memcpy(&data[startOffset], &data[endOffset], this->freeSpaceOffset - endOffset);
            }

            slots[slotId] = SlotInfo::EmptySlot;

            for (int i = slotId - 1; i >= this->maxSlotId; i--)
            {
                if (slots[i] != SlotInfo::EmptySlot)
                {
                    slots[i].offset -= length;
                }
            }

            if (slotId == this->maxSlotId)
            {
                this->maxSlotId++;
            }

            this->freeSpaceOffset -= length;
            this->slotCount--;
        }

        void DeleteRow(RowId rowId)
        {
            this->DeleteRow(rowId.slotId);
        }        

        void SetNextPageId(PageId nextPage) { this->nextPage = nextPage; }
        PageId GetNextPageId() { return this->nextPage; }

        class Enumerator : public IEnumerator<RowPtr>
        {
        public:
            typedef shared_ptr<Enumerator> Ptr;

            Enumerator(DataPageImpl<TData>* page)
                : page(page), currentSlotId(InvalidSlotId)
            {}

            virtual bool MoveNext() override
            {
                if (this->currentSlotId == EndOfSlot)
                {
                    return false;
                }

                this->currentSlotId--;
                for (; this->currentSlotId >= this->page->maxSlotId; this->currentSlotId--)
                {
                    if (this->page->slots[this->currentSlotId] != SlotInfo::EmptySlot)
                    {
                        return true;
                    }
                }

                this->currentSlotId = EndOfSlot;
                return false;               
            }

            virtual const RowPtr& Current() override
            {
                if (this->currentSlotId == InvalidSlotId)
                {
                    throw InvalidOperationException(T("Current called before MoveNext."));
                }

                if (this->currentSlotId == EndOfSlot)
                {
                    throw OutOfRangeException(T("Reached the end."));
                }

                return RowPtr(&(this->page->data[this->page->slots[this->currentSlotId].offset]), this->page->slots[this->currentSlotId].length);
            }

        private:
            DataPageImpl<TData>* page;
            SlotId currentSlotId;
        };

        IEnumerator<RowPtr>::Ptr GetEnumerator()
        {
            return make_shared<Enumerator>(this);
        }

        RowPtr InsertDataRow(const vector<DbValue::Ptr>& fields)
        {
            int totalSize = 0;
            for_each(fields.begin(), fields.end(), [&](DbValue::Ptr v) { totalSize += v->Size(); });
            RowPtr rowPtr = this->GetRowPtr(this->InsertEmptyRow(totalSize));

            DataRow::CopyDataRow(rowPtr.data, fields);  

            return rowPtr;
        }
    };

    // Stores rows on the page. One row for a slot.
    typedef DataPageImpl<DataPageData> DataPage;


}

#endif
