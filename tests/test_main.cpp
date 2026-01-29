#include <QCoreApplication>
#include <QtTest>

#include "util/test_dbutil.h"
#include "services/test_inserirproduto_service.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    int status = 0;

    status |= QTest::qExec(new TestDbUtil, argc, argv);
    status |= QTest::qExec(new TestInserirProdutoService, argc, argv);

    return status;
}
