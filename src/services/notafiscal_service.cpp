#include "notafiscal_service.h"
#include <qdebug.h>
#include <QFile>
#include <QDomDocument>
#include <QDomNode>
#include <QSqlError>
#include <QDomElement>

NotaFiscal_service::NotaFiscal_service(QObject *parent)
    : QObject{parent}
{}

NotaFiscal_service::Resultado NotaFiscal_service::salvarResNfe(NotaFiscalDTO resumoNota){
    if(notaRepo.salvarResNFe(resumoNota)){
        qDebug() << "Resumo nota salvo com sucesso!";
        return {true, NotaErro::Nenhum, ""};
    }else{
        return {false, NotaErro::Salvar, "Erro ao salvar resumo NFe."};
    }

}

qlonglong NotaFiscal_service::getIdFromChave(QString chnfe){
    return notaRepo.getIdFromChave(chnfe);
}

NotaFiscal_service::Resultado NotaFiscal_service::updateWhereChave(NotaFiscalDTO dto,
                                                                   QString chave){
    if(notaRepo.updateWhereChave(dto,chave)){
        return {true, NotaErro::Nenhum, ""};
    }else{
        return {false, NotaErro::Update, "Erro ao atualizar Nota Fiscal"};
    }
}

NotaFiscalDTO NotaFiscal_service::lerNotaFiscalDoXML(const QString &xmlPath)
{
    NotaFiscalDTO nf;
    nf.xmlPath = xmlPath;

    QFile file(xmlPath);

    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Erro ao abrir XML:" << xmlPath;
        return nf;
    }

    QDomDocument doc;
    if (!doc.setContent(&file)) {
        qWarning() << "Erro ao carregar XML:" << xmlPath;
        file.close();
        return nf;
    }
    file.close();

    auto getTag = [&](const QDomElement &parent, const QString &tag) {
        QDomNode n = parent.elementsByTagName(tag).item(0);
        return n.isNull() ? QString() : n.toElement().text().trimmed();
    };

    // Pegando nó nfeProc ou NFe
    QDomNodeList listaInfNFe = doc.elementsByTagName("infNFe");
    if (listaInfNFe.isEmpty()) {
        qWarning() << "XML sem <infNFe>";
        return nf;
    }

    QDomElement infNFe = listaInfNFe.at(0).toElement();

    // CHAVE DA NFE (atributo Id removendo "NFe")
    QString id = infNFe.attribute("Id");
    if (id.startsWith("NFe"))
        nf.chNfe = id.mid(3);

    // --- TAGS DIRETAS DO INFNFE ---
    QDomNodeList ideList = infNFe.elementsByTagName("ide");
    if (!ideList.isEmpty()) {
        QDomElement ide = ideList.at(0).toElement();
        nf.cuf   = getTag(ide, "cUF");
        nf.nnf   = getTag(ide, "nNF").toLongLong();
        nf.serie = getTag(ide, "serie").toInt();
        nf.modelo = getTag(ide, "mod");
        nf.tpAmb = (getTag(ide, "tpAmb") == "1");  // 1 = produção
    }

    // --- VALOR TOTAL DA NOTA ---
    QDomNodeList totalList = infNFe.elementsByTagName("ICMSTot");
    if (!totalList.isEmpty()) {
        QDomElement tot = totalList.at(0).toElement();
        nf.valorTotal = getTag(tot, "vNF").toDouble();
    }

    // --- EMITENTE ---
    QDomNodeList emitList = infNFe.elementsByTagName("emit");
    if (!emitList.isEmpty()) {
        QDomElement emite = emitList.at(0).toElement();
        nf.cnpjEmit = getTag(emite, "CNPJ");
    }

    // --- <protNFe> (protocolo) ---
    QDomNodeList protList = doc.elementsByTagName("protNFe");
    if (!protList.isEmpty()) {
        QDomElement prot = protList.at(0).toElement();
        QDomElement infProt = prot.elementsByTagName("infProt").item(0).toElement();

        nf.cstat = getTag(infProt, "cStat");
        nf.nProt = getTag(infProt, "nProt");
    }

    return nf;
}
