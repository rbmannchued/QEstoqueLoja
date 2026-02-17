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
        nf.dhEmi = getTag(ide, "dhEmi");
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


QList<ProdutoNotaDTO> NotaFiscal_service::carregarProdutosDaNFe(const QString &xml_path, qlonglong id_nf)
{
    QList<ProdutoNotaDTO> lista;

    QFile file(xml_path);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Erro ao abrir arquivo XML:" << xml_path;
        return lista;
    }

    QDomDocument doc;
    if (!doc.setContent(&file)) {
        qDebug() << "Erro ao ler conteúdo do XML";
        file.close();
        return lista;
    }
    file.close();

    QDomNodeList itens = doc.elementsByTagName("det");

    for (int i = 0; i < itens.count(); i++) {
        QDomElement det = itens.at(i).toElement();

        ProdutoNotaDTO p;
        p.idNf = id_nf;

        p.nitem = det.attribute("nItem").toInt();

        QDomElement prod = det.firstChildElement("prod");

        p.descricao          = prod.firstChildElement("xProd").text();
        p.codigoBarras    = prod.firstChildElement("cEAN").text();
        p.uCom  = prod.firstChildElement("uCom").text();
        p.ncm           = prod.firstChildElement("NCM").text();
        p.cfop          = prod.firstChildElement("CFOP").text();
        p.quantidade         = prod.firstChildElement("qCom").text().replace(",", ".").toDouble();
        p.preco         = prod.firstChildElement("vUnCom").text().replace(",", ".").toDouble();

        // ======= IMPOSTOS =======
        QDomElement imposto = det.firstChildElement("imposto");

        // -------------------- CSOSN ou CST ICMS --------------------
        p.csosn = "";
        p.cstIcms = "";
        p.temSt = false;

        QDomElement icms = imposto.firstChildElement("ICMS");

        QStringList gruposICMS = {
            "ICMS00","ICMS10","ICMS20","ICMS30","ICMS40","ICMS51","ICMS60","ICMS70","ICMS90",
            "ICMSSN101","ICMSSN102","ICMSSN201","ICMSSN202","ICMSSN500","ICMSSN900"
        };

        for (const QString &g : gruposICMS) {
            QDomElement e = icms.firstChildElement(g);
            if (!e.isNull()) {

                // pega CST ou CSOSN
                QString CST = e.firstChildElement("CST").text();
                QString CSOSN = e.firstChildElement("CSOSN").text();

                if (!CSOSN.isEmpty())
                    p.csosn = CSOSN;

                if (!CST.isEmpty())
                    p.cstIcms = CST;

                // identifica ST
                if (g == "ICMS10" || g == "ICMS30" || g == "ICMS60" ||
                    g == "ICMS70" || g == "ICMSSN201" || g == "ICMSSN202") {
                    p.temSt = true;
                }

                // valida ST por valores
                if (!e.firstChildElement("vBCST").isNull() ||
                    !e.firstChildElement("vICMSST").isNull() ||
                    !e.firstChildElement("vBCSTRet").isNull() ||
                    !e.firstChildElement("vICMSSTRet").isNull())
                {
                    p.temSt = true;
                }

                break;
            }
        }

        // -------------------- PIS --------------------
        QDomNodeList listaPIS = imposto.elementsByTagName("PISOutr");
        if (listaPIS.isEmpty())
            listaPIS = imposto.elementsByTagName("PISNT");

        if (!listaPIS.isEmpty())
            p.pis = listaPIS.at(0).firstChildElement("CST").text();
        else
            p.pis = "";

        QString aliquota = listaPIS.at(0).firstChildElement("pPIS").text();
        p.aliquotaIcms = aliquota.replace(",", ".").toDouble();

        lista.append(p);
    }

    return lista;
}

