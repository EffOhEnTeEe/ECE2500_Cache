#include <QDebug>
#include "cache.h"
#include <QFile>
#include <QString>
#include <QStringList>

//Global variables.
QList<QString> rw_instruction;//Read or write
QList<QString> address_instruction;//Addresses
QList<QString> final;       //What you output to the file. Contains all the data


int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        qFatal("Error: incorrent number of arguments");
    }
    Cache c1;
    c1.Read(QString(argv[1]));
    c1.to_int();
    //1024 cache, 8 block size
    c1.DirectMap(1024,8,0);
    c1.DirectMap(1024,8,1);
    c1.NWay(1024, 8, 0, 1, 0, 0);
    c1.NWay(1024, 8, 1, 1, 0, 0);
    c1.NWay(1024, 8, 0, 0, 1, 0);
    c1.NWay(1024, 8, 1, 0, 1, 0);
    c1.NWay(1024, 8, 0, 0, 0, 1);
    c1.NWay(1024, 8, 1, 0, 0, 1);
    //1024 cache, 16 block size
    c1.DirectMap(1024,16,0);
    c1.DirectMap(1024,16,1);
    c1.NWay(1024, 16, 0, 1, 0, 0);
    c1.NWay(1024, 16, 1, 1, 0, 0);
    c1.NWay(1024, 16, 0, 0, 1, 0);
    c1.NWay(1024, 16, 1, 0, 1, 0);
    c1.NWay(1024, 16, 0, 0, 0, 1);
    c1.NWay(1024, 16, 1, 0, 0, 1);
    //1024 cache, 32 block size
    c1.DirectMap(1024,32,0);
    c1.DirectMap(1024,32,1);
    c1.NWay(1024, 32, 0, 1, 0, 0);
    c1.NWay(1024, 32, 1, 1, 0, 0);
    c1.NWay(1024, 32, 0, 0, 1, 0);
    c1.NWay(1024, 32, 1, 0, 1, 0);
    c1.NWay(1024, 32, 0, 0, 0, 1);
    c1.NWay(1024, 32, 1, 0, 0, 1);
    //1024 cache, 128 block size
    c1.DirectMap(1024,128,0);
    c1.DirectMap(1024,128,1);
    c1.NWay(1024, 128, 0, 1, 0, 0);
    c1.NWay(1024, 128, 1, 1, 0, 0);
    c1.NWay(1024, 128, 0, 0, 1, 0);
    c1.NWay(1024, 128, 1, 0, 1, 0);
    c1.NWay(1024, 128, 0, 0, 0, 1);
    c1.NWay(1024, 128, 1, 0, 0, 1);
    //////////////////////////////////
    //4K cache, 8 block size
    c1.DirectMap(4096,8,0);
    c1.DirectMap(4096,8,1);
    c1.NWay(4096, 8, 0, 1, 0, 0);
    c1.NWay(4096, 8, 1, 1, 0, 0);
    c1.NWay(4096, 8, 0, 0, 1, 0);
    c1.NWay(4096, 8, 1, 0, 1, 0);
    c1.NWay(4096, 8, 0, 0, 0, 1);
    c1.NWay(4096, 8, 1, 0, 0, 1);
    //4096 cache, 16 block size
    c1.DirectMap(4096,16,0);
    c1.DirectMap(4096,16,1);
    c1.NWay(4096, 16, 0, 1, 0, 0);
    c1.NWay(4096, 16, 1, 1, 0, 0);
    c1.NWay(4096, 16, 0, 0, 1, 0);
    c1.NWay(4096, 16, 1, 0, 1, 0);
    c1.NWay(4096, 16, 0, 0, 0, 1);
    c1.NWay(4096, 16, 1, 0, 0, 1);
    //4096 cache, 32 block size
    c1.DirectMap(4096,32,0);
    c1.DirectMap(4096,32,1);
    c1.NWay(4096, 32, 0, 1, 0, 0);
    c1.NWay(4096, 32, 1, 1, 0, 0);
    c1.NWay(4096, 32, 0, 0, 1, 0);
    c1.NWay(4096, 32, 1, 0, 1, 0);
    c1.NWay(4096, 32, 0, 0, 0, 1);
    c1.NWay(4096, 32, 1, 0, 0, 1);
    //4096 cache, 128 block size
    c1.DirectMap(4096,128,0);
    c1.DirectMap(4096,128,1);
    c1.NWay(4096, 128, 0, 1, 0, 0);
    c1.NWay(4096, 128, 1, 1, 0, 0);
    c1.NWay(4096, 128, 0, 0, 1, 0);
    c1.NWay(4096, 128, 1, 0, 1, 0);
    c1.NWay(4096, 128, 0, 0, 0, 1);
    c1.NWay(4096, 128, 1, 0, 0, 1);

    //////64K

    c1.DirectMap(65536,8,0);
    c1.DirectMap(65536,8,1);
    c1.NWay(65536, 8, 0, 1, 0, 0);
    c1.NWay(65536, 8, 1, 1, 0, 0);
    c1.NWay(65536, 8, 0, 0, 1, 0);
    c1.NWay(65536, 8, 1, 0, 1, 0);
    c1.NWay(65536, 8, 0, 0, 0, 1);
    c1.NWay(65536, 8, 1, 0, 0, 1);
    //65536 cache, 16 block size
    c1.DirectMap(65536,16,0);
    c1.DirectMap(65536,16,1);
    c1.NWay(65536, 16, 0, 1, 0, 0);
    c1.NWay(65536, 16, 1, 1, 0, 0);
    c1.NWay(65536, 16, 0, 0, 1, 0);
    c1.NWay(65536, 16, 1, 0, 1, 0);
    c1.NWay(65536, 16, 0, 0, 0, 1);
    c1.NWay(65536, 16, 1, 0, 0, 1);
    //65536 cache, 32 block size
    c1.DirectMap(65536,32,0);
    c1.DirectMap(65536,32,1);
    c1.NWay(65536, 32, 0, 1, 0, 0);
    c1.NWay(65536, 32, 1, 1, 0, 0);
    c1.NWay(65536, 32, 0, 0, 1, 0);
    c1.NWay(65536, 32, 1, 0, 1, 0);
    c1.NWay(65536, 32, 0, 0, 0, 1);
    c1.NWay(65536, 32, 1, 0, 0, 1);
    //65536 cache, 128 block size
    c1.DirectMap(65536,128,0);
    c1.DirectMap(65536,128,1);
    c1.NWay(65536, 128, 0, 1, 0, 0);
    c1.NWay(65536, 128, 1, 1, 0, 0);
    c1.NWay(65536, 128, 0, 0, 1, 0);
    c1.NWay(65536, 128, 1, 0, 1, 0);
    c1.NWay(65536, 128, 0, 0, 0, 1);
    c1.NWay(65536, 128, 1, 0, 0, 1);

    //128K
    c1.DirectMap(131072,8,0);
    c1.DirectMap(131072,8,1);
    c1.NWay(131072, 8, 0, 1, 0, 0);
    c1.NWay(131072, 8, 1, 1, 0, 0);
    c1.NWay(131072, 8, 0, 0, 1, 0);
    c1.NWay(131072, 8, 1, 0, 1, 0);
    c1.NWay(131072, 8, 0, 0, 0, 1);
    c1.NWay(131072, 8, 1, 0, 0, 1);
    //131072 cache, 16 block size
    c1.DirectMap(131072,16,0);
    c1.DirectMap(131072,16,1);
    c1.NWay(131072, 16, 0, 1, 0, 0);
    c1.NWay(131072, 16, 1, 1, 0, 0);
    c1.NWay(131072, 16, 0, 0, 1, 0);
    c1.NWay(131072, 16, 1, 0, 1, 0);
    c1.NWay(131072, 16, 0, 0, 0, 1);
    c1.NWay(131072, 16, 1, 0, 0, 1);
    //131072 cache, 32 block size
    c1.DirectMap(131072,32,0);
    c1.DirectMap(131072,32,1);
    c1.NWay(131072, 32, 0, 1, 0, 0);
    c1.NWay(131072, 32, 1, 1, 0, 0);
    c1.NWay(131072, 32, 0, 0, 1, 0);
    c1.NWay(131072, 32, 1, 0, 1, 0);
    c1.NWay(131072, 32, 0, 0, 0, 1);
    c1.NWay(131072, 32, 1, 0, 0, 1);
    //131072 cache, 128 block size
    c1.DirectMap(131072,128,0);
    c1.DirectMap(131072,128,1);
    c1.NWay(131072, 128, 0, 1, 0, 0);
    c1.NWay(131072, 128, 1, 1, 0, 0);
    c1.NWay(131072, 128, 0, 0, 1, 0);
    c1.NWay(131072, 128, 1, 0, 1, 0);
    c1.NWay(131072, 128, 0, 0, 0, 1);
    c1.NWay(131072, 128, 1, 0, 0, 1);


    //Writing to the file
    QString outputname = QString(argv[1]);
    outputname.append(".result");
    QFile output(outputname);

    if(!output.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qFatal("Error: could not open output file");
    }

    QTextStream stream(&output);

    stream << c1.getEverything();
    return 0;
}
