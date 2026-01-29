#ifndef TEST_INSERIRPRODUTO_SERVICE_H
#define TEST_INSERIRPRODUTO_SERVICE_H

#include <QObject>
#include <QSqlDatabase>


class TestInserirProdutoService : public QObject
{
    Q_OBJECT
private:
        QSqlDatabase db;

private slots:
    void inserir_sucesso();
    void erro_codigo_barras_existente();
    void erro_preco_invalido();
    void erro_quantidade_invalida();
    void erro_ncm_nf();
    void cleanup();
};

#endif // TEST_INSERIRPRODUTO_SERVICE_H
