#include "databaseconnection_service.h"
#include "apppath_service.h"
#include <QSqlError>
#include <QDebug>

bool DatabaseConnection_service::initialized = false;

DatabaseConnection_service::DatabaseConnection_service(QObject *parent)
    : QObject{parent}
{}


bool DatabaseConnection_service::init()
{
    if (initialized) return true;

    try {
        if (!QSqlDatabase::contains("qt_sql_default_connection")) {
            QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
            db.setDatabaseName(AppPath_service::databasePath());

            if (db.databaseName().isEmpty()) {
                return false;
            }
        }

        initialized = true;
        return true;
    }
    catch (...) {
        return false;
    }
}

bool DatabaseConnection_service::open()
{
    init();
    QSqlDatabase db = QSqlDatabase::database();

    if (!db.isOpen()) {
        if (!db.open()) {
            qDebug() << "[DB ERROR]" << db.lastError().text();
            return false;
        }
    }
    return true;
}

void DatabaseConnection_service::close()
{
    QSqlDatabase db = QSqlDatabase::database();
    if (db.isOpen()) {
        db.close();
    }
}

QSqlDatabase DatabaseConnection_service::db()
{
    init();
    return QSqlDatabase::database();
}

