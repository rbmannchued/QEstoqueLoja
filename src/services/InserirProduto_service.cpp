#include "InserirProduto_service.h"
#include <QSqlQuery>

InserirProduto_Service::InserirProduto_Service(QSqlDatabase db)
    : db(db),
    portugues(QLocale(QLocale::Portuguese, QLocale::Brazil))
{
}

InserirProduto_Service::Resultado InserirProduto_Service::validar(const ProdutoDTO &p)
{
    if (p.descricao.trimmed().isEmpty()) {
        return {false, InserirProdutoErro::CampoVazio, "O campo 'Descrição' não pode estar vazio."};
    }

    if (p.percentLucro.trimmed().isEmpty()) {
        return {false, InserirProdutoErro::CampoVazio, "O campo 'Percentual de Lucro' não pode estar vazio."};
    }

    if (p.nf && (p.ncm.trimmed().isEmpty() || p.ncm.length() != 8)) {
        return {false, InserirProdutoErro::NcmInvalido, "NCM inválido para NF."};
    }

    if((p.ncm.trimmed().isEmpty() && p.nf) || (p.ncm.length() != 8 && p.nf)){
        return {false, InserirProdutoErro::NcmAviso, "O campo NCM está errado"};
    }

    return {true, InserirProdutoErro::Nenhum, ""};
}

bool InserirProduto_Service::codigoBarrasExiste(const QString &codigo)
{
    // if (!db.isOpen()){
    //     if(!db.open()){
    //         return false;
    //     }
    // }

    QSqlQuery query(db);
    query.prepare("SELECT COUNT(*) FROM produtos WHERE codigo_barras = :codigo");
    query.bindValue(":codigo", codigo);

    if (!query.exec()) return false;

    query.next();
    return query.value(0).toInt() > 0;
}
ProdutoDTO InserirProduto_Service::converterDadosParaDB(const ProdutoDTO &p)
{
    ProdutoDTO out = p; // cópia

    bool okPreco = false;
    bool okQuant = false;
    bool okLucro = false;
    bool okAliquota = false;

    // Preço final
    double preco = portugues.toDouble(p.preco, &okPreco);
    if (okPreco)
        out.preco = QString::number(preco, 'f', 2);

    // Quantidade (sempre com ponto)
    double quant = portugues.toDouble(p.quantidade, &okQuant);
    if (okQuant)
        out.quantidade = QString::number(quant, 'f', 3); // ou 2 casas se preferir

    // Preço fornecedor
    if (p.precoFornecedor.trimmed().isEmpty()) {
        out.precoFornecedor = "";
    } else {
        double pf = portugues.toDouble(p.precoFornecedor);
        out.precoFornecedor = QString::number(pf, 'f', 2);
    }

    // Percentual lucro
    double lucro = portugues.toDouble(p.percentLucro, &okLucro);
    if (okLucro)
        out.percentLucro = QString::number(lucro, 'f', 2);

    // Alíquota ICMS
    double aliq = portugues.toDouble(p.aliquotaIcms, &okAliquota);
    if (okAliquota)
        out.aliquotaIcms = QString::number(aliq, 'f', 2);

    return out;
}

InserirProduto_Service::Resultado InserirProduto_Service::validarConversao(const ProdutoDTO &p)
{
    bool okPreco = false;
    bool okQuant = false;

    double preco = portugues.toDouble(p.preco, &okPreco);
    double quant = portugues.toDouble(p.quantidade, &okQuant);

    if (!okPreco || preco < 0) {
        return {false, InserirProdutoErro::PrecoInvalido,
                "Por favor, insira um preço válido."};
    }

    if (!okQuant || quant <= 0) {
        return {false, InserirProdutoErro::QuantidadeInvalida,
                "Por favor, insira uma quantidade válida."};
    }
    qDebug() << "[validarConversao]";
    qDebug() << "preco raw:" << p.preco;
    qDebug() << "quant raw:" << p.quantidade;
    qDebug() << "preco conv:" << preco << " ok:" << okPreco;
    qDebug() << "quant conv:" << quant << " ok:" << okQuant;

    return {true, InserirProdutoErro::Nenhum, ""};
}

double InserirProduto_Service::calcularPrecoFinal(double precoFornecedor, double percentualLucro)
{
    return precoFornecedor * (1.0 + percentualLucro / 100.0);
}


double InserirProduto_Service::calcularPercentualLucro(double precoFornecedor, double precoFinal)
{
    return precoFinal - precoFornecedor / precoFornecedor * 100.0;
}

InserirProduto_Service::Resultado InserirProduto_Service::inserir(const ProdutoDTO &p)
{
    Resultado v1 = validar(p);
    if (!v1.ok) return v1;

    Resultado v2 = validarConversao(p);
    if (!v2.ok) return v2;

    if (codigoBarrasExiste(p.codigoBarras)) {
        return {false, InserirProdutoErro::CodigoBarrasExistente,
                "Código de barras já existe."};
    }

    ProdutoDTO dbData = converterDadosParaDB(p);

    if (!db.open())
        return {false, InserirProdutoErro::ErroBanco,
                "Erro ao abrir banco de dados."};

    QSqlQuery query(db);
    query.prepare(
        "INSERT INTO produtos "
        "(quantidade, descricao, preco, codigo_barras, nf, un_comercial, "
        "preco_fornecedor, porcent_lucro, ncm, cest, aliquota_imposto, csosn, pis) "
        "VALUES "
        "(:quantidade, :descricao, :preco, :codigo_barras, :nf, :un_comercial, "
        ":preco_fornecedor, :porcent_lucro, :ncm, :cest, :aliquota_imposto, :csosn, :pis)"
    );

    query.bindValue(":quantidade", dbData.quantidade);
    query.bindValue(":descricao", dbData.descricao);
    query.bindValue(":preco", dbData.preco);
    query.bindValue(":codigo_barras", dbData.codigoBarras);
    query.bindValue(":nf", dbData.nf);
    query.bindValue(":un_comercial", dbData.uCom);
    query.bindValue(":preco_fornecedor", dbData.precoFornecedor);
    query.bindValue(":porcent_lucro", dbData.percentLucro);
    query.bindValue(":ncm", dbData.ncm);
    query.bindValue(":cest", dbData.cest);
    query.bindValue(":aliquota_imposto", dbData.aliquotaIcms);
    query.bindValue(":csosn", dbData.csosn);
    query.bindValue(":pis", dbData.pis);

    if (!query.exec()) {
        qDebug() << "[SQL ERROR]" << query.lastError().text();
        qDebug() << "[SQL QUERY]" << query.lastQuery();
        return {false, InserirProdutoErro::ErroBanco,
                query.lastError().text()};
    }

    return {true, InserirProdutoErro::Nenhum, ""};
}
