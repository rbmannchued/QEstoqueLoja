#ifndef ACBR_SERVICE_H
#define ACBR_SERVICE_H

#include <QObject>
#include <QMap>

enum class AcbrErro {
    Nenhum,
    CampoVazio,
    ConfiguracaoInvalida
};

class Acbr_service : public QObject
{
    Q_OBJECT

public:
    struct Resultado {
        bool ok;
        AcbrErro erro = AcbrErro::Nenhum;
        QString msg;
    };
    explicit Acbr_service(QObject *parent = nullptr);
    Acbr_service::Resultado configurar(const QString &versaoApp);
private:
    QMap<QString, QString> financeiroValues;
    QMap<QString, QString> fiscalValues;
    QMap<QString, QString> empresaValues;
    QMap<QString, QString> emailValues;

signals:
};

#endif // ACBR_SERVICE_H
