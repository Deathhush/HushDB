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
    class BufferManager
    {
        typedef std::map<PageId, Page*> BufferPool;
    public:
        BufferManager()
            : nextPageId(1)
        {
        }

        ~BufferManager()
        {
            for (BufferPool::iterator i = pagePool.begin(); i != pagePool.end(); i++)
            {
                delete i->second;
            }

            pagePool.clear();
        }

        template <typename TPage>
        TPage* AllocatePage()
        {
            TPage* page = new TPage();
            page->SetPageId(nextPageId);
            pagePool.insert(BufferPool::value_type(nextPageId, (Page*)page));

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
            Page* page = 0;
            if (pagePool.find(pageId) != pagePool.end())
            {
                page = pagePool[pageId];
            }
            return page;
        }

    private:
        PageId nextPageId;
        BufferPool pagePool;
    };
}

#endif