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
    vector<Candle> rawCandles; // 元素K线表
    vector<Candle> chanCandles; // 包含处理后的K线表
    void addCandle(float high, float low, float open, float close); // 添加一根K线高和低进行处理
    void mergeCandles();
};

#pragma pack(pop)

#endif