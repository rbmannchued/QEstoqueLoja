#ifndef CLIENTES_H
#define CLIENTES_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlQueryModel>

namespace Ui {
class Clientes;
}

class Clientes : public QWidget
{
    Q_OBJECT

public:
    explicit Clientes(QWidget *parent = nullptr);
    ~Clientes();

private:
    Ui::Clientes *ui;
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQueryModel* model = new QSqlQueryModel;
};

#endif // CLIENTES_H
