#include <QtTest>
#include "test_produto_service.h"
#include "services/Produto_service.h"
#include "../db/test_db_factory.h"
#include <QLocale>
#include <QSqlQuery>

void TestProdutoService::inserir_produto_ok()
{
    db = TestDbFactory::create();
    Produto_Service service(db);

    ProdutoDTO p;
    p.quantidade = 10.00;
    p.descricao = "Produto Teste";
    p.preco = 5.50;
    p.codigoBarras = "7898765430018";
    p.nf = 0;
    p.uCom = "UN";
    p.precoFornecedor = 4.00;
    p.percentLucro = 20.00;
    p.ncm = "00000000";
    p.cest = "";
    p.aliquotaIcms = 32.50;
    p.csosn = "102";
    p.pis = "49";

    auto r = service.inserir(p);

    QVERIFY(r.ok == true);


    QSqlQuery query(db);
    query.exec("SELECT * FROM produtos ORDER BY id DESC LIMIT 1");
    QString desc, codbarras, nf, ucom, ncm, cest, csosn, pis;
    double preco, precoforn, quant, aliquota, percentlucro;

    while(query.next()){
        quant = query.value("quantidade").toFloat();
        desc = query.value("descricao").toString();
        preco = query.value("preco").toDouble();
        codbarras = query.value("codigo_barras").toString();
        nf = query.value("nf").toString();
        ucom = query.value("un_comercial").toString();
        precoforn = query.value("preco_fornecedor").toDouble();
        percentlucro = query.value("porcent_lucro").toFloat();
        ncm = query.value("ncm").toString();
        cest = query.value("cest").toString();
        aliquota = query.value("aliquota_imposto").toFloat();
        csosn = query.value("csosn").toString();
        pis = query.value("pis").toString();
    }

    QCOMPARE(quant, 10);
    QCOMPARE(desc, "Produto Teste");
    QCOMPARE(preco, 5.5);
    QCOMPARE(codbarras, "7898765430018");
    QCOMPARE(nf, "0");
    QCOMPARE(ucom, "UN");
    QCOMPARE(precoforn, 4);
    QCOMPARE(percentlucro, 20);
    QCOMPARE(ncm, "00000000");
    QCOMPARE(cest, "");
    QCOMPARE(aliquota, 32.5);
    QCOMPARE(csosn, "102");
    QCOMPARE(pis, "49");

}


void TestProdutoService::inserir_produto_errado()
{
    db = TestDbFactory::create();
    Produto_Service service(db);

    ProdutoDTO p;
    p.quantidade = 0;
    p.descricao = "";
    p.preco = -5.50;
    p.codigoBarras = "";
    p.nf = 0;
    p.uCom = "UN";
    p.precoFornecedor = -4.00;
    p.percentLucro = -20.00;
    p.ncm = "12313";
    p.cest = "";
    p.aliquotaIcms = -2.9;
    p.csosn = "102";
    p.pis = "49";

    auto r = service.inserir(p);

    QVERIFY(!r.ok);

    QSqlQuery query(db);
    QVERIFY(query.exec("SELECT COUNT(*) FROM produtos"));
    QVERIFY(query.next());

    int count = query.value(0).toInt();

    QCOMPARE(count, 0);
}


void TestProdutoService::erro_codigo_barras_existente()
{
    db = TestDbFactory::create();
    Produto_Service service(db);

    ProdutoDTO p;
    p.quantidade = 1;
    p.descricao = "Produto 1";
    p.preco = 10;
    p.codigoBarras = "ABC";
    p.nf = false;
    p.uCom = "UN";
    p.precoFornecedor = 5;
    p.percentLucro = 10;
    p.ncm = "";
    p.cest = "";
    p.aliquotaIcms = 18;
    p.csosn = "102";
    p.pis = "1";

    service.inserir(p);
    auto r2 = service.inserir(p);

    QVERIFY(!r2.ok);
    QCOMPARE(r2.erro, InserirProdutoErro::CodigoBarrasExistente);
}

void TestProdutoService::erro_preco_invalido()
{
    db = TestDbFactory::create();
    Produto_Service service(db);

    ProdutoDTO p;
    p.quantidade = 1;
    p.descricao = "Produto";
    p.preco = -2.909090;
    p.codigoBarras = "111";
    p.nf = false;
    p.uCom = "UN";
    p.precoFornecedor = 5;
    p.percentLucro = 10;
    p.ncm = "";
    p.cest = "";
    p.aliquotaIcms = 18;
    p.csosn = "102";
    p.pis = "1";

    auto r = service.inserir(p);

    QVERIFY(!r.ok);
    QCOMPARE(r.erro, InserirProdutoErro::PrecoInvalido);
}

void TestProdutoService::erro_quantidade_invalida()
{
    db = TestDbFactory::create();
    Produto_Service service(db);

    ProdutoDTO p;
    p.quantidade = 0;
    p.descricao = "Produto";
    p.preco = 10;
    p.codigoBarras = "222";
    p.nf = false;
    p.uCom = "UN";
    p.precoFornecedor = 5;
    p.percentLucro = 10;
    p.ncm = "";
    p.cest = "";
    p.aliquotaIcms = 18;
    p.csosn = "102";
    p.pis = "1";

    auto r = service.inserir(p);

    QVERIFY(!r.ok);
    QCOMPARE(r.erro, InserirProdutoErro::QuantidadeInvalida);
}

void TestProdutoService::erro_ncm_nf()
{
    db = TestDbFactory::create();
    Produto_Service service(db);

    ProdutoDTO p;
    p.quantidade = 1;
    p.descricao = "Produto";
    p.preco = 10;
    p.codigoBarras = "333";
    p.nf = true;
    p.uCom = "UN";
    p.precoFornecedor = 5;
    p.percentLucro = 10;
    p.ncm = ""; // inválido para NF
    p.cest = "";
    p.aliquotaIcms = 18;
    p.csosn = "102";
    p.pis = "1";

    auto r = service.inserir(p);

    QVERIFY(!r.ok);
    QCOMPARE(r.erro, InserirProdutoErro::NcmInvalido);
}

void TestProdutoService::cleanup()
{
    QSqlQuery q(db);
    q.exec("DELETE FROM produtos");
}


