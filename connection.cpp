#include "connection.h"

Connection::Connection()
{

}

bool Connection::createconnect()
{

    bool test=false;

    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
    db.setDatabaseName("partenaireDATA");//inserer le nom de la source de données
    db.setUserName("system");//inserer nom de l'utilisateur
    db.setPassword("molka123B");//inserer mot de passe de cet utilisateur

    // Try to open the database
    if (db.open()) {
        test=true;
        qDebug() << "Connected to Oracle Database!";
        return  test;
    } else {
        qDebug() << "Connection failed: " << db.lastError().text();
        return  test;
    }



}
