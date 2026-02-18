#ifndef CLIENTE_REPOSITORY_H
#define CLIENTE_REPOSITORY_H

#include <QObject>
#include <QSqlDatabase>
#include "../dto/Cliente_dto.h"

class Cliente_repository : public QObject
{
    Q_OBJECT
public:
    explicit Cliente_repository(QObject *parent = nullptr);
    qlonglong contarQuantosRegistrosPorCPFCNPJ(const QString &cpfcnpj);
    bool inserir(ClienteDTO cliente);
    qlonglong getIdFromCPFCNPJ(const QString &cpfcnpj);
private:
    QSqlDatabase db;

signals:
};

#endif // CLIENTE_REPOSITORY_H
