#ifndef __Proxy__Database__
#define __Proxy__Database__

#include "mongo/client/dbclient.h"

class Database {
public:
    Database();

    bool run();

private:
    mongo::DBClientConnection c;
};

#endif /* defined(__Proxy__Database__) */
