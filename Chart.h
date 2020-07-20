#ifndef __CHART_H__
#define __CHART_H__

#include <vector>

using namespace std;

#pragma pack(push, 1)

struct Candle
{
    float high;
    float low;
    float open;
    float close;
    int startPosition;
    int endPosition;
    int topPosition;
};

class Chart
{
public:
    vector<Candle> rawCandles; // Ԫ��K�߱�
    vector<Candle> chanCandles; // ����������K�߱�
    void addCandle(float high, float low, float open, float close); // ���һ��K�߸ߺ͵ͽ��д���
    void mergeCandles();
};

#pragma pack(pop)

#endif