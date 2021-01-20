#ifndef __INDICATOR_H__
#define __INDICATOR_H__

#pragma pack(push,1)

#include <vector>
#include "Chart.h"
#include "Shake.h"

using namespace std;

vector<int> exceedEarlyHigh(vector<Candle>& candles, vector<int>& points);

vector<int> reachEarlyLow(vector<Candle>& candles, vector<int>& points);

vector<int> findFracAroundShake(vector<Candle>& candles, vector<Shake>& shakes, int dataLen);

vector<int> findNegFracAroundShake(vector<Candle>& candles, vector<Shake>& shakes, int dataLen);

vector<int> findZeroDivergence(vector<Candle>& candles, float* pIn, int dataLen);

vector<int> findRsiDivergence(vector<Candle>& candles, float* pIn, int dataLen);

vector<int> findCrossDivergence(float* pPrice, float* pFast, float* pSlow, int dataLen);

vector<int> findSingleLineDivergence(float* pPrice, float* pInd, int dataLen);

vector<float> findPivotLines(vector<Candle>& candles);

#pragma pack(pop)

#endif
