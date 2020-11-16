#ifndef __SHAKE_H__
#define __SHAKE_H__

#pragma pack(push,1)

#include <vector>
#include "Chart.h"

using namespace std;

struct Shake {
    int start;
    int end;
    float high;
    float low;
    float central;
    int counts;
 //   int highEdgeCount;
 //   int lowEdgeCount;
 //   int highHalfCount;
 //   int lowHalfCount;
 //   int trendCount;
};

struct Trend {
    int start;
    int end;
    int direction;
    float high;
    float low;
};

class Wave {
public:
    vector<Candle> candles;
    vector<Trend> trends;      // ���ΰ����������
    vector<Shake> shakes;      // �����а�����
    void createTrends(vector<Candle>& candles, float* pEnds, int nCount);
    void scanForSimpleShakes();
    void scanForLongShakes();
    void scanForShrinkShakes();
    void scanForShakeChannels();
};


#pragma pack(pop)

#endif
