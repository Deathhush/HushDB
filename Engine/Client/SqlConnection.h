#ifndef HUSH_SQLCONNECTION_H
#define HUSH_SQLCONNECTION_H

#include <memory>
using namespace std;

#include "..\..\Common\BasicType.h"
using namespace Hush;

#include "..\Catalog.h"
#include "..\StorageEngine\DataFile.h"

namespace HushDB
{
    class SqlConnection
    {
    public:
        typedef shared_ptr<SqlConnection> Ptr;

        String Database;

        void Open()
        {
            this->bufferManager = make_shared<BufferManager>(this->Database);
            this->catalog = make_shared<Catalog>(this->bufferManager.get());            
        }

        BufferManager* GetBufferManager()
        {
            return this->bufferManager.get();
        }

        Catalog* GetCatalog()
        {
            return this->catalog.get();
        }

    private:
        BufferManager::Ptr bufferManager;
        Catalog::Ptr catalog;
    };
}

#endif
