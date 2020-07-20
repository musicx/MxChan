#include "pch.h"
#include "FuncReg.h"

#include <vector>

#include "Chart.h"
#include "Stroke.h"
#include "Shake.h"
#include "Indicator.h"

using namespace std;

//=============================================================================
// �������1�ţ�����ʶ��׶˵�
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
// �������2�ţ�����εĶ˵��׼����
//=============================================================================
void Func2(int dataLen, float* pOut, float* pIn, float* pHigh, float* pLow)
{
    findTypicalTrends(dataLen, pOut, pIn, pHigh, pLow);
}

//=============================================================================
// �������3�ţ�����εĶ˵�1+1�սử��
//=============================================================================
void Func3(int dataLen, float* pOut, float* pIn, float* pHigh, float* pLow)
{
    findTrends(dataLen, pOut, pIn, pHigh, pLow);
}

//=============================================================================
// �������4�ţ�����ߵ�����
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
// �������5�ţ�����͵�����
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
// �������6�ţ�������㡢�յ��ź�
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
// �������7�ţ�ѡ��ָ��
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
// �������8�ţ�ѡ��ָ��
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



//���صĺ���
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

//������TCalc��ע�ắ��
BOOL RegisterTdxFunc(PluginTCalcFuncInfo** pFun)
{
    if (*pFun == NULL)
    {
        (*pFun) = g_CalcFuncSets;
        return TRUE;
    }
    return FALSE;
}
