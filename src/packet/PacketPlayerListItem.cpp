#include "PacketPlayerListItem.h"

#include "PacketHandler.h"

#include <iomanip>
#include <sstream>

PacketPlayerListItem::PacketPlayerListItem() : Packet(0x38) {}

void PacketPlayerListItem::read(PacketBuffer &buffer) {
    varint_t type;
    buffer.getVarInt(type);
    this->type = (Type) type;
    varint_t size;
    buffer.getVarInt(size);
    for (size_t i = 0; i < size; i++) {
        Action action;
        long_t msb, lsb;
        buffer.getLong(msb);
        buffer.getLong(lsb);
        std::stringstream ss;
        ss << std::hex << std::setfill('0') << std::setw(16) << msb;
        ss << std::hex << std::setfill('0') << std::setw(16) << lsb;
        string_t uuid = ss.str();
        uuid.insert(uuid.begin() + 8, '-');
        uuid.insert(uuid.begin() + 13, '-');
        uuid.insert(uuid.begin() + 18, '-');
        uuid.insert(uuid.begin() + 23, '-');
        action.profile.uuid = uuid;
        if (type ==  Type::ADD_PLAYER) {
            buffer.getString(action.profile.name);
            varint_t propertiesSize;
            buffer.getVarInt(propertiesSize);
            for (size_t j = 0; j < propertiesSize; j++) {
                Mojang::Profile::Property property;
                buffer.getString(property.name);
                buffer.getString(property.value);
                buffer.getBool(property.isSigned);
                if (property.isSigned)
                    buffer.getString(property.signature);
                action.profile.properties.push_back(property);
            }
        }
        if (type == Type::ADD_PLAYER || type == Type::UPDATE_GAMEMODE)
            buffer.getVarInt(action.gameMode);
        if (type == Type::ADD_PLAYER || type == Type::UPDATE_LATENCY)
            buffer.getVarInt(action.ping);
        if (type == Type::ADD_PLAYER || type == Type::UPDATE_DISPLAY_NAME) {
            buffer.getBool(action.hasDisplayName);
            if (action.hasDisplayName)
                buffer.getString(action.displayName);
        }
        actions.push_back(action);
    }
}

void PacketPlayerListItem::write(PacketBuffer &buffer) {
    buffer.putVarInt(type);
    buffer.putVarInt(actions.size());
    for (Action const &action : actions) {
        buffer.putLong(std::stoull(action.profile.uuid.substr(0, 8)
                                 + action.profile.uuid.substr(9, 4)
                                 + action.profile.uuid.substr(14, 4), nullptr, 16));
        buffer.putLong(std::stoull(action.profile.uuid.substr(19, 4)
                                 + action.profile.uuid.substr(24, 12), nullptr, 16));
        if (type ==  Type::ADD_PLAYER) {
            buffer.putString(action.profile.name);
            buffer.putVarInt(action.profile.properties.size());
            for (Mojang::Profile::Property const &property : action.profile.properties) {
                buffer.putString(property.name);
                buffer.putString(property.value);
                buffer.putBool(property.isSigned);
                if (property.isSigned)
                    buffer.putString(property.signature);
            }
        }
        if (type == Type::ADD_PLAYER || type == Type::UPDATE_GAMEMODE)
            buffer.putVarInt(action.gameMode);
        if (type == Type::ADD_PLAYER || type == Type::UPDATE_LATENCY)
            buffer.putVarInt(action.ping);
        if (type == Type::ADD_PLAYER || type == Type::UPDATE_DISPLAY_NAME) {
            buffer.putBool(action.hasDisplayName);
            if (action.hasDisplayName)
                buffer.putString(action.displayName);
        }
    }
}

void PacketPlayerListItem::handle(PacketHandler *handler) {
    handler->handlePlayerListItem(this);
}
