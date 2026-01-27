#include <QTest>
#include "test_dbutil.h"

int main(int argc, char *argv[])
{
    TestDbUtil tc;
    return QTest::qExec(&tc, argc, argv);
}
