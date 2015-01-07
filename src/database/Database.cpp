#include "Database.h"

#include "Logger.h"

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
