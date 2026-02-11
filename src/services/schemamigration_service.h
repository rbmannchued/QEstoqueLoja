#ifndef SCHEMAMIGRATION_SERVICE_H
#define SCHEMAMIGRATION_SERVICE_H
#include <QObject>
#include <QString>


class SchemaMigration_service : public QObject
{
    Q_OBJECT

    enum class SchemaErro {
        Nenhum,
        CampoVazio,
        FalhaConexao,
        ErroSQL,
        ErroMigracao
    };
public:

    struct Resultado {
        bool ok;
        SchemaErro status;
        QString message;
        int oldVersion;
        int newVersion;
    };

    explicit SchemaMigration_service(QObject *parent = nullptr);
    Resultado init();      // cria banco + tabelas base
    Resultado migrate();   // migração incremental

signals:
};

#endif // SCHEMAMIGRATION_SERVICE_H
