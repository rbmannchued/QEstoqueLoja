#include "test_db_factory.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QDir>

QSqlDatabase TestDbFactory::create()
{
    static int i = 0;
    QString connName = "test_conn_" + QString::number(i++);

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connName);

    // arquivo temporário real
    QString path = QDir::tempPath() + "/qestoque_test_" + connName + ".db";
    db.setDatabaseName(path);

    if (!db.open()) {
        qFatal("Falha ao abrir banco de teste");
    }

    QSqlQuery q(db);

    QString createTable = R"(
        CREATE TABLE IF NOT EXISTS produtos (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            quantidade INTEGER,
            descricao TEXT,
            preco DECIMAL(10,2),
            codigo_barras VARCHAR(20),
            nf BOOLEAN,
            un_comercial TEXT,
            preco_fornecedor DECIMAL(10,2) NULL,
            porcent_lucro REAL,
            ncm VARCHAR(8) DEFAULT '00000000',
            cest TEXT NULL,
            aliquota_imposto REAL NULL,
            csosn VARCHAR(5),
            pis VARCHAR(5),
            local TEXT NULL
        )
    )";

    if (!q.exec(createTable)) {
        qFatal(q.lastError().text().toUtf8());
    }

    return db;
}
