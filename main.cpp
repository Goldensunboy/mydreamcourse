#include <QtGui/QApplication>
#include <qdebug.h>

#include "mdcwindow.h"
#include "defs.h"

QByteArray file;
u16 header;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MDCWindow window;
    window.show();

    return a.exec();
}

void debugPrint(QByteArray array, int len)
{
    QString str, hex;
    for(int i = 0; i < len; ++i)
    {
        if(!(i % 16))
        {
            str.append("\n");
        }
        hex.setNum((byte)array[i], 16);
        if((byte)array[i] < 16)
        {
            str.append("0");
        }
        str.append(hex.toUpper() + " ");
    }
    qDebug() << str.append("\n");
}

void debugPrint(QByteArray array)
{
    debugPrint(array, array.size());
}
