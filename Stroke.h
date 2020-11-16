#ifndef __STROKE_H__
#define __STROKE_H__

#include <vector>
#include "Chart.h"

using namespace std;

#pragma pack(push, 1)

struct Point
{
    int position;            // raw
    int index;               // merged
    int bullish;             // top or bottom
};

class Stroke
{
public:
    vector<Point> points;      // 笔起止点
    void scanForStrokes(vector<Candle>& candles);
    void scanForFractals(vector<Candle>& candles);
};


void findTypicalTrends(int nCount, float* pOut, float* pIn, float* pHigh, float* pLow); // 特征序列画段

void findTrends(int nCount, float* pOut, float* pIn, float* pHigh, float* pLow); // 4点画段

#pragma pack(pop)

#endif
