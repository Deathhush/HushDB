#ifndef HUSHDB_BUFFERMANAGER_H
#define HUSHDB_BUFFERMANAGER_H

#include <memory.h>
#include <map>
using namespace std;

#include "..\..\Common\BasicType.h"
#include "Page.h"
using namespace Hush;

namespace HushDB
{
    struct PageFrame
    {
        Page* Page;
        int PinCount;
        bool IsDirty;
    };

    class BufferManager
    {
        typedef std::map<PageId, PageFrame> BufferPool;
    public:
        BufferManager()
            : nextPageId(1)
        {
        }

        ~BufferManager()
        {
            for (BufferPool::iterator i = pagePool.begin(); i != pagePool.end(); i++)
            {
                delete i->second.Page;
            }

            pagePool.clear();
        }

        template <typename TPage>
        TPage* AllocatePage()
        {
            TPage* page = new TPage();
            page->SetPageId(nextPageId);

            pagePool.insert(BufferPool::value_type(nextPageId, PageFrame{ (Page*)page, 1, true }));

            nextPageId++;

            return page;
        }

        Page* AllocatePage()
        {
            return AllocatePage<Page>();
        }

        template <typename TPage>
        inline TPage* GetPageAs(const PageId& pageId)
        {
            return (TPage*)GetPage(pageId);
        }

        Page* GetPage(const PageId& pageId)
        {
            Page* page = nullptr;
            if (pagePool.find(pageId) != pagePool.end())
            {
                page = pagePool[pageId].Page;
                pagePool[pageId].PinCount++;
            }
            return page;
        }

        void ReleasePage(const PageId& pageId, bool isDirty = false)
        {
            if (pagePool.find(pageId) != pagePool.end())
            {                
                PageFrame& frame = pagePool[pageId];
                frame.PinCount--;                   
                if (isDirty)
                {
                    frame.IsDirty = true;
                }
            }
        }

        PageFrame& GetFrame(const PageId& pageId)
        {
            return pagePool.at(pageId);
        }

    private:
        PageId nextPageId;
        BufferPool pagePool;
    };
}

#endif