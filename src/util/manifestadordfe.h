#ifndef MANIFESTADORDFE_H
#define MANIFESTADORDFE_H

#include <QObject>
#include <sstream>
#include <QMap>
#include <QSqlDatabase>
#include "../nota/eventocienciaop.h"
#include "../services/config_service.h"
#include "../services/dfe_service.h"
#include "../infra/databaseconnection_service.h"
#include "../services/notafiscal_service.h"
#include "../dto/NotaFiscal_dto.h"
#include <qlocale.h>
#include "../services/produtonota_service.h"

struct ProcNfe {
    QString chave;
    QString nsu;
    QString cnpjEmit;
    QString schema;
    QString vnf;
    QString cstat;
    QString xml_path;
    QString nProt;
    QString dhEmi;
    QString nome;
    QString cSitNfe;
};


struct Emitente {
    QString cnpj;
    QString nome;
    QString xLgr;
    QString nro;
    QString xBairro;
    QString xMun;
    QString cMun;
    QString uf;
    QString cep;
    QString ie;
};

// struct ProdutoNota {
//     float quant;
//     QString desc;
//     double preco;
//     QString cod_barras;
//     QString un_comercial;
//     QString ncm;
//     QString csosn;
//     QString pis;
//     QString cfop;
//     float aliquota_imposto;
//     QString nitem;
//     qlonglong id_nf;
//     QString cst_icms;
//     bool tem_st;
// };

class ManifestadorDFe : public QObject
{
    Q_OBJECT
public:
    explicit ManifestadorDFe(QObject *parent = nullptr);
    bool enviarCienciaOperacao(const QString &chNFe, const QString &cnpjEmit);
    void consultarEManifestar();
    void consultarEBaixarXML();
    void consultaAlternada();
    void consultarSePossivel();
private:
    QSqlDatabase db;
    ConfigDTO configDTO;
    QString cuf,cnpj;
    QString ultimo_nsu;
    QString ultNsuXml;
    QString novoUltNsuXml;
    Dfe_service dfeServ;
    NotaFiscal_service nfServ;
    QLocale portugues;
    ProdutoNota_service prodNotaServ;



    void salvarEventoNoBanco(const QString &tipo, const EventoRetornoInfo &info, const QString &chaveNFe);
    void carregarConfigs();
    void processarHeaderDfe(const QString &bloco);
    void salvarNovoUltNsu(const QString &ultNsu);
    bool existeCienciaOperacao(const QString &chNFe);
    void salvarResumoNota(NotaFiscalDTO resumo);
    void processarResumo(const QString &bloco);
    void processarNota(const QString &bloco);
    bool salvarEmitenteCliente(NotaFiscalDTO notaInfo);
    Emitente lerEmitenteDoXML(const QString &xmlPath);
    bool atualizarNotaBanco(NotaFiscalDTO notaInfo);
    void processarHeaderDfeXML(const QString &bloco);
    bool salvarProdutosNota(const QString &xml_path, const QString &chnfe);
signals:
};

#endif // MANIFESTADORDFE_H
