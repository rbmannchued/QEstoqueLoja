#ifndef CLIENTE_SERVICE_H
#define CLIENTE_SERVICE_H

#include <QObject>
#include "../repository/cliente_repository.h"

enum class ClienteErro {
    Nenhum,
    CampoVazio,
    Banco,
    InsercaoInvalida
};

class Cliente_service : public QObject
{
    Q_OBJECT
public:
    struct Resultado {
        bool ok;
        ClienteErro erro = ClienteErro::Nenhum;
        QString msg;
    };
    explicit Cliente_service(QObject *parent = nullptr);
    qlonglong contarQuantosRegistrosPorCPFCNPJ(const QString &cpfcnpj);
    Cliente_service::Resultado inserirClienteEmitente(ClienteDTO emissor);
    qlonglong getIdFromCpfCnpj(const QString &cpfcnpj);
private:
    Cliente_repository cliRepo;

signals:
};

#endif // CLIENTE_SERVICE_H
