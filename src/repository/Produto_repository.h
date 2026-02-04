#ifndef PRODUTO_REPOSITORY_H
#define PRODUTO_REPOSITORY_H

#include <QSqlDatabase>
#include <QSqlQueryModel>
#include "../dto/Produto_dto.h"

class Produto_Repository
{
public:
    explicit Produto_Repository(QSqlDatabase db);

    bool codigoBarrasExiste(const QString &codigo);
    bool inserir(const ProdutoDTO &p, QString &erroSQL);
    QSqlQueryModel* listarProdutos();
    QSqlQueryModel *getProdutoPeloCodigo(const QString &codigoBarras);
    bool deletar(const QString &id, QString &erroSQL);
private:
    QSqlDatabase db;
};

#endif // PRODUTO_REPOSITORY_H
