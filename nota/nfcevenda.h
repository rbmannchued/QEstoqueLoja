#ifndef NFCEVENDA_H
#define NFCEVENDA_H

#include <QObject>
#include <QString>
#include <QDebug>
#include <CppBrasil/CppUtil>
#include <CppBrasil/NFe/CppNFe>
#include <CppBrasil/NFe/ConvNF>
#include <QLocale>
#include <QSqlDatabase>

class NfceVenda : public QObject
{
    Q_OBJECT
public:
    explicit NfceVenda(QObject *parent = nullptr);
    ~NfceVenda();
    const CppNFe *getCppNFe();

    void setProdutosVendidos(QList<QList<QVariant>> produtosVendidos);
    void setPagamentoValores(QString formaPag, float desconto, float recebido, float troco, float taxa);
    void setCliente(QString cpf, bool ehPf);
    int getNNF();
    int getSerie();
    QString getXmlPath();
    int getProximoNNF();
    float getVNF();
private:
    CppNFe *m_nfe;
    QMap<QString, QString> fiscalValues;
    QMap<QString, QString> empresaValues;
    QString caminhoXml = QCoreApplication::applicationDirPath() + "/xmlNf";
    QList<QList<QVariant>>listaProdutos;
    int quantProds = 0;
    QVector<float> vTotTribProduto;
    QVector<float> descontoProd;
    double descontoNf,trocoNf,vPagNf = 0;
    QString tPagNf = "01";
    QString indPagNf = "0";
    double vNf = 0.0;
    QLocale portugues;
    int nNf = 1;
    int serieNf = 1;
    float taxaPercentual = 0.0;
    QString nomeCliente, cpfCliente;
    bool ehPfCliente = false;
    QSqlDatabase db = QSqlDatabase::database();



    void configurar();
    void nfe();
    void infNFe(); //A01 -  Grupo A. Dados da Nota Fiscal eletrônica
    void ide(); //Grupo B. Identificação da Nota Fiscal eletrônica
    void emite(); //Grupo C. Identificação do Emitente da Nota Fiscal eletrônica
    //void avulsa(); Grupo D. - exclusivo do fisco
    void dest(); //Grupo E. Identificação do Destinatário da Nota Fiscal eletrônica
    void retirada(); //Grupo F. Identificação do Local de Retirada
    void entrega(); //Grupo G. Identificação do Local de Entrega
    void autXML(); //Grupo GA. Autorização para obter XML
    void det(int quantProdutos); //Grupo H. Detalhamento de Produtos e Serviços da NF-e
    void det_produto(const int &i); //dentro de det - Grupo I. Produtos e Serviços da NF-e
    void det_imposto(const int &i); //dentro de det - Grupo M. Tributos incidentes no Produto ou Serviço
    void det_impostoDevol(); //dentro de det - Grupo UA. Tributos Devolvidos (para o item da NF-e)
    void det_obsItem(); //dentro de det - Grupo VA. Observações de uso livre (para o item da NF-e)
    void total(); //Grupo W. Total da NF-e
    void transp(); //Grupo X. Informações do Transporte da NF-e
    void transp_transporta(); //dentro de transp - Grupo Transportador
    void transp_retTransp(); //dentro de transp - Grupo Retenção ICMS transporte
    void transp_veicTransp(); //dentro de transp - Grupo Veículo Transporte
    void transp_reboque(); //dentro de transp - Grupo Reboque
    void transp_vol(); //dentro de transp - Grupo Volumes
    void cobr(); //Grupo Y. Dados da Cobrança
    void pag(); //Grupo YA. Informações de Pagamento
    void infIntermed(); //Grupo YB. Informações do Intermediador da Transação
    void infAdic(); //Grupo Z. Informações Adicionais da NF-e
    void exporta(); //Grupo ZA. Informações de Comércio Exterior
    void compra(); //Grupo ZB. Informações de Compras
    void cana(); //Grupo ZC. Informações do Registro de Aquisição de Cana
    void infRespTec();//Grupo ZD. Informações do Responsável Técnico

    void aplicarAcrescimoProporcional(float taxaPercentual);
    void aplicarDescontoTotal(float descontoTotal);
    void aplicarDescontoProduto(float descontoTotal);
    float corrigirDescontoParaAplicacaoPosTaxa(float descontoDesejado, float taxaPercentual);
    float corrigirTaxa(float taxaAntiga, float desconto);
    bool isValidGTIN(const QString &gtin);
public slots:
    void onReqStatusServico();
    void onReqGerarEnviar();


private slots:
    void onWSChange(const WebServicesNF &webServicesNF);

signals:
    void errorOccurred(const QString &error);
    void retWSChange(const QString &webServices);
    void retStatusServico(const QString &status);
    void retLote(const QString &lote);
    void retXML(const QString &xml);


};

#endif // NFCEVENDA_H
