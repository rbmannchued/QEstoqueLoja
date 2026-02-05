#ifndef PRODUTO_SERVICE_H
#define PRODUTO_SERVICE_H

#include <QSqlDatabase>
#include <QLocale>
#include "../dto/Produto_dto.h"
#include "../util/ibptutil.h"
#include <QSqlQueryModel>
#include "../repository/Produto_repository.h"

enum class ProdutoErro {
    Nenhum,
    CampoVazio,
    CodigoBarrasExistente,
    NcmInvalido,
    NcmAviso,
    ConversaoPrecoInvalida,
    QuantidadeInvalida,
    ErroBanco,
    PrecoInvalido
};

class Produto_Service
{
public:
  struct Resultado {
    bool ok;
    ProdutoErro erro = ProdutoErro::Nenhum;
    QString msg;
  };
  explicit Produto_Service(QSqlDatabase db);

  Resultado validar(const ProdutoDTO &p);
  Resultado inserir(const ProdutoDTO &p);
  bool codigoBarrasExiste(const QString &codigo);

  double calcularPrecoFinal(double precoFornecedor, double percentualLucro);

  double calcularPercentualLucro(double precoFornecedor, double precoFinal);
  static double round2(double v);
  static float round2f(float v);
  QSqlQueryModel *listarProdutos();
  QSqlQueryModel *getProdutoPeloCodigo(const QString &codigoBarras);
  Resultado deletar(const QString &id);
  static QString normalizeText(const QString &text);
  QSqlQueryModel *pesquisar(const QString &texto);
  QSqlDatabase db;
  QLocale portugues;
  IbptUtil ibpt;
  ProdutoDTO converterDadosParaDB(const ProdutoDTO &p);
  Produto_Service::Resultado validarConversao(const ProdutoDTO &p);
  Produto_Repository *repo;
  Resultado alterar(const ProdutoDTO &p, const QString &id);
  Resultado alterarVerificarCodigoBarras(const ProdutoDTO &p, const QString &codigo, const QString &id);
};

#endif
