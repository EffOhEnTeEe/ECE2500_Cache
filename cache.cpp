#include "cache.h"
#include <QVector>
#include <QMap>
#include <QList>
#include <QStringList>
#include <QFile>
#include <QTextStream>
#include <QDebug>

Cache::Cache()
{
}
void Cache::Read(QString filename)
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
        QStringList splitter = line.split(" ");
        readwrite.append(splitter[0]);
        addresses.append((splitter[1]));
    }
    this->to_int();
}

void Cache::to_int()//Converts the address from string to an integer
{
    for (int i = 0; i < addresses.size(); i++)
    {
        addresses[i].trimmed();
        if (addresses[i].startsWith("0x"))
        {
            int temp;
            addresses[i].remove(0,2);
            temp = addresses[i].toInt(0,16);
            int_addresses.append(temp);
            //qDebug()<<int_addresses[i];
        }
        else
        {
            qDebug()<<"Not a valid address";
        }
    }
}

void Cache::DirectMap(int cachesize, int blocksize, int wt_enable)
{
    //wt_enable = 1 means write through, 0 means write back
    int num_blocks = cachesize/blocksize;//Number of blocks
    int numsets = num_blocks;//Number of sets
    int C2M = 0;//Cache to memory
    int M2C = 0;//Memory to Cache
    int age;    //Bit that keeps track of least recently used
    float hit = 0;//Keep track of hits
    float hitdenum = int_addresses.size();//Used to calculate the hit ratio
    int dirty = 0;//Dirty bit;
    float hitratio = 0;
    QVector<int> blocks(numsets);//Keeps track of tags
    //To keep track of valid bits, just check if an index in blocks is NULL or not
    QVector<int> lru(numsets); //Keep track of least recently used

    //Calculating block index and tag for each
    for (int i = 0; i<int_addresses.size();i++)
    {
        //Block index
        int b_index = (int_addresses[i]/blocksize)%num_blocks;
        //Tag
        int tag = int_addresses[i]/cachesize;
        if (addresses[i].toLower() == "read")
        {
            if (blocks[b_index] != tag)
            {
                blocks[b_index] = tag;
                M2C += blocksize;
            }
            else if (blocks[b_index] == tag)
            {
                hit++;
            }
        }
        if (addresses[i].toLower() == "write")
        {
            if (wt_enable == 1)
            {
                if (blocks[b_index] != tag)
                {
                    blocks[b_index] = tag;
                    M2C += blocksize;
                    C2M += blocksize;
                }
                else if (blocks[b_index] == tag)
                {
                    hit++;
                    C2M += blocksize;
                }
            }
            else if (wt_enable == 0)//Write back
            {
                if (blocks[b_index] != tag && blocks[b_index] == NULL)
                {
                    blocks[b_index] = tag;
                    M2C += blocksize;
                }
                if (blocks[b_index] != tag && blocks[b_index] != NULL)
                {
                    dirty++;
                    blocks[b_index] = tag;
                    M2C += blocksize;
                }
                else if (blocks[b_index] == tag)
                {
                    hit++;
                }
            }
        }
    }//For loop end bracket
    C2M = C2M + (dirty*blocksize);
}
