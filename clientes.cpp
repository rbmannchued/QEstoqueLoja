#include "clientes.h"
#include "ui_clientes.h"
#include <QSqlQueryModel>
#include <QSqlDatabase>
#include <QMessageBox>
#include "alterarcliente.h"
#include <QSqlQuery>
#include "inserircliente.h"

Clientes::Clientes(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Clientes)
{
    ui->setupUi(this);

    model->setQuery("SELECT * FROM clientes");
    ui->Tview_Clientes->setModel(model);

    model->setHeaderData(0, Qt::Horizontal, tr("ID"));
    model->setHeaderData(1, Qt::Horizontal, tr("Nome"));
    model->setHeaderData(2, Qt::Horizontal, tr("Email"));
    model->setHeaderData(3, Qt::Horizontal, tr("Telefone"));
    model->setHeaderData(4, Qt::Horizontal, tr("Endereço"));
    model->setHeaderData(5, Qt::Horizontal, tr("CPF"));
    model->setHeaderData(6, Qt::Horizontal, tr("Data Nascimento"));
    model->setHeaderData(7, Qt::Horizontal, tr("Data Cadastro"));

    ui->Tview_Clientes->setColumnWidth(0,50);
    ui->Tview_Clientes->setColumnWidth(1,150);//nome
    ui->Tview_Clientes->setColumnWidth(2,150);
    ui->Tview_Clientes->setColumnWidth(5,150);

}

Clientes::~Clientes()
{
    delete ui;
}

void Clientes::on_Btn_Alterar_clicked()
{
    qDebug() << "Teste";
    if(ui->Tview_Clientes->selectionModel()->isSelected(ui->Tview_Clientes->currentIndex())){
        // obter id selecionado
        QItemSelectionModel *selectionModel = ui->Tview_Clientes->selectionModel();
        QModelIndex selectedIndex = selectionModel->selectedIndexes().first();
        QVariant idVariant = ui->Tview_Clientes->model()->data(ui->Tview_Clientes->model()->index(selectedIndex.row(), 0));
        QString clienteId = idVariant.toString();
        if(clienteId.toInt() > 1){
            AlterarCliente *alterarJanela = new AlterarCliente(nullptr, clienteId);
            alterarJanela->setWindowModality(Qt::ApplicationModal);
            connect(alterarJanela, &AlterarCliente::clienteAtualizado, this, &Clientes::atualizarTableview);
            alterarJanela->show();
        }else{
            QMessageBox::warning(this,"Erro","Não é possivel alterar o cliente 1!");
            return;
        }

    }
    else{
        QMessageBox::warning(this,"Erro","Selecione um cliente antes de alterar!");
    }
}


void Clientes::on_Btn_Deletar_clicked()
{
    if(ui->Tview_Clientes->selectionModel()->isSelected(ui->Tview_Clientes->currentIndex())){
        // Cria uma mensagem de confirmação
        QMessageBox::StandardButton resposta;
        resposta = QMessageBox::question(
            nullptr,
            "Confirmação",
            "Tem certeza que deseja deletar o cliente?",
            QMessageBox::Yes | QMessageBox::No
            );
        // Verifica a resposta do usuário
        if (resposta == QMessageBox::Yes) {

            // obter id selecionado
            QItemSelectionModel *selectionModel = ui->Tview_Clientes->selectionModel();
            QModelIndex selectedIndex = selectionModel->selectedIndexes().first();
            QVariant idVariant = ui->Tview_Clientes->model()->data(ui->Tview_Clientes->model()->index(selectedIndex.row(), 0));
            QString clienteId = idVariant.toString();

            if(clienteId.toInt() > 1){
                // query para deletar a partir do id
                db.open();
                QSqlQuery query;
                query.prepare("DELETE FROM clientes WHERE id = :valor1");
                query.bindValue(":valor1", clienteId);
                query.exec();

                // atualizar
                model->setQuery("SELECT * FROM clientes");
            }else{
                QMessageBox::warning(this,"Erro","Não é possivel deletar o cliente 1!");
                return;

            }
        }
    }
    else{
        QMessageBox::warning(this,"Erro","Selecione um cliente antes de deletar!");
    }
}


void Clientes::on_Btn_Novo_clicked()
{
    InserirCliente *inserircliente = new InserirCliente();
    inserircliente->setWindowModality(Qt::ApplicationModal);
    connect(inserircliente, &InserirCliente::clienteInserido, this, &Clientes::atualizarTableview);

    inserircliente->show();
}
void Clientes::atualizarTableview(){
    if(!db.open()){
        qDebug() << "erro ao abrir banco de dados. atualizarTableView";
    }
    model->setQuery("SELECT * FROM clientes");

    db.close();
}

