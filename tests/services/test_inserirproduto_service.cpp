#include <QtTest>
#include "test_inserirproduto_service.h"
#include "services/InserirProduto_service.h"
#include "../db/test_db_factory.h"
#include <QLocale>
#include <QSqlQuery>

void TestInserirProdutoService::inserir_sucesso()
{
    db = TestDbFactory::create();
    InserirProduto_Service service(db);

    ProdutoDTO p;
    p.quantidade = "10";
    p.descricao = "Produto Teste";
    p.preco = "5,50";
    p.codigoBarras = "512gvdsa2a";
    p.nf = 0;
    p.uCom = "UN";
    p.precoFornecedor = "4,00";
    p.percentLucro = "20,00";
    p.ncm = "00000000";
    p.cest = "1";
    p.aliquotaIcms = "18";
    p.csosn = "102";
    p.pis = "1";

    auto r = service.inserir(p);

    QVERIFY(r.ok == true);
}

void TestInserirProdutoService::erro_codigo_barras_existente()
{
    db = TestDbFactory::create();
    InserirProduto_Service service(db);

    ProdutoDTO p;
    p.quantidade = "1";
    p.descricao = "Produto 1";
    p.preco = "10";
    p.codigoBarras = "ABC";
    p.nf = false;
    p.uCom = "UN";
    p.precoFornecedor = "5";
    p.percentLucro = "10";
    p.ncm = "";
    p.cest = "";
    p.aliquotaIcms = "18";
    p.csosn = "102";
    p.pis = "1";

    service.inserir(p);
    auto r2 = service.inserir(p);

    QVERIFY(!r2.ok);
    QCOMPARE(r2.erro, InserirProdutoErro::CodigoBarrasExistente);
}

void TestInserirProdutoService::erro_preco_invalido()
{
    db = TestDbFactory::create();
    InserirProduto_Service service(db);

    ProdutoDTO p;
    p.quantidade = "1";
    p.descricao = "Produto";
    p.preco = "abc";
    p.codigoBarras = "111";
    p.nf = false;
    p.uCom = "UN";
    p.precoFornecedor = "5";
    p.percentLucro = "10";
    p.ncm = "";
    p.cest = "";
    p.aliquotaIcms = "18";
    p.csosn = "102";
    p.pis = "1";

    auto r = service.inserir(p);

    QVERIFY(!r.ok);
    QCOMPARE(r.erro, InserirProdutoErro::PrecoInvalido);
}

void TestInserirProdutoService::erro_quantidade_invalida()
{
    db = TestDbFactory::create();
    InserirProduto_Service service(db);

    ProdutoDTO p;
    p.quantidade = "0";
    p.descricao = "Produto";
    p.preco = "10";
    p.codigoBarras = "222";
    p.nf = false;
    p.uCom = "UN";
    p.precoFornecedor = "5";
    p.percentLucro = "10";
    p.ncm = "";
    p.cest = "";
    p.aliquotaIcms = "18";
    p.csosn = "102";
    p.pis = "1";

    auto r = service.inserir(p);

    QVERIFY(!r.ok);
    QCOMPARE(r.erro, InserirProdutoErro::QuantidadeInvalida);
}

void TestInserirProdutoService::erro_ncm_nf()
{
    db = TestDbFactory::create();
    InserirProduto_Service service(db);

    ProdutoDTO p;
    p.quantidade = "1";
    p.descricao = "Produto";
    p.preco = "10";
    p.codigoBarras = "333";
    p.nf = true;
    p.uCom = "UN";
    p.precoFornecedor = "5";
    p.percentLucro = "10";
    p.ncm = ""; // inválido para NF
    p.cest = "";
    p.aliquotaIcms = "18";
    p.csosn = "102";
    p.pis = "1";

    auto r = service.inserir(p);

    QVERIFY(!r.ok);
    QCOMPARE(r.erro, InserirProdutoErro::NcmInvalido);
}

void TestInserirProdutoService::cleanup()
{
    QSqlQuery q;
    q.exec("DELETE FROM produtos");
}

