#include "acbr_service.h"
#include "../configuracao.h"
#include "../util/mailmanager.h"
#include "../nota/acbrmanager.h"
#include "../util/consultacnpjmanager.h"
#include <qdir.h>
#include <qstandardpaths.h>


Acbr_service::Acbr_service(QObject *parent)
    : QObject{parent}
{
    // fiscalValues  = Configuracao::get_All_Fiscal_Values();
    // empresaValues = Configuracao::get_All_Empresa_Values();
    // emailValues   = Configuracao::get_All_Email_Values();

}

Acbr_service::Resultado Acbr_service::configurar(const QString &versaoApp)
{
    auto acbr = AcbrManager::instance()->nfe();
    auto cnpj = ConsultaCnpjManager::instance()->cnpj();
    auto mail = MailManager::instance().mail();

    Config_service *confServ = new Config_service(this);
    configDTO = confServ->carregarTudo();

    if(configDTO.tpAmbFiscal != true){
        return{false, AcbrErro::NaoEmitindoNf, "Não atualizou pois emitir NF não está marcado"};
    }


    QString caminhoXml = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) +
                         "/xmlNf";
    QString caminhoEntradas = caminhoXml + "/entradas";
    QDir dir;
    if(!dir.exists(caminhoXml)){
        dir.mkpath(caminhoXml);
    }
    if(!dir.exists(caminhoEntradas)){
        dir.mkpath(caminhoEntradas);
    }

    // LIMPAR strings antes de usar
    auto cleanStr = [](const QString& str) -> std::string {
        std::string result = str.toStdString();
        result.erase(std::remove(result.begin(), result.end(), '\0'), result.end());
        return result.empty() ? "" : result;
    };
    QString ufEstado = configDTO.estadoEmpresa;
    std::string certificadoPath = cleanStr(configDTO.certificadoPathFiscal);
    std::string certificadoSenha = cleanStr(configDTO.senhaCertificadoFiscal);
    std::string uf = cleanStr(ufEstado);
    std::string schemaPath = cleanStr(configDTO.schemaPathFiscal);
    std::string idCsc = cleanStr(configDTO.idCscFiscal);
    std::string csc = cleanStr(configDTO.cscFiscal);
    std::string tpAmb = (configDTO.tpAmbFiscal == 0 ? "1" : "0");
    QString caminhoCompletoLogo = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) +
                                  "/imagens/" + QFileInfo(configDTO.logoPathEmpresa).fileName();


    if(certificadoPath == "")
        return {false, AcbrErro::CampoVazio, "Caminho do certificado não configurado"};

    if(schemaPath == "")
        return {false, AcbrErro::CampoVazio, "Schema não configurado"};

    if(ufEstado.size() != 2)
        return {false, AcbrErro::ConfiguracaoInvalida, "UF inválida"};
    //     // CONFIGURAÇÕES PRINCIPAIS - DFe
    acbr->ConfigGravarValor("DFe", "ArquivoPFX", certificadoPath);
    acbr->ConfigGravarValor("DFe", "Senha", certificadoSenha);
    acbr->ConfigGravarValor("DFe", "UF", uf);
    acbr->ConfigGravarValor("DFe", "SSLHttpLib", "3");
    acbr->ConfigGravarValor("DFe", "SSLCryptLib", "1");
    acbr->ConfigGravarValor("DFe", "SSLXmlSignLib", "4");

    // //     // CONFIGURAÇÕES NFe
    acbr->ConfigGravarValor("NFe", "PathSchemas", schemaPath);
    acbr->ConfigGravarValor("NFe", "IdCSC", idCsc);
    acbr->ConfigGravarValor("NFe", "CSC", csc);
    acbr->ConfigGravarValor("NFe", "ModeloDF", "1");  // NFCe
    acbr->ConfigGravarValor("NFe", "VersaoDF", "3");
    acbr->ConfigGravarValor("NFe", "VersaoQRCode", "3");
    acbr->ConfigGravarValor("NFe", "FormaEmissao", "0");
    acbr->ConfigGravarValor("NFe", "Ambiente", tpAmb);
    acbr->ConfigGravarValor("NFE", "Download.PathDownload", caminhoEntradas.toStdString());
    //separar xml em pastas por nome da empresa
    acbr->ConfigGravarValor("NFe", "Download.SepararPorNome", "1");
    // // CONFIGURAÇÕES DE ARQUIVO

    acbr->ConfigGravarValor("NFe", "PathSalvar", caminhoXml.toStdString());
    acbr->ConfigGravarValor("NFe", "AdicionarLiteral", "1");
    acbr->ConfigGravarValor("NFe", "SepararPorCNPJ", "1");
    acbr->ConfigGravarValor("NFe", "SepararPorModelo", "1");
    acbr->ConfigGravarValor("NFe", "SepararPorAno", "1");
    acbr->ConfigGravarValor("NFe", "SepararPorMes", "1");
    acbr->ConfigGravarValor("NFe", "SalvarApenasNFeProcessadas", "1");
    acbr->ConfigGravarValor("NFe", "PathNFe", caminhoXml.toStdString());
    acbr->ConfigGravarValor("NFe", "SalvarEvento", "1");
    acbr->ConfigGravarValor("NFe", "PathEvento", caminhoXml.toStdString());

    acbr->ConfigGravarValor("NFe", "SalvarGer", "0");



    //sistema
    acbr->ConfigGravarValor("Sistema", "Nome", "QEstoqueLoja");
    acbr->ConfigGravarValor("Sistema", "Versao", versaoApp.toStdString());

    //     // CONFIGURAÇÕES DE CONEXÃO
    //     // nfce->ConfigGravarValor("NFe", "Timeout", "30000");
    //     // nfce->ConfigGravarValor("NFe", "Tentativas", "5");
    //     // nfce->ConfigGravarValor("NFe", "IntervaloTentativas", "1000");

    // // CONFIGURAÇÕES DANFE NFCe
    acbr->ConfigGravarValor("DANFE", "PathLogo", caminhoCompletoLogo.toStdString());

    //     // nfce->ConfigGravarValor("DANFENFCe", "TipoRelatorioBobina", "0");
    //     // nfce->ConfigGravarValor("DANFENFCe", "ImprimeItens", "1");
    //     // nfce->ConfigGravarValor("DANFENFCe", "ViaConsumidor", "1");
    //     // nfce->ConfigGravarValor("DANFENFCe", "FormatarNumeroDocumento", "1");
    acbr->ConfigGravar("");

    cnpj->ConfigGravarValor("ConsultaCNPJ", "Provedor", "3");
    cnpj->ConfigGravar("");

    //acbrMail
    mail->ConfigGravarValor("Email", "Nome", configDTO.nomeEmail.toStdString());
    mail->ConfigGravarValor("Email", "Servidor", configDTO.smtpEmail.toStdString());
    mail->ConfigGravarValor("Email", "Conta", configDTO.contaEmail.toStdString());
    mail->ConfigGravarValor("Email", "Usuario", configDTO.usuarioEmail.toStdString());
    mail->ConfigGravarValor("Email", "Senha", configDTO.senhaEmail.toStdString());
    mail->ConfigGravarValor("Email", "Porta", configDTO.portaEmail.toStdString());
    mail->ConfigGravarValor("Email", "SSL", QVariant(configDTO.sslEmail).toString().toStdString());
    mail->ConfigGravarValor("Email", "TLS", QVariant(configDTO.tlsEmail).toString().toStdString());
    mail->ConfigGravar("");

    return {true, AcbrErro::Nenhum, ""};
}
