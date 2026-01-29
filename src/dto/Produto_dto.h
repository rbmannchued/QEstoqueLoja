#ifndef PRODUTO_DTO_H
#define PRODUTO_DTO_H

#include <QString>

struct ProdutoDTO {
    QString quantidade;
    QString descricao;
    QString preco;
    QString codigoBarras;
    bool nf;
    QString uCom;
    QString precoFornecedor;
    QString percentLucro;
    QString ncm;
    QString cest;
    QString aliquotaIcms;
    QString csosn;
    QString pis;
};

#endif
