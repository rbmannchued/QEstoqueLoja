#ifndef VENDAS_H
#define VENDAS_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <QItemSelection>

namespace Ui {
class Vendas;
}

class Vendas : public QWidget
{
    Q_OBJECT

public:
    QSqlDatabase db = QSqlDatabase::database();
    explicit Vendas(QWidget *parent = nullptr);
    ~Vendas();
    void atualizarTabelas();

private slots:
    void on_Btn_InserirVenda_clicked();
    void handleSelectionChange(const QItemSelection &selected, const QItemSelection &deselected);

private:
    QSqlQueryModel *modeloProdVendidos = new QSqlQueryModel;
    QSqlQueryModel *modeloVendas2 = new QSqlQueryModel;
    Ui::Vendas *ui;
};

#endif // VENDAS_H