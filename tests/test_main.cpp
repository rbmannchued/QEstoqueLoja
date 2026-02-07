#include <QCoreApplication>
#include <QtTest>

#include "util/test_dbutil.h"
#include "services/test_produto_service.h"
#include "services/test_barcode_service.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    int status = 0;

    status |= QTest::qExec(new TestDbUtil, argc, argv);
    status |= QTest::qExec(new TestProdutoService, argc, argv);
    status |= QTest::qExec(new test_barcode_service, argc, argv);
    return status;
}
