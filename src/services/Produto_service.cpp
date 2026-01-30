#include "Produto_service.h"
#include <QSqlQuery>
#include <cmath>

Produto_Service::Produto_Service(QSqlDatabase db)
    : db(db),
    portugues(QLocale(QLocale::Portuguese, QLocale::Brazil))
{
}

Produto_Service::Resultado Produto_Service::validar(const ProdutoDTO &p)
{
    if (p.descricao.trimmed().isEmpty()) {
        return {false, InserirProdutoErro::CampoVazio, "O campo 'Descrição' não pode estar vazio."};
    }

    if (p.percentLucro <= 0) {
        return {false, InserirProdutoErro::CampoVazio, "O campo 'Percentual de Lucro' está incorreto."};
    }

    if (p.nf && (p.ncm.trimmed().isEmpty() || p.ncm.length() != 8)) {
        return {false, InserirProdutoErro::NcmInvalido, "NCM inválido para NF."};
    }

    return {true, InserirProdutoErro::Nenhum, ""};
}

bool Produto_Service::codigoBarrasExiste(const QString &codigo)
{
    if (!db.isOpen()){
        if(!db.open()){
            return false;
        }
    }

    QSqlQuery query(db);
    query.prepare("SELECT COUNT(*) FROM produtos WHERE codigo_barras = :codigo");
    query.bindValue(":codigo", codigo);

    if (!query.exec()) return false;

    query.next();
    return query.value(0).toInt() > 0;
}
ProdutoDTO Produto_Service::converterDadosParaDB(const ProdutoDTO &p)
{
    ProdutoDTO out = p; // cópia

    // normalização numérica
    out.preco           = round2(p.preco);
    out.quantidade      = round2(p.quantidade);
    out.precoFornecedor = round2(p.precoFornecedor);
    out.percentLucro    = round2(p.percentLucro);
    out.aliquotaIcms = round2(p.aliquotaIcms);

    return out;
}

double Produto_Service::round2(double v)
{
    return std::round(v * 100.0) / 100.0;
}

float Produto_Service::round2f(float v)
{
    return std::round(v * 100.0f) / 100.0f;
}
Produto_Service::Resultado Produto_Service::validarConversao(const ProdutoDTO &p)
{
    // validações básicas
    if (p.preco < 0) {
        return {false, InserirProdutoErro::PrecoInvalido,
                "Por favor, insira um preço válido."};
    }

    if (p.quantidade <= 0) {
        return {false, InserirProdutoErro::QuantidadeInvalida,
                "Por favor, insira uma quantidade válida."};
    }

    // arredondamento
    double preco = round2(p.preco);
    float quant  = round2(p.quantidade);

    qDebug() << "[validarConversao]";
    qDebug() << "preco in :" << p.preco << " -> out:" << preco;
    qDebug() << "quant in :" << p.quantidade << " -> out:" << quant;

    return {true, InserirProdutoErro::Nenhum, ""};
}

double Produto_Service::calcularPrecoFinal(double precoFornecedor, double percentualLucro)
{
    return precoFornecedor * (1.0 + percentualLucro / 100.0);
}


double Produto_Service::calcularPercentualLucro(double precoFornecedor, double precoFinal)
{
    return ((precoFinal - precoFornecedor) / precoFornecedor) * 100.0;
}

Produto_Service::Resultado Produto_Service::inserir(const ProdutoDTO &p)
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
