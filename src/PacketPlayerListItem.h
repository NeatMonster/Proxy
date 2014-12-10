#ifndef __Proxy__PacketPlayerListItem__
#define __Proxy__PacketPlayerListItem__

#include "Mojang.h"
#include "Packet.h"

#include <vector>

class Player;

class PacketPlayerListItem : public Packet {
public:
    enum Type {ADD_PLAYER, UPDATE_GAMEMODE, UPDATE_LATENCY, UPDATE_DISPLAY_NAME, REMOVE_PLAYER};

    struct Action {
        Mojang::Profile profile;
        varint_t gameMode;
        varint_t ping;
        bool hasDisplayName;
        string_t displayName;
    };

    Type type;
    std::vector<Action> actions;

    PacketPlayerListItem();

    void read(ByteBuffer&);

    void write(ByteBuffer&);

    void handle(PacketHandler*);
};

#endif /* defined(__Proxy__PacketPlayerListItem__) */
