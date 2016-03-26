#ifndef HUSHDB_BUFFERMANAGER_H
#define HUSHDB_BUFFERMANAGER_H

#include <memory.h>
#include <map>
using namespace std;

#include "..\..\Common\BasicType.h"
#include "Page.h"
#include "PageFile.h"
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
        typedef shared_ptr<BufferManager> Ptr;
        typedef std::map<PageId, PageFrame> BufferPool;
    public:
        BufferManager()
            : nextPageId(0)
        {
            this->pageFile = nullptr;
        }

        BufferManager(const String& fileName)
        {
            this->pageFile = make_shared<PageFile>(fileName);
            this->pageFile->Open();
        }

        ~BufferManager()
        {
            if (this->pageFile != nullptr)
            {
                this->pageFile->Flush();
                this->pageFile->Close();
            }

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

            if (this->pageFile != nullptr)
            {                
                page->SetPageId(this->pageFile->GetNextPageId());
                this->pageFile->AllocatePage(page);
            }
            else
            {
                page->SetPageId(nextPageId);                
            }

            pagePool.insert(BufferPool::value_type(page->GetPageId(), PageFrame{ (Page*)page, 1, true }));
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
            else
            {
                // try to load from file
                if (this->pageFile != nullptr && pageId < this->pageFile->GetNextPageId())
                {
                    page = new Page();
                    this->pageFile->ReadPageTo(page, pageId);
                    pagePool.insert(BufferPool::value_type(page->GetPageId(), PageFrame{ (Page*)page, 1, false }));
                    page = pagePool[pageId].Page;
                    pagePool[pageId].PinCount++;
                }
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

                    if (this->pageFile != nullptr)
                    {
                        this->pageFile->WritePage(frame.Page);
                        frame.IsDirty = false;
                    }
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
        PageFile::Ptr pageFile;
    };
}

#endif