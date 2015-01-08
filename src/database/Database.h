#ifndef __Proxy__Database__
#define __Proxy__Database__

#include "Profile.h"
#include "Types.h"

#include "mongo/client/dbclient.h"

class Database {
public:
    Database();

    bool run();

    void addProfile(Profile*);

private:
    mongo::DBClientConnection c;
};

#endif /* defined(__Proxy__Database__) */
