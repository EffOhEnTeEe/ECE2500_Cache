#include <QDebug>
#include "cache.h"
#include <QFile>
#include <QString>
#include <QStringList>

//Global variables.
QList<QString> rw_instruction;//Read or write
QList<QString> address_instruction;//Addresses

void Read(QString filename)
{
    QString path = filename;
    QFile input(path);
    QTextStream stream(&input);
    if (!input.open(QFile::ReadOnly | QFile::Text))
    {
        qDebug()<<"Error: Could not find file";
        return;
    }
    while (!stream.atEnd())
    {
        QString line;
        line = stream.readLine();
        QStringList splitter = line.split("\t");
        rw_instruction.append(splitter[0]);
        address_instruction.append((splitter[1]));
    }
}

int main(int argc, char *argv[])
{
    Read(QString(argv[1]));
    qDebug()<<"Hello world";
    return 0;
}
