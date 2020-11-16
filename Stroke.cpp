#include "pch.h"
#include "Stroke.h"

using namespace std;

bool lower(Candle x, Candle y) {
    if (x.high <= y.high && x.low <= y.low && x.high + x.low < y.high + y.low) {
        return true;
    }
    return false;
}


void Stroke::scanForStrokes(vector<Candle>& candles) {
    for (int i = 0; i < candles.size() - 1; i++) {
        bool insert = false;
        if (i == 0 || lower(candles[i], candles[i - 1]) && lower(candles[i], candles[i + 1]) || 
            lower(candles[i - 1], candles[i]) && lower(candles[i + 1], candles[i])) {
            Point point;
            point.index = i;
            point.position = candles[i].topPosition;
            bool bullish = lower(candles[i + 1], candles[i]);
        	if (i > 0)
        	{
                bullish = bullish && lower(candles[i - 1], candles[i]) ;
        	}
            point.bullish = bullish ? 1 : -1;
            this->points.push_back(point);
            insert = true;
        }

        if (insert && this->points.size() > 1) {
            Point lastPoint = this->points.back();
            this->points.pop_back();
            Point secondPoint = this->points.back();
            
            if (lastPoint.bullish == secondPoint.bullish) {
                if (lastPoint.bullish > 0 && candles[lastPoint.index].high > candles[secondPoint.index].high || 
                    lastPoint.bullish < 0 && candles[lastPoint.index].low < candles[secondPoint.index].low) {
                    this->points.pop_back();
                    this->points.push_back(lastPoint);
                }
            }
            else if (lastPoint.position - secondPoint.position > 3 && lastPoint.index - secondPoint.index > 2) {
                this->points.push_back(lastPoint);
            }
        	else if (lastPoint.index - secondPoint.index <= 3 && this->points.size() > 2) {
            	if (lastPoint.bullish > 0 && candles[lastPoint.index].high > candles[this->points[this->points.size() - 2].index].high && candles[secondPoint.index].low > candles[this->points[this->points.size() - 3].index].low ||
                    lastPoint.bullish < 0 && candles[lastPoint.index].low < candles[this->points[this->points.size() - 2].index].low && candles[secondPoint.index].high < candles[this->points[this->points.size() - 3].index].high) {
                    this->points.pop_back();
                    this->points.pop_back();
                    this->points.push_back(lastPoint);
	            }
            }
        }
    }
}

void Stroke::scanForFractals(vector<Candle>& candles)
{
	for (int i = 1; i < candles.size() - 1; i++)
	{
        if (lower(candles[i], candles[i - 1]) && lower(candles[i], candles[i + 1]) ||
            lower(candles[i - 1], candles[i]) && lower(candles[i + 1], candles[i])) {
            Point point;
            point.index = i;
            point.position = candles[i].topPosition;
            bool bullish = lower(candles[i + 1], candles[i]);
            if (i > 0)
            {
                bullish = bullish && lower(candles[i - 1], candles[i]);
            }
            point.bullish = bullish ? 1 : -1;
        	if (i < candles.size() - 2)
        	{
        		if(bullish && candles[i + 2].close < candles[i + 1].low)
        		{
                    point.bullish++;
        		}
                else if (!bullish && candles[i + 2].close > candles[i + 1].high)
                {
                    point.bullish--;
                }
        	} 
            this->points.push_back(point);
        }
	}
}

void findTrends(int nCount, float* pOut, float* pIn, float* pHigh, float* pLow) {
    int direction = 0;
    int prevHigh = 0;
    int currHigh = 0;
    int prevLow = 0;
    int currLow = 0;
    int highFreeze = 0;
    int lowFreeze = 0;

    for (int i = 0; i < nCount; i++) {
        // always look for 4 nearest points
        if (pIn[i] == 1) {
            prevHigh = currHigh;
            currHigh = i;
        }
        else if (pIn[i] == -1) {
            prevLow = currLow;
            currLow = i;
        }

        if (direction == 0) {
            // initialization
            if (pIn[i] != 0) {
                direction = pIn[i];
                pOut[i] = pIn[i];
            }
        }
        else if (direction == 1) {
            // bullish
            if (pIn[i] == 1) {
                if (highFreeze != 0) {
                    if (pHigh[currHigh] >= pHigh[highFreeze]) {
                        pOut[highFreeze] = 0;
                        pOut[currHigh] = 1;
                        highFreeze = 0;
                        lowFreeze = 0;
                    }
                }
                else if (pHigh[prevHigh] < pHigh[currHigh]) {
                    pOut[prevHigh] = 0;
                    pOut[currHigh] = 1;
                }
                else if (pHigh[prevHigh] >= pHigh[currHigh]) {
                    highFreeze = prevHigh;
                    lowFreeze = currLow;
                }
            }
            else if (pIn[i] == -1) {
                if (lowFreeze != 0) {
                    if (pLow[currLow] <= pLow[lowFreeze]) {
                        direction = -1;
                        pOut[highFreeze] = 1;
                        pOut[currLow] = -1;
                        highFreeze = 0;
                        lowFreeze = 0;
                    }
                }
                else if (pLow[prevLow] > pLow[currLow]) {
                    direction = -1;
                    pOut[currLow] = -1;
                    highFreeze = 0;
                    lowFreeze = 0;
                }
            }
        }
        else {
            // bearish
            if (pIn[i] == -1) {
                if (lowFreeze != 0) {
                    if (pLow[currLow] <= pLow[lowFreeze]) {
                        pOut[lowFreeze] = 0;
                        pOut[currLow] = -1;
                        highFreeze = 0;
                        lowFreeze = 0;
                    }
                }
                else if (pLow[prevLow] > pLow[currLow]) {
                    pOut[prevLow] = 0;
                    pOut[currLow] = -1;
                }
                else if (pLow[prevLow] <= pLow[currLow]) {
                    lowFreeze = prevLow;
                    highFreeze = currHigh;
                }
            }
            else if (pIn[i] == 1) {
                if (highFreeze != 0) {
                    if (pHigh[currHigh] >= pHigh[highFreeze]) {
                        direction = 1;
                        pOut[lowFreeze] = -1;
                        pOut[currHigh] = 1;
                        highFreeze = 0;
                        lowFreeze = 0;
                    }
                }
                else if (pHigh[prevHigh] < pHigh[currHigh]) {
                    direction = 1;
                    pOut[currHigh] = 1;
                    highFreeze = 0;
                    lowFreeze = 0;
                }
            }
        }
    }
}


void findTypicalTrends(int nCount, float* pOut, float* pIn, float* pHigh, float* pLow)
{
    int nState = 0;    // 当前方向
    int nLastD = 0; // 前一个向下线段的底位置
    int nLastG = 0; // 前一个向上线段的顶位置
    float fTop0 = 0;
    float fTop1 = 0;       //前高
    float fTop2 = 0;       //现高
    float fBot0 = 0;       // 向上线段内前低
    float fBot1 = 0;
    float fBot2 = 0;
    for (int i = 0; i < nCount; i++)
    {
        if (pIn[i] == 1)   // 高点
        {
            fTop1 = fTop2;
            fTop2 = pHigh[i];
        }
        else if (pIn[i] == -1) {   // 低点
            fBot1 = fBot2;
            fBot2 = pLow[i];
        }

        if (nState == 0)
        {
            if (pIn[i] == 1)       // 高点
            {
                nState = 1;
                nLastG = i;
                pOut[nLastG] = 1;
                fTop0 = 0;
                fBot0 = 0;
            }
            else if (pIn[i] == -1)     // 低点
            {
                nState = -1;
                nLastD = i;
                pOut[nLastD] = -1;
                fTop0 = 0;
                fBot0 = 0;
            }
        }
        else if (nState == 1) // 向上线段运行中
        {
            if (pIn[i] == 1) // 遇到高点
            {
                if (pHigh[i] > pHigh[nLastG]) // 比线段最高点还高，高点后移
                {
                    pOut[nLastG] = 0;
                    nLastG = i;
                    pOut[nLastG] = 1;
                    fTop0 = 0;
                    fBot0 = 0;
                }
            }
            else if (pIn[i] == -1) // 遇到低点
            {
                if (pLow[i] < pLow[nLastD]) // 低点比向上线段最低点还低了，当一段处理，也是终结。
                {
                    nState = -1;
                    nLastD = i;
                    pOut[nLastD] = -1;
                    fTop0 = 0;
                    fBot0 = 0;
                }
                else if (fTop1 > 0 && fTop2 > 0 && fBot1 > 0 && fBot2 > 0 && fTop2 < fTop1 && fBot2 < fBot1) // 向上线段终结
                {
                    nState = -1;
                    nLastD = i;
                    pOut[nLastD] = -1;
                    fTop0 = 0;
                    fBot0 = 0;
                }
                else
                {
                    if (fBot0 == 0)
                    {
                        fBot0 = pLow[i];
                    }
                    else if (pLow[i] < fBot0) // 向上线段终结
                    {
                        nState = -1;
                        nLastD = i;
                        pOut[nLastD] = -1;
                        fTop0 = 0;
                        fBot0 = 0;
                    }
                }
            }
        }
        else if (nState == -1) // 向下线段运行中
        {
            if (pIn[i] == -1) // 遇到低点
            {
                if (pLow[i] < pLow[nLastD]) // 比线段最低点还低，低点后移
                {
                    pOut[nLastD] = 0;
                    nLastD = i;
                    pOut[nLastD] = -1;
                    fTop0 = 0;
                    fBot0 = 0;
                }
            }
            else if (pIn[i] == 1) // 遇到高点
            {
                if (pHigh[i] > pHigh[nLastG]) // 高点比向下线段最高点还高了，当一段处理，也是终结。
                {
                    nState = 1;
                    nLastG = i;
                    pOut[nLastG] = 1;
                    fTop0 = 0;
                    fBot0 = 0;
                }
                else if (fTop1 > 0 && fTop2 > 0 && fBot1 > 0 && fBot2 > 0 && fTop2 > fTop1 && fBot2 > fBot1) // 向下线段终结
                {
                    nState = 1;
                    nLastG = i;
                    pOut[nLastG] = 1;
                    fTop0 = 0;
                    fBot0 = 0;
                }
                else
                {
                    if (fTop0 == 0)
                    {
                        fTop0 = pHigh[i];
                    }
                    else if (pHigh[i] > fTop0) // 向下线段终结
                    {
                        nState = 1;
                        nLastG = i;
                        pOut[nLastG] = 1;
                        fTop0 = 0;
                        fBot0 = 0;
                    }
                }
            }
        }
    }
}
