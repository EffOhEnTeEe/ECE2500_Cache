#ifndef CACHE_H
#define CACHE_H
#include <QString>
#include <QStringList>

class Cache
{
private:
    QList<QString> readwrite;
    QList<QString> addresses;
    QList<int> int_addresses;
    QStringList final;//Each line
    QString everything;//The entire thing
public:
    Cache();
    void Read(QString filename);
    void to_int();
    void DirectMap(int cachesize, int blocksize, int wt_enable);
    void NWay(int cachesize, int blocksize, int wt_enable, int twoway, int fourway, int full);
    void toStringList(int c_size, int b_size, QString maptype, int w_type, float h_rate, int mem2cache, int cache2mem, int comparisons);
    QString getEverything();
};

#endif // CACHE_H
