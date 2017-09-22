#include <QCoreApplication>
#include "multi.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Multi *multi2 = new Multi();
    multi2->init();
    return a.exec();
}
