#include "Database.h"

#include "Logger.h"

#include "mongo/bson/bson.h"

#include <stdexcept>

Database::Database() {
    mongo::client::initialize();
}

bool Database::run() {
    try {
        c.connect("localhost");
        Logger() << "Connexion à MongoDB établie avec succès" << std::endl;
        return true;
    } catch (const mongo::DBException &e) {
        Logger(LogLevel::WARNING) << "IMPOSSIBLE DE SE CONNECTER À MONGODB !" << std::endl;
        Logger(LogLevel::WARNING) << "L'erreur rencontrée est : " << e.what() << std::endl;
        Logger(LogLevel::WARNING) << "Peut-être que MongoDB n'est pas lancé ?" << std::endl;
    }
    return false;
}

void Database::addProfile(Profile *profile) {
    mongo::BSONObjBuilder profileObj;
    profileObj.append("name", profile->getName());
    mongo::BSONArrayBuilder propertiesObj;
    for (Profile::Property property : profile->getProperties()) {
        mongo::BSONObjBuilder propertyObj;
        propertyObj.append("name", property.name);
        propertyObj.append("value", property.value);
        propertyObj.append("isSigned", property.isSigned);
        if (property.isSigned)
            propertyObj.append("signature", property.signature);
        propertiesObj.append(propertyObj.obj());
    }
    profileObj.appendArray("properties", propertiesObj.obj());
    c.update("mf.profiles", BSON("_id" << profile->getUUID()), BSON("$set" << profileObj.obj()), true);
    std::cout << "Added profile" << std::endl;
}

std::pair<string_t, ushort_t> Database::getServer(mongo::OID _id) {
    auto cursor = c.query("mf.servers", BSON("id_" << _id));
    if (!cursor->more())
        throw std::runtime_error("OID invalide");
    mongo::BSONObj server = cursor->next();
    return std::make_pair(server["host"].String(), server["port"].Number());
}

std::map<string_t, std::pair<string_t, ushort_t>> Database::getServers() {
    std::map<string_t, std::pair<string_t, ushort_t>> servers;
    auto cursor = c.query("mf.servers");
    while (cursor->more()) {
        mongo::BSONObj server = cursor->next();
        servers[server["_id"].OID().toString()] = std::make_pair(server["host"].String(), server["port"].Number());
    }
    return servers;
}
