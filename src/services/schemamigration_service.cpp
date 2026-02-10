#include "schemamigration_service.h"
#include <QObject>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QCoreApplication>
#include <QStandardPaths>
#include <QDir>
#include <QFileInfo>
#include <QSql>
#include <QSqlError>
#include "../configuracao.h"
#include "../mainwindow.h"
#include "../services/Produto_service.h"
#include "../infra/databaseconnection_service.h"

const int SCHEMA_LATEST_VERSION = 7;

SchemaMigration_service::SchemaMigration_service(QObject *parent)
    : QObject{parent}
{}


SchemaMigration_service::Resultado SchemaMigration_service::init()
{
    if (!DatabaseConnection_service::init()) {
        return {false, SchemaErro::FalhaConexao, "Falha ao inicializar conexão"};
    }

    if (!DatabaseConnection_service::open()) {
        return {false, SchemaErro::FalhaConexao, "Falha ao abrir banco"};
    }

    QSqlDatabase db = DatabaseConnection_service::db();
    QSqlQuery query(db);

    // schema base
    if (!query.exec(R"(
        CREATE TABLE IF NOT EXISTS produtos (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            quantidade INTEGER,
            descricao TEXT,
            preco DECIMAL(10,2),
            codigo_barras VARCHAR(20),
            nf BOOLEAN
        )
    )")) return {false, SchemaErro::ErroSQL, query.lastError().text()};

    if (!query.exec(R"(
        CREATE TABLE IF NOT EXISTS vendas2 (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            cliente TEXT,
            data_hora DATETIME DEFAULT CURRENT_TIMESTAMP,
            total DECIMAL(10,2)
        )
    )")) return {false, SchemaErro::ErroSQL, query.lastError().text()};

    if (!query.exec(R"(
        CREATE TABLE IF NOT EXISTS produtos_vendidos (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            id_produto INTEGER,
            id_venda INTEGER,
            quantidade INTEGER,
            preco_vendido DECIMAL(10,2),
            FOREIGN KEY (id_produto) REFERENCES produtos(id),
            FOREIGN KEY (id_venda) REFERENCES vendas2(id)
        )
    )")) return {false, SchemaErro::ErroSQL, query.lastError().text()};

    return {true, SchemaErro::Nenhum, "Schema base criado"};
}


SchemaMigration_service::Resultado SchemaMigration_service::migrate()
{
    if (!DatabaseConnection_service::open()) {
        return {false, SchemaErro::FalhaConexao, "Falha ao abrir banco", -1, -1};
    }

    QSqlDatabase db = DatabaseConnection_service::db();
    QSqlQuery q(db);

    int oldVersion = 0;

    // leitura segura da versão
    if (!q.exec("PRAGMA user_version")) {
        return {false, SchemaErro::ErroMigracao, q.lastError().text(), -1, -1};
    }

    if (q.next()) {
        oldVersion = q.value(0).toInt();
    }
    q.finish();

    int currentVersion = oldVersion;

    if (currentVersion >= SCHEMA_LATEST_VERSION) {
        return {true, SchemaErro::Nenhum, "Schema já atualizado", currentVersion, currentVersion};
    }

    while (currentVersion < SCHEMA_LATEST_VERSION) {

        if (!db.transaction()) {
            return {false, SchemaErro::ErroMigracao, "Falha ao iniciar transação", oldVersion, currentVersion};
        }

        QSqlQuery q(db);   // 🔥 query nova por iteração
        bool ok = true;
        QString erro;

        switch (currentVersion) {

        case 0: {
            ok &= q.exec("ALTER TABLE vendas2 ADD COLUMN forma_pagamento VARCHAR(20)");
            ok &= q.exec("ALTER TABLE vendas2 ADD COLUMN valor_recebido DECIMAL(10,2)");
            ok &= q.exec("ALTER TABLE vendas2 ADD COLUMN troco DECIMAL(10,2)");
            ok &= q.exec("ALTER TABLE vendas2 ADD COLUMN taxa DECIMAL(10,2)");
            ok &= q.exec("ALTER TABLE vendas2 ADD COLUMN valor_final DECIMAL(10,2)");
            ok &= q.exec("ALTER TABLE vendas2 ADD COLUMN desconto DECIMAL(10,2)");

            ok &= q.exec("UPDATE vendas2 "
                         "SET data_hora = strftime('%Y-%m-%d %H:%M:%S', "
                         "substr(data_hora, 7, 4) || '-' || substr(data_hora, 4, 2) || '-' || substr(data_hora, 1, 2) "
                         "|| ' ' || substr(data_hora, 12, 8)) "
                         "WHERE substr(data_hora, 3, 1) = '-' AND substr(data_hora, 6, 1) = '-'");

            ok &= q.exec("UPDATE vendas2 SET forma_pagamento = 'Não Sei', "
                         "valor_recebido = total, "
                         "troco = 0, taxa = 0, "
                         "valor_final = total,"
                         "desconto = 0");

            ok &= q.exec("PRAGMA user_version = 1");

            if (ok) currentVersion = 1;
            break;
        }

        case 1: {
            ok &= q.exec("CREATE TABLE config (id INTEGER PRIMARY KEY AUTOINCREMENT, "
                         "key VARCHAR(255) NOT NULL UNIQUE, "
                         "value TEXT)");

            QStringList inserts = {
                "INSERT INTO config (key, value) VALUES ('nome_empresa', '')",
                "INSERT INTO config (key, value) VALUES ('endereco_empresa', '')",
                "INSERT INTO config (key, value) VALUES ('telefone_empresa', '')",
                "INSERT INTO config (key, value) VALUES ('cnpj_empresa', '')",
                "INSERT INTO config (key, value) VALUES ('email_empresa', '')",
                "INSERT INTO config (key, value) VALUES ('porcent_lucro', '40')",
                "INSERT INTO config (key, value) VALUES ('taxa_debito', '2')",
                "INSERT INTO config (key, value) VALUES ('taxa_credito', '3')"
            };

            for (auto &sql : inserts) ok &= q.exec(sql);

            // normalização
            QSqlQuery qs(db);
            if (qs.exec("SELECT id, descricao FROM produtos")) {
                while (qs.next()) {
                    int id = qs.value(0).toInt();
                    QString desc = Produto_Service::normalizeText(qs.value(1).toString());

                    QSqlQuery uq(db);
                    uq.prepare("UPDATE produtos SET descricao = :d WHERE id = :id");
                    uq.bindValue(":d", desc);
                    uq.bindValue(":id", id);
                    if (!uq.exec()) ok = false;
                }
            }
            qs.finish();

            ok &= q.exec("PRAGMA user_version = 2");
            if (ok) currentVersion = 2;
            break;
        }

        case 2: {
            ok &= q.exec("CREATE TABLE entradas_vendas (id INTEGER PRIMARY KEY AUTOINCREMENT, "
                         "id_venda INTEGER, total DECIMAL(10,2), data_hora DATETIME DEFAULT CURRENT_TIMESTAMP,"
                         "forma_pagamento VARCHAR(20), valor_recebido DECIMAL(10,2), troco DECIMAL(10,2), "
                         "taxa DECIMAL(10,2), valor_final DECIMAL(10,2), desconto DECIMAL(10,2), "
                         "FOREIGN KEY (id_venda) REFERENCES vendas2(id))");

            ok &= q.exec("ALTER TABLE vendas2 ADD COLUMN esta_pago BOOLEAN DEFAULT 1");
            ok &= q.exec("UPDATE vendas2 SET esta_pago = 1");

            ok &= q.exec("PRAGMA user_version = 3");
            if (ok) currentVersion = 3;
            break;
        }

        case 3: {
            ok &= q.exec("CREATE TABLE clientes (id INTEGER PRIMARY KEY AUTOINCREMENT, "
                         "nome TEXT NOT NULL, email TEXT, telefone TEXT, endereco TEXT, cpf TEXT, "
                         "data_nascimento DATE, data_cadastro DATETIME DEFAULT CURRENT_TIMESTAMP, eh_pf BOOLEAN)");

            ok &= q.exec("INSERT INTO clientes(nome, eh_pf) VALUES ('Consumidor', 1)");

            ok &= q.exec("CREATE TABLE vendas (id INTEGER PRIMARY KEY AUTOINCREMENT, "
                         "cliente TEXT, data_hora DATETIME DEFAULT CURRENT_TIMESTAMP, total DECIMAL(10,2), "
                         "forma_pagamento VARCHAR(20), valor_recebido DECIMAL(10,2), troco DECIMAL(10,2), "
                         "taxa DECIMAL(10,2), valor_final DECIMAL(10,2), desconto DECIMAL(10,2), "
                         "esta_pago BOOLEAN DEFAULT 1, id_cliente INTEGER, "
                         "FOREIGN KEY (id_cliente) REFERENCES clientes(id))");

            ok &= q.exec("INSERT INTO vendas SELECT id, cliente, data_hora, total, forma_pagamento, "
                         "valor_recebido, troco, taxa, valor_final, desconto, esta_pago, NULL FROM vendas2");

            ok &= q.exec("DROP TABLE vendas2");
            ok &= q.exec("ALTER TABLE vendas RENAME TO vendas2");

            ok &= q.exec("PRAGMA user_version = 4");
            if (ok) currentVersion = 4;
            break;
        }

        default:
            db.rollback();
            return {false, SchemaErro::ErroMigracao, "Versão desconhecida", currentVersion, currentVersion};
        }

        if (!ok) {
            erro = q.lastError().text();
            db.rollback();
            return {false, SchemaErro::ErroMigracao, erro, currentVersion, currentVersion};
        }

        if (!db.commit()) {
            db.rollback();
            return {false, SchemaErro::ErroMigracao, "Falha ao commit", currentVersion, currentVersion};
        }
    }
    qDebug() << "currentversion schema " << currentVersion;
    return {true, SchemaErro::Nenhum, "Migração concluída", oldVersion, currentVersion};
}



