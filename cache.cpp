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
    QList<QString> rw_instruction;//Read or write
    QList<QString> address_instruction;//Addresses
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
        QStringList splitter = line.split("\t");
        readwrite.append(splitter[0]);
        addresses.append((splitter[1]));
    }
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
    float hit = 0;//Keep track of hits
    float hitdenum = int_addresses.size();//Used to calculate the hit ratio
    float hitratio = 0;
    QVector<int> blocks(numsets);//Keeps track of tags
    QVector<int> written(numsets); //Keep track of written bits for write back. A method of implementing valid bits
    //Calculating block index and tag for each
    for (int i = 0; i<int_addresses.size();i++)
    {
        //Block index
        int b_index = (int_addresses[i]/blocksize)%num_blocks;
        qDebug()<<(i+1)<<":"<<"Block index is "<<b_index;
        //Tag
        int tag = int_addresses[i]/blocksize/numsets;
        qDebug()<<(i+1)<<":"<<"Tag is "<<tag;
        if (readwrite[i].toLower() == "read")
        {
            qDebug()<<"Read block executed";
            if ( (blocks[b_index] != tag) && (written[b_index]!= 1))
            {
                //If tag doesn't match and no data or
                //data that wasn't written
                blocks[b_index] = tag;
                M2C += blocksize;
                qDebug()<<"1 Hit is"<<hit<< " M2C is "<<M2C<<" C2M is"<<C2M<<"\n";;
            }
            else if ( (blocks[b_index] != tag) && (written[b_index] == 1)) //Tag doesn't match and data was recently written
            {
                //If tag doesn't match and written data exists
                blocks[b_index] = tag;
                written[b_index] = 0;//Indicates data is there not as a result of a write
                C2M += blocksize;
                qDebug()<<"2 Hit is"<<hit<< " M2C is "<<M2C<<" C2M is"<<C2M<<"\n";

            }
            else if (blocks[b_index] == tag)
            {
                hit++;
                qDebug()<<"3 Hit is"<<hit<< " M2C is "<<M2C<<" C2M is"<<C2M<<"\n";;
            }
        }
        else if (readwrite[i].toLower() == "write")
        {
            if (wt_enable == 1)//Write through
            {
                qDebug()<<"Write through";
                if (blocks[b_index] != tag)
                {
                    blocks[b_index] = tag;
                    M2C += blocksize;
                    C2M += blocksize;
                    qDebug()<<"4 Hit is"<<hit<< " M2C is "<<M2C<<" C2M is"<<C2M<<"\n";;
                }
                else if (blocks[b_index] == tag)
                {
                    hit++;
                    C2M += blocksize;
                    qDebug()<<"5 Hit is"<<hit<< " M2C is "<<M2C<<" C2M is"<<C2M<<"\n";;
                }
            }
            else if (wt_enable == 0)//Write back
            {
                qDebug()<<"Write back";
                if (blocks[b_index] != tag && (written[b_index]!= 1))
                {
                    blocks[b_index] = tag;
                    written[b_index] = 1;//Signify this was just recently written to
                    M2C += blocksize;
                    qDebug()<<"6 Hit is"<<hit<< " M2C is "<<M2C<<" C2M is"<<C2M<<"\n";;
                }
                else if (blocks[b_index] != tag && (written[b_index] == 1))
                {
                    blocks[b_index] = tag;
                    M2C += blocksize;
                    C2M += blocksize;
                    qDebug()<<"7 Hit is"<<hit<< " M2C is "<<M2C<<" C2M is"<<C2M<<"\n";;
                }
                else if (blocks[b_index] == tag)
                {
                    hit++;
                    written[b_index] = 1;//Signify this was just recently written to
                    qDebug()<<"8 Hit is"<<hit<< " M2C is "<<M2C<<" C2M is"<<C2M<<"\n";;
                }
            }
        }
    }//For loop end bracket
    hitratio = hit/hitdenum;
    for (int i = 0; i<written.size(); i++)//Evict the last bits
    {
        C2M += written[i]*blocksize;
    }
    qDebug()<<"For 1024 cache, 8 block size, write back, hit ratio is "<<hitratio<<"M2C is "<<M2C<<" C2M is "<<C2M;
}

void Cache::NWay(int cachesize, int blocksize, int wt_enable, int twoway, int fourway, int full)
{
    //wt_enable = 1 means write through, 0 means write back
    int num_blocks = cachesize/blocksize;//Number of blocks
    int age = 0;    //Used to keep track of LRU
    int numsets;
    int numways;
    //Deciding the number of sets
    if (twoway)
    {
        numsets = num_blocks/2;//Number of sets
    }
    else if (fourway)
    {
        numsets = num_blocks/4;
    }
    else if (full)
    {
        numsets = 1;
    }
    //Deciding ways
    if (twoway)
    {
        numways = 2;
    }
    else if (fourway)
    {
        numways = 4;
    }
    else if (full)
    {
        numways = num_blocks;
    }
    int C2M = 0;//Cache to memory
    int M2C = 0;//Memory to Cache
    float hit = 0;//Keep track of hits
    float hitdenum = int_addresses.size();//Used to calculate the hit ratio
    float hitratio = 0;

    QVector< QVector<int> > blocks(numways);//Keeps track of tags
    QVector< QVector<int> > written(numways);   //Keep track of written bits for write back. Null signifies empty.
    //0 signifies read/write WT. 1 signifies written WB.
    QVector< QVector<int> > LRU(numways); //Keep track of written bits for write back.

    for(int i = 0; i < numways; i++)//Appends to the Vectors
    {
        QVector<int> temp(numsets);
        blocks.append(temp);
        written.append(temp);
        LRU.append(temp);

    }
    //Start of the algorithm
    for (int i = 0; i<int_addresses.size();i++)
    {
        //Block index
        int b_index = (int_addresses[i]/blocksize)%num_blocks;
        qDebug()<<(i+1)<<":"<<"Block index is "<<b_index;
        //Tag
        int tag = int_addresses[i]/cachesize;
        qDebug()<<(i+1)<<":"<<"Tag is "<<tag;

        if (readwrite[i].toLower() == "read")
        {
            int flag_hit = 0;//If a hit
            int flag_found = 0;//If a space is found
            //Checking for a hit
            for(int i = 0; i<numways; i++)//Iterate through all the ways
            {
                if(blocks[i][b_index] == tag)
                {
                    hit++;
                    LRU[i][b_index] = age;
                    age++;
                    flag_hit = 1;
                    break;
                }
            }
            //If a hit was not found, see if there is an empty space for the data
            if (flag_hit != 1)
            {
                for(int i = 0; i<numways; i++)//Iterate through all the ways
                {
                    if((blocks[i][b_index]!= tag) && (written[i][b_index]== NULL))
                    {
                        //If tag doesn't match and no data is there
                        blocks[i][b_index] = tag;
                        written[i][b_index] = 0;
                        LRU[i][b_index] = age;
                        age++;
                        M2C += blocksize;
                        qDebug()<<"1 Hit is"<<hit<< " M2C is "<<M2C<<" C2M is"<<C2M<<" age is "<<age<<"\n";
                        flag_found = 1;//Placement found
                        break;
                    }
                }
            }
            if (flag == 0)//If place
            {
                for(int i = 0; i<numways; i++)//Iterate through all the ways
                {

                }
            }
        }
        if (readwrite[i].toLower() == "write")
        {

        }
    }
}
