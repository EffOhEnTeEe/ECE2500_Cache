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
                qDebug()<<"1 Hit is"<<hit<< " M2C is "<<M2C<<" C2M is"<<C2M<<"\n";
            }
            else if ( (blocks[b_index] != tag) && (written[b_index] == 1)) //Tag doesn't match and data was recently written
            {
                //If tag doesn't match and written data exists
                blocks[b_index] = tag;
                written[b_index] = 0;//Indicates data is there not as a result of a write
                M2C += blocksize;
                C2M += blocksize;
                qDebug()<<"2 Hit is"<<hit<< " M2C is "<<M2C<<" C2M is"<<C2M<<"\n";

            }
            else if (blocks[b_index] == tag)
            {
                hit++;
                qDebug()<<"3 Hit is"<<hit<< " M2C is "<<M2C<<" C2M is"<<C2M<<"\n";
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
                    C2M += 4;
                    qDebug()<<"4 Hit is"<<hit<< " M2C is "<<M2C<<" C2M is"<<C2M<<"\n";
                }
                else if (blocks[b_index] == tag)
                {
                    hit++;
                    C2M += 4;
                    qDebug()<<"5 Hit is"<<hit<< " M2C is "<<M2C<<" C2M is"<<C2M<<"\n";
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
                    qDebug()<<"6 Hit is"<<hit<< " M2C is "<<M2C<<" C2M is"<<C2M<<"\n";
                }
                else if (blocks[b_index] != tag && (written[b_index] == 1))
                {
                    blocks[b_index] = tag;
                    M2C += blocksize;
                    C2M += blocksize;
                    qDebug()<<"7 Hit is"<<hit<< " M2C is "<<M2C<<" C2M is"<<C2M<<"\n";
                }
                else if (blocks[b_index] == tag)
                {
                    hit++;
                    written[b_index] = 1;//Signify this was just recently written to
                    qDebug()<<"8 Hit is"<<hit<< " M2C is "<<M2C<<" C2M is"<<C2M<<"\n";
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
        numsets = num_blocks/2;
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
    QVector< QVector<int> > LRU(numways); //Keep track of age bits

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
        int b_index = (int_addresses[i]/blocksize)%numsets;
        qDebug()<<(i+1)<<":"<<"Block index is "<<b_index;
        //Tag
        int tag = int_addresses[i]/blocksize/numsets;
        qDebug()<<(i+1)<<":"<<"Tag is "<<tag;
        qDebug()<<(i+1)<<":"<<"# of sets is"<<numsets;
        qDebug()<<(i+1)<<":"<<"# of ways is"<<numways;

        if (readwrite[i].toLower() == "read")
        {
            int flag_hit = 0;//If a hit
            int flag_found = 0;//If a space is found
            //Checking for a hit
            for(int a = 0; a<numways; a++)//Iterate through all the ways to find a hit
            {
                if(blocks[a][b_index] == tag)
                {
                    hit++;
                    LRU[a][b_index] = age;
                    age++;
                    flag_hit = 1;
                    qDebug()<<"ReadHit: Hit is"<<hit<< " M2C is "<<M2C<<" C2M is "<<C2M<<" age is "<<age<<
                              " written is"<<written[a][b_index]<<"\n";
                    break;
                }
            }
            //If a hit was not found, see if there is an empty space for the data
            if (flag_hit != 1)
            {
                for(int a = 0; a<numways; a++)//Iterate through all the ways to find empty set
                {
                    if((blocks[a][b_index]!= tag) && (written[a][b_index]== NULL))
                    {
                        //If tag doesn't match and no data is there
                        blocks[a][b_index] = tag;
                        written[a][b_index] = 0;
                        LRU[a][b_index] = age;
                        age++;
                        M2C += blocksize;
                        flag_found = 1;//Placement found
                        qDebug()<<"ReadPlace: Hit is"<<hit<< " M2C is "<<M2C<<" C2M is "<<C2M<<" age is "<<age<<
                                  " written is"<<written[a][b_index]<<"\n";
                        break;
                    }
                }
            }
            if (flag_found == 0)//If placement was not found, perform LRU algorithm
            {
                int r_i = 0;//Replaced i
                for(int a = 0; a<numways-1; a++)//Iterate through all the ways
                {
                    if(LRU[r_i][b_index] < LRU[a+1][b_index])
                    {
                        r_i = a;
                    }
                    else
                    {
                        r_i = a+1;
                    }
                }
                if (written[r_i][b_index]== 1)//Data here was from a write back
                {
                    blocks[r_i][b_index] = tag;
                    written[r_i][b_index] = 0;//Indicates data is there not as a result of a write
                    M2C += blocksize;//Get data from memory since it's a read miss
                    C2M += blocksize;//Eviction so C2M needs to update
                    LRU[r_i][b_index] = age;//Update age bit to make it most recent
                    age++;
                    qDebug()<<"ReadLRU1: Hit is"<<hit<< " M2C is "<<M2C<<" C2M is "<<C2M<<" age is "<<age<<
                              " written is"<<written[r_i][b_index]<<"\n";
                }
                if (written[r_i][b_index]== 0)//Data here was not from a write back
                {
                    blocks[r_i][b_index] = tag;
                    written[r_i][b_index] = 0;//Indicates data is there not as a result of a write
                    M2C += blocksize;//Get data from memory since it's a read miss
                    LRU[r_i][b_index] = age;//Update age bit to make it most recent
                    age++;
                    qDebug()<<"ReadLRU0: Hit is"<<hit<< " M2C is "<<M2C<<" C2M is "<<C2M<<" age is "<<age<<
                              " written is"<<written[r_i][b_index]<<"\n";
                }
            }
        }
        if (readwrite[i].toLower() == "write")
        {
            if (wt_enable == 1)//Write through
            {
                int flag_hit = 0;//If a hit
                int flag_found = 0;//If a space is found
                //Checking for a hit
                for(int a = 0; a<numways; a++)//Iterate through all the ways to find a hit
                {
                    if(blocks[a][b_index] == tag)
                    {
                        hit++;
                        LRU[a][b_index] = age;
                        age++;
                        C2M += 4;
                        flag_hit = 1;
                        qDebug()<<"WTHit: Hit is"<<hit<< " M2C is "<<M2C<<" C2M is "<<C2M<<" age is "<<age<<
                                  " written is"<<written[a][b_index]<<"\n";
                        break;
                    }
                }
                //If a hit was not found, see if there is an empty space for the data
                if (flag_hit != 1)
                {
                    for(int a = 0; a<numways; a++)//Iterate through all the ways to find empty set
                    {
                        if((blocks[a][b_index]!= tag) && (written[a][b_index]== NULL))
                        {
                            //If tag doesn't match and no data is there
                            blocks[a][b_index] = tag;
                            written[a][b_index] = 0;
                            LRU[a][b_index] = age;
                            age++;
                            M2C += blocksize;
                            C2M += 4;
                            flag_found = 1;//Placement found
                            qDebug()<<"WTPlaced: Hit is"<<hit<< " M2C is "<<M2C<<" C2M is "<<C2M<<" age is "<<age<<
                                      " written is"<<written[a][b_index]<<"\n";
                            break;
                        }
                    }
                }
                if (flag_found == 0)//If placement was not found, perform LRU algorithm
                {
                    int r_i = 0;//Replaced i
                    for(int a = 0; a<numways-1; a++)
                    {
                        if(LRU[r_i][b_index] < LRU[a+1][b_index])
                        {
                            r_i = a;
                        }
                        else
                        {
                            r_i = a+1;
                        }
                    }
                    blocks[r_i][b_index] = tag;
                    written[r_i][b_index] = 0;//Indicates there is data. Do this just in case
                    M2C += blocksize;//Get data from memory since it's a read miss
                    C2M += 4;//Write data to memory
                    LRU[r_i][b_index] = age;//Update age bit to make it most recent
                    age++;
                    qDebug()<<"WTLRU: Hit is"<<hit<< " M2C is "<<M2C<<" C2M is "<<C2M<<" age is "<<age<<
                              " written is"<<written[r_i][b_index]<<"\n";
                }
            }//End of WT

            else if (wt_enable == 0)//Write back
            {
                int flag_hit = 0;//If a hit
                int flag_found = 0;//If a space is found
                //Checking for a hit
                for(int a = 0; a<numways; a++)//Iterate through all the ways to find a hit
                {
                    if(blocks[a][b_index] == tag)
                    {
                        hit++;
                        LRU[a][b_index] = age;
                        age++;
                        flag_hit = 1;
                        qDebug()<<"WBHit: Hit is"<<hit<< " M2C is "<<M2C<<" C2M is "<<C2M<<" age is "<<age<<
                                  " written is"<<written[a][b_index]<<"\n";
                        break;
                    }
                }
                //If a hit was not found, see if there is an empty space for the data
                if (flag_hit != 1)
                {
                    for(int a = 0; a<numways; a++)//Iterate through all the ways to find empty set
                    {
                        if((blocks[a][b_index]!= tag) && (written[a][b_index]== NULL))
                        {
                            //If tag doesn't match and no data is there
                            blocks[a][b_index] = tag;
                            written[a][b_index] = 1;
                            LRU[a][b_index] = age;
                            age++;
                            M2C += blocksize;
                            flag_found = 1;//Placement found
                            qDebug()<<"WBPlaced: Hit is"<<hit<< " M2C is "<<M2C<<" C2M is "<<C2M<<" age is "<<age<<
                                      " written is"<<written[a][b_index]<<"\n";
                            break;
                        }
                    }
                }
                if (flag_found == 0)//If placement was not found, perform LRU algorithm
                {
                    int r_i = 0;//Replaced i
                    for(int a = 0; a<numways-1; a++)
                    {
                        if(LRU[r_i][b_index] < LRU[a+1][b_index])
                        {
                            r_i = a;
                        }
                        else
                        {
                            r_i = a+1;
                        }
                    }
                    if (written[r_i][b_index] == 1)//If data there was from a write back
                    {
                        blocks[r_i][b_index] = tag;
                        written[r_i][b_index] = 1;//Indicates data is there as a result of a write back. Do this just in case
                        M2C += blocksize;//Get data from memory since it's a read miss
                        C2M += blocksize;//Write data to memory since evicting
                        LRU[r_i][b_index] = age;//Update age bit to make it most recent
                        age++;
                        qDebug()<<"WBLRU1: Hit is"<<hit<< " M2C is "<<M2C<<" C2M is "<<C2M<<" age is "<<age<<
                                  " written is"<<written[r_i][b_index]<<"\n";
                    }
                    if (written[r_i][b_index] == 0)//If data there is not from a write back
                    {
                        blocks[r_i][b_index] = tag;
                        written[r_i][b_index] = 1;//Indicates data is there as a result of a write back. Do this just in case
                        M2C += blocksize;//Get data from memory since it's a read miss
                        LRU[r_i][b_index] = age;//Update age bit to make it most recent
                        age++;
                        qDebug()<<"WBLRU0: Hit is"<<hit<< " M2C is "<<M2C<<" C2M is "<<C2M<<" age is "<<age<<
                                  " written is"<<written[r_i][b_index]<<"\n";
                    }
                }
            }
        }
    }//End of for loop
    hitratio = hit/hitdenum;
    for (int i = 0; i<num_blocks; i++)//Evict the last bits
    {
        for (int j = 0; j<numsets; j++)
            C2M += written[i][j]*blocksize;
    }
    qDebug()<<"For 1024 cache, 8 block size, write through, 2 way, hit ratio is "<<hitratio<<"M2C is "<<M2C<<" C2M is "<<C2M;
}
