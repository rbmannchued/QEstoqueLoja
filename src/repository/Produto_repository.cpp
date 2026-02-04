#include "Produto_repository.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

Produto_Repository::Produto_Repository(QSqlDatabase db)
    : db(db)
{
}

bool Produto_Repository::codigoBarrasExiste(const QString &codigo)
{
    if (!db.isOpen()) {
        if (!db.open()) {
            qDebug() << "Erro ao abrir banco (codigoBarrasExiste)";
            return false;
        }
    }

    QSqlQuery query(db);
    query.prepare("SELECT COUNT(*) FROM produtos WHERE codigo_barras = :codigo");
    query.bindValue(":codigo", codigo);

    if (!query.exec()) {
        qDebug() << "SQL ERROR:" << query.lastError().text();
        return false;
    }

    query.next();
    return query.value(0).toInt() > 0;
}

bool Produto_Repository::inserir(const ProdutoDTO &p, QString &erroSQL)
{
    if (!db.isOpen()) {
        if (!db.open()) {
            erroSQL = "Erro ao abrir banco de dados";
            return false;
        }
    }

    QSqlQuery query(db);
    query.prepare(
        "INSERT INTO produtos "
        "(quantidade, descricao, preco, codigo_barras, nf, un_comercial, "
        "preco_fornecedor, porcent_lucro, ncm, cest, aliquota_imposto, csosn, pis) "
        "VALUES "
        "(:quantidade, :descricao, :preco, :codigo_barras, :nf, :un_comercial, "
        ":preco_fornecedor, :porcent_lucro, :ncm, :cest, :aliquota_imposto, :csosn, :pis)"
        );

    query.bindValue(":quantidade", p.quantidade);
    query.bindValue(":descricao", p.descricao);
    query.bindValue(":preco", p.preco);
    query.bindValue(":codigo_barras", p.codigoBarras);
    query.bindValue(":nf", p.nf);
    query.bindValue(":un_comercial", p.uCom);
    query.bindValue(":preco_fornecedor", p.precoFornecedor);
    query.bindValue(":porcent_lucro", p.percentLucro);
    query.bindValue(":ncm", p.ncm);
    query.bindValue(":cest", p.cest);
    query.bindValue(":aliquota_imposto", p.aliquotaIcms);
    query.bindValue(":csosn", p.csosn);
    query.bindValue(":pis", p.pis);

    if (!query.exec()) {
        erroSQL = query.lastError().text();
        qDebug() << "[SQL ERROR]" << erroSQL;
        qDebug() << "[SQL QUERY]" << query.lastQuery();
        return false;
    }

    return true;
}

QSqlQueryModel* Produto_Repository::listarProdutos()
{
    if (!db.isOpen()) {
        if (!db.open()) {
            qDebug() << "Erro ao abrir banco (listarProdutos)";
            return nullptr;
        }
    }

    auto *model = new QSqlQueryModel();
    model->setQuery("SELECT * FROM produtos ORDER BY id DESC", db);

    if (model->lastError().isValid()) {
        qDebug() << "Erro SQL:" << model->lastError().text();
    }

    return model;
}

QSqlQueryModel* Produto_Repository::getProdutoPeloCodigo(const QString &codigoBarras){
    if (!db.isOpen()) {
        if (!db.open()) {
            qDebug() << "Erro ao abrir banco (listarProdutos)";
            return nullptr;
        }
    }

    auto *model = new QSqlQueryModel();
    QString sql = QString(
                      "SELECT * FROM produtos WHERE codigo_barras = '%1' ORDER BY id DESC"
                      ).arg(codigoBarras);
    model->setQuery(sql, db);

    if (model->lastError().isValid()) {
        qDebug() << "Erro SQL:" << model->lastError().text();
    }

    return model;
}


bool Produto_Repository::deletar(const QString &id, QString &erroSQL)
{
    if (!db.isOpen()) {
        if (!db.open()) {
            erroSQL = "Erro ao abrir banco de dados";
            return false;
        }
    }

    QSqlQuery query(db);
    query.prepare(
        "DELETE FROM produtos WHERE id = :id"
        );

    query.bindValue(":id", id);

    if (!query.exec()) {
        erroSQL = query.lastError().text();
        qDebug() << "[SQL ERROR]" << erroSQL;
        qDebug() << "[SQL QUERY]" << query.lastQuery();
        return false;
    }

    return true;
}

