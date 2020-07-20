#ifndef __INDICATOR_H__
#define __INDICATOR_H__

#pragma pack(push,1)

#include <vector>
#include "Chart.h"

using namespace std;

vector<int> exceedEarlyHigh(vector<Candle>& candles, vector<int>& points);

vector<int> reachEarlyLow(vector<Candle>& candles, vector<int>& points);


#pragma pack(pop)

#endif
