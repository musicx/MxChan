#include "pch.h"
#include "FuncReg.h"

#include <vector>

#include "Chart.h"
#include "Stroke.h"
#include "Shake.h"
#include "Indicator.h"

using namespace std;

//=============================================================================
// 输出函数1号：输出笔顶底端点
//OOR:= ROUND((1 - O / H) * 10000) * 10000;
//CCR:= ROUND((1 - C / H) * 10000);
//OC:= OOR + CCR;
//=============================================================================
void Func1(int dataLen, float* pOut, float* pHigh, float* pLow, float* pOC)
{
    if (dataLen < 10) {
        for (int i = 0; i < dataLen; i++) {
            pOut[i] = 0;
        }
    }
    else {
        Chart chart;
        for (int i = 0; i < dataLen; i++) {
            //float close = pC[i];
            //float open = pO[i];
            //float high = close * (1 + round(pHL[i] / 10000.) / 10000.);
            //float low = close * (1 - (int(pHL[i]) % 10000) / 10000.);

            float high = pHigh[i];
            float low = pLow[i];
            float open = high * (1 - round(pOC[i] / 10000.) / 10000.);
            float close = high * (1 - (int(pOC[i]) % 10000) / 10000.);

            chart.addCandle(high, low, open, close);
        }
        chart.mergeCandles();


        Stroke stroke;
        stroke.scan(chart.chanCandles);

        unsigned int index = 0;
        for (int i = 0; i < dataLen; i++) {
            if (i != stroke.points[index].position) {
                pOut[i] = 0;
            }
            else {
                pOut[i] = stroke.points[index].bullish;

                index++;
                if (index >= stroke.points.size()) {
                    index = 0;
                }
            }
        }
    }
}

//=============================================================================
// 输出函数2号：输出段的端点标准画法
//=============================================================================
void Func2(int dataLen, float* pOut, float* pIn, float* pHigh, float* pLow)
{
    findTypicalTrends(dataLen, pOut, pIn, pHigh, pLow);
}

//=============================================================================
// 输出函数3号：输出段的端点1+1终结画法
//=============================================================================
void Func3(int dataLen, float* pOut, float* pIn, float* pHigh, float* pLow)
{
    findTrends(dataLen, pOut, pIn, pHigh, pLow);
}

//=============================================================================
// 输出函数4号：中枢高点数据
//HR:= ROUND((H / C - 1) * 10000) * 10000;
//LR:= ROUND((1 - L / C) * 10000);
//HL:= HR + LR;
//=============================================================================
void Func4(int dataLen, float* pOut, float* pEnds, float* pClose, float* pHL)
{
    if (dataLen < 10) {
        for (int i = 0; i < dataLen; i++) {
            pOut[i] = 0;
        }
    }
    else {
        Chart chart;
        for (int i = 0; i < dataLen; i++) {
            float close = pClose[i];
            float high = close * (1 + round(pHL[i] / 10000.) / 10000.);
            float low = close * (1 - (int(pHL[i]) % 10000) / 10000.);
            float open = (high + low) / 2;   // fake open

            chart.addCandle(high, low, open, close);
        }

        Wave wave;
        wave.createTrends(chart.rawCandles, pEnds, dataLen);
        wave.scanForShakeChannels();

        for (unsigned int i = 0; i < wave.shakes.size(); i++) {
            for (int pos = wave.shakes[i].start; pos < wave.shakes[i].end + 1; pos++) {
                pOut[pos] = wave.shakes[i].high;
            }
        }
    }
}

//=============================================================================
// 输出函数5号：中枢低点数据
//=============================================================================
void Func5(int dataLen, float* pOut, float* pEnds, float* pClose, float* pHL)
{
    if (dataLen < 10) {
        for (int i = 0; i < dataLen; i++) {
            pOut[i] = 0;
        }
    }
    else {
        Chart chart;
        for (int i = 0; i < dataLen; i++) {
            float close = pClose[i];
            float high = close * (1 + round(pHL[i] / 10000.) / 10000.);
            float low = close * (1 - (int(pHL[i]) % 10000) / 10000.);
            float open = (high + low) / 2;   // fake open

            chart.addCandle(high, low, open, close);
        }

        Wave wave;
        wave.createTrends(chart.rawCandles, pEnds, dataLen);
        wave.scanForShakeChannels();

        for (unsigned int i = 0; i < wave.shakes.size(); i++) {
            for (int pos = wave.shakes[i].start; pos < wave.shakes[i].end + 1; pos++) {
                pOut[pos] = wave.shakes[i].low;
            }
        }
    }
}

//=============================================================================
// 输出函数6号：中枢起点、终点信号
//=============================================================================
void Func6(int dataLen, float* pOut, float* pEnds, float* pClose, float* pHL)
{
    if (dataLen < 10) {
        for (int i = 0; i < dataLen; i++) {
            pOut[i] = 0;
        }
    }
    else {
        Chart chart;
        for (int i = 0; i < dataLen; i++) {
            float close = pClose[i];
            float high = close * (1 + round(pHL[i] / 10000.) / 10000.);
            float low = close * (1 - (int(pHL[i]) % 10000) / 10000.);
            float open = (high + low) / 2;   // fake open

            chart.addCandle(high, low, open, close);
        }

        Wave wave;
        wave.createTrends(chart.rawCandles, pEnds, dataLen);
        wave.scanForShakeChannels();

        for (unsigned int i = 0; i < wave.shakes.size(); i++) {
            pOut[wave.shakes[i].start] = 1;
            pOut[wave.shakes[i].end] = 1;
        }
    }
}


//=============================================================================
// 输出函数7号：选股指标
//=============================================================================
void Func7(int dataLen, float* pOut, float* pClose, float* pHL, float* pIn)
{
    if (dataLen < 10) {
        for (int i = 0; i < dataLen; i++) {
            pOut[i] = 0;
        }
    }
    else {
        Chart chart;
        for (int i = 0; i < dataLen; i++) {
            float close = pClose[i];
            float high = close * (1 + round(pHL[i] / 10000.) / 10000.);
            float low = close * (1 - (int(pHL[i]) % 10000) / 10000.);
            float open = (high + low) / 2;   // fake open

            chart.addCandle(high, low, open, close);
        }
        chart.mergeCandles();

        Stroke stroke;
        stroke.scan(chart.chanCandles);

        vector<int> points(dataLen, 0);
    	for (auto point: stroke.points)
    	{
            points[point.position] = point.bullish;
    	}

        vector<int> signals = exceedEarlyHigh(chart.rawCandles, points);
    	for (int i = 0; i < dataLen; i++)
    	{
            pOut[i] = signals[i];
    	}
    }
}


//=============================================================================
// 输出函数8号：选股指标
//=============================================================================
void Func8(int dataLen, float* pOut, float* pClose, float* pHL, float* pIn)
{
    if (dataLen < 10) {
        for (int i = 0; i < dataLen; i++) {
            pOut[i] = 0;
        }
    }
    else {
        Chart chart;
        for (int i = 0; i < dataLen; i++) {
            float close = pClose[i];
            float high = close * (1 + round(pHL[i] / 10000.) / 10000.);
            float low = close * (1 - (int(pHL[i]) % 10000) / 10000.);
            float open = (high + low) / 2;   // fake open

            chart.addCandle(high, low, open, close);
        }
        chart.mergeCandles();

        Stroke stroke;
        stroke.scan(chart.chanCandles);

        vector<int> points(dataLen, 0);
        for (auto point : stroke.points)
        {
            points[point.position] = point.bullish;
        }

        vector<int> signals = reachEarlyLow(chart.rawCandles, points);
        for (int i = 0; i < dataLen; i++)
        {
            pOut[i] = signals[i];
        }
    }
}



//加载的函数
PluginTCalcFuncInfo g_CalcFuncSets[] =
{
    {1,(pPluginFUNC)&Func1},
    {2,(pPluginFUNC)&Func2},
    {3,(pPluginFUNC)&Func3},
    {4,(pPluginFUNC)&Func4},
    {5,(pPluginFUNC)&Func5},
    {6,(pPluginFUNC)&Func6},
    {7,(pPluginFUNC)&Func7},
    {8,(pPluginFUNC)&Func8},
    {0,NULL},
};

//导出给TCalc的注册函数
BOOL RegisterTdxFunc(PluginTCalcFuncInfo** pFun)
{
    if (*pFun == NULL)
    {
        (*pFun) = g_CalcFuncSets;
        return TRUE;
    }
    return FALSE;
}
