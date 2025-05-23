#ifndef PAGAMENTOVENDA_H
#define PAGAMENTOVENDA_H

#include "pagamento.h"
#include "venda.h"

class pagamentoVenda : public pagamento
{
    Q_OBJECT
public:
    explicit pagamentoVenda(QList<QList<QVariant>> listaProdutos, QString total, QString cliente, QString data, int idCliente, QWidget *parent = nullptr);
    QList<QList<QVariant>> rowDataList;

private:
    void terminarPagamento() override;
    int idCliente;


};

#endif // PAGAMENTOVENDA_H
