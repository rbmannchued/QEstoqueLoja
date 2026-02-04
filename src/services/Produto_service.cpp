#include "Produto_service.h"
#include <QSqlQuery>
#include <cmath>

Produto_Service::Produto_Service(QSqlDatabase db)
    : db(db),
    portugues(QLocale(QLocale::Portuguese, QLocale::Brazil)),
    repo(new Produto_Repository(db))
{}
Produto_Service::Resultado Produto_Service::validar(const ProdutoDTO &p)
{
    if (p.descricao.trimmed().isEmpty()) {
        return {false, ProdutoErro::CampoVazio, "O campo 'Descrição' não pode estar vazio."};
    }

    if (p.percentLucro <= 0) {
        return {false, ProdutoErro::CampoVazio, "O campo 'Percentual de Lucro' está incorreto."};
    }

    if (p.nf && (p.ncm.trimmed().isEmpty() || p.ncm.length() != 8)) {
        return {false, ProdutoErro::NcmInvalido, "NCM inválido para NF."};
    }

    return {true, ProdutoErro::Nenhum, ""};
}

bool Produto_Service::codigoBarrasExiste(const QString &codigo)
{
    return repo->codigoBarrasExiste(codigo);
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
        return {false, ProdutoErro::PrecoInvalido,
                "Por favor, insira um preço válido."};
    }

    if (p.quantidade <= 0) {
        return {false, ProdutoErro::QuantidadeInvalida,
                "Por favor, insira uma quantidade válida."};
    }

    // arredondamento
    double preco = round2(p.preco);
    float quant  = round2(p.quantidade);

    qDebug() << "[validarConversao]";
    qDebug() << "preco in :" << p.preco << " -> out:" << preco;
    qDebug() << "quant in :" << p.quantidade << " -> out:" << quant;

    return {true, ProdutoErro::Nenhum, ""};
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

    if (repo->codigoBarrasExiste(p.codigoBarras)) {
        return {false, ProdutoErro::CodigoBarrasExistente,
                "Código de barras já existe."};
    }

    ProdutoDTO dbData = converterDadosParaDB(p);

    QString erroSQL;
    if (!repo->inserir(dbData, erroSQL)) {
        return {false, ProdutoErro::ErroBanco, erroSQL};
    }

    return {true, ProdutoErro::Nenhum, ""};
}

Produto_Service::Resultado Produto_Service::deletar(const QString &id){
    QString errosql = "";

    if(!repo->deletar(id, errosql)){
        return {false, ProdutoErro::ErroBanco, errosql};
    }else{
        return {true, ProdutoErro::Nenhum, ""};
    }

}

QSqlQueryModel* Produto_Service::listarProdutos()
{
    return repo->listarProdutos();
}

QSqlQueryModel* Produto_Service::getProdutoPeloCodigo(const QString &codigoBarras){
    return repo->getProdutoPeloCodigo(codigoBarras);
}
