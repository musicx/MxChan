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
        stroke.scanForStrokes(chart.chanCandles);

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
// �������2�ţ�������׷��εı�־
//=============================================================================
void Func2(int dataLen, float* pOut, float* pHigh, float* pLow, float* pOC)
{
    if (dataLen < 10) {
        for (int i = 0; i < dataLen; i++) {
            pOut[i] = 0;
        }
    }
    else {
        Chart chart;
        for (int i = 0; i < dataLen; i++) {
            float high = pHigh[i];
            float low = pLow[i];
            float open = high * (1 - round(pOC[i] / 10000.) / 10000.);
            float close = high * (1 - (int(pOC[i]) % 10000) / 10000.);
            chart.addCandle(high, low, open, close);
        }
        chart.mergeCandles();

        Stroke stroke;
        stroke.scanForFractals(chart.chanCandles);

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
// �������3�ţ�����εĶ˵��׼����
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
        wave.scanForLongShakes();

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
        wave.scanForLongShakes();

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
        wave.scanForLongShakes();

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
        stroke.scanForStrokes(chart.chanCandles);

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
// �������8�ţ�ѡ��ָ��, �����й����¼��������ϵĵ׷��ͼ�ͣ��
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
        stroke.scanForStrokes(chart.chanCandles);

        vector<float> points(dataLen, 0);
        for (auto point : stroke.points)
        {
            points[point.position] = point.bullish;
        }

        Wave wave;
        wave.createTrends(chart.rawCandles, &points[0], dataLen);
        wave.scanForLongShakes();
        
        vector<int> signals = findFracAroundShake(chart.chanCandles, wave.shakes, dataLen);
        for (int i = 0; i < dataLen; i++)
        {
            pOut[i] = signals[i];
        }
    }
}



//=============================================================================
// �������9�ţ�ѡ��ָ�꣬�����й����ϼ��������µĶ����ͼ�ͣ��
//=============================================================================
void Func9(int dataLen, float* pOut, float* pClose, float* pHL, float* pIn)
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
        stroke.scanForStrokes(chart.chanCandles);

        vector<float> points(dataLen, 0);
        for (auto point : stroke.points)
        {
            points[point.position] = point.bullish;
        }

        Wave wave;
        wave.createTrends(chart.rawCandles, &points[0], dataLen);
        wave.scanForLongShakes();

        vector<int> signals = findNegFracAroundShake(chart.chanCandles, wave.shakes, dataLen);
        for (int i = 0; i < dataLen; i++)
        {
            pOut[i] = signals[i];
        }
    }
}



//=============================================================================
// �������10�ţ�ָ�걳��
//=============================================================================
void Func10(int dataLen, float* pOut, float* pClose, float* pHL, float* pIn)
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
        
        vector<int> signals = findZeroDivergence(chart.rawCandles, pIn, dataLen);
        for (int i = 0; i < dataLen; i++)
        {
            pOut[i] = signals[i];
        }
    }
}




//=============================================================================
// �������11��: �ʵĵ׷��͵ĵ׷��ͣ��������
//OOR:= ROUND((1 - O / H) * 10000) * 10000;
//CCR:= ROUND((1 - C / H) * 10000);
//OC:= OOR + CCR;
//=============================================================================
void Func11(int dataLen, float* pOut, float* pHigh, float* pLow, float* pOC)
{
    if (dataLen < 10) {
        for (int i = 0; i < dataLen; i++) {
            pOut[i] = 0;
        }
    }
    else {
        Chart chart;
        for (int i = 0; i < dataLen; i++) {
            float high = pHigh[i];
            float low = pLow[i];
            float open = high * (1 - round(pOC[i] / 10000.) / 10000.);
            float close = high * (1 - (int(pOC[i]) % 10000) / 10000.);

            chart.addCandle(high, low, open, close);
        }
        chart.mergeCandles();

        Stroke stroke;
        stroke.scanForStrokes(chart.chanCandles);

        vector<int> outputs(dataLen, 0);
    	for (int i = 4; i < stroke.points.size(); i++)
    	{
    		if (stroke.points[i].bullish < 0 && chart.rawCandles[stroke.points[i - 4].position].low > chart.rawCandles[stroke.points[i - 2].position].low &&
                chart.rawCandles[stroke.points[i - 2].position].low < chart.rawCandles[stroke.points[i].position].low)
    		{
                outputs[stroke.points[i].position] = 1;

                float rise = chart.rawCandles[stroke.points[i - 1].position].high - chart.rawCandles[stroke.points[i - 2].position].low;
    			float fall = chart.rawCandles[stroke.points[i - 1].position].high - chart.rawCandles[stroke.points[i].position].low;
                if (fall < rise * 0.618)
                {
                    outputs[stroke.points[i].position] = 2;
                }
    		}
    	}

    	for (int i = 0; i < dataLen; i++)
    	{
            pOut[i] = outputs[i];
    	}
    	
    }
}


//=============================================================================
// �������12��: �������ϱ�������������±��սᣬ�������
//OOR:= ROUND((1 - O / H) * 10000) * 10000;
//CCR:= ROUND((1 - C / H) * 10000);
//OC:= OOR + CCR;
//=============================================================================
void Func12(int dataLen, float* pOut, float* pHigh, float* pLow, float* pOC)
{
    if (dataLen < 10) {
        for (int i = 0; i < dataLen; i++) {
            pOut[i] = 0;
        }
    }
    else {
        Chart chart;
        for (int i = 0; i < dataLen; i++) {
            float high = pHigh[i];
            float low = pLow[i];
            float open = high * (1 - round(pOC[i] / 10000.) / 10000.);
            float close = high * (1 - (int(pOC[i]) % 10000) / 10000.);

            chart.addCandle(high, low, open, close);
        }
        chart.mergeCandles();
    	
        Stroke stroke;
        stroke.scanForStrokes(chart.chanCandles);

        vector<float> points(dataLen, 0);
        for (auto point : stroke.points)
        {
            points[point.position] = point.bullish;
        }

        Wave wave;
        wave.createTrends(chart.rawCandles, &points[0], dataLen);
        wave.scanForLongShakes();

        int shakeIndex = 0;
        vector<int> outputs(dataLen, 0);
        for (int i = 2; i < stroke.points.size(); i++)
        {
            while (shakeIndex < wave.shakes.size() && wave.shakes[shakeIndex].end < stroke.points[i - 2].position)
        	{
                shakeIndex++;
        	}
            if (stroke.points[i].bullish < 0 && chart.rawCandles[stroke.points[i - 2].position].low < chart.rawCandles[stroke.points[i].position].low &&
                shakeIndex < wave.shakes.size() && wave.shakes[shakeIndex].high < chart.rawCandles[stroke.points[i].position].low)
            {
                outputs[stroke.points[i].position] = 1;
            }
        }

        for (int i = 0; i < dataLen; i++)
        {
            pOut[i] = outputs[i];
        }

    }
}


//=============================================================================
// �������13��: �������µͱ�, һ�����, ��δ���Ǳ���
//OOR:= ROUND((1 - O / H) * 10000) * 10000;
//CCR:= ROUND((1 - C / H) * 10000);
//OC:= OOR + CCR;
//=============================================================================
void Func13(int dataLen, float* pOut, float* pHigh, float* pLow, float* pOC)
{
    if (dataLen < 10) {
        for (int i = 0; i < dataLen; i++) {
            pOut[i] = 0;
        }
    }
    else {
        Chart chart;
        for (int i = 0; i < dataLen; i++) {
            float high = pHigh[i];
            float low = pLow[i];
            float open = high * (1 - round(pOC[i] / 10000.) / 10000.);
            float close = high * (1 - (int(pOC[i]) % 10000) / 10000.);

            chart.addCandle(high, low, open, close);
        }
        chart.mergeCandles();

        Stroke stroke;
        stroke.scanForStrokes(chart.chanCandles);

        vector<int> outputs(dataLen, 0);
        for (int i = 4; i < stroke.points.size(); i++)
        {
            if (stroke.points[i].bullish < 0 && chart.rawCandles[stroke.points[i - 4].position].low > chart.rawCandles[stroke.points[i].position].low &&
                chart.rawCandles[stroke.points[i - 2].position].low > chart.rawCandles[stroke.points[i].position].low)
            {
                outputs[stroke.points[i].position] = 1;
            }
        }

        for (int i = 0; i < dataLen; i++)
        {
            pOut[i] = outputs[i];
        }

    }
}


//=============================================================================
// �������14��: �ߵ͸��ټӻƽ�ָ����ϣ�������
//OOR:= ROUND((1 - O / H) * 10000) * 10000;
//CCR:= ROUND((1 - C / H) * 10000);
//OC:= OOR + CCR;
//=============================================================================
void Func14(int dataLen, float* pOut, float* pHigh, float* pLow, float* pOC)
{
    if (dataLen < 10) {
        for (int i = 0; i < dataLen; i++) {
            pOut[i] = 0;
        }
    }
    else {
        Chart chart;
        for (int i = 0; i < dataLen; i++) {
            float high = pHigh[i];
            float low = pLow[i];
            float open = high * (1 - round(pOC[i] / 10000.) / 10000.);
            float close = high * (1 - (int(pOC[i]) % 10000) / 10000.);

            chart.addCandle(high, low, open, close);
        }
        chart.mergeCandles();

        Stroke stroke;
        stroke.scanForStrokes(chart.chanCandles);

        vector<int> outputs(dataLen, 0);
        for (int i = 6; i < stroke.points.size(); i++)
        {
            if (stroke.points[i].bullish < 0 && chart.rawCandles[stroke.points[i - 6].position].low > chart.rawCandles[stroke.points[i - 4].position].low &&
                chart.rawCandles[stroke.points[i - 4].position].low < chart.rawCandles[stroke.points[i - 2].position].low &&
                chart.rawCandles[stroke.points[i - 3].position].high > chart.rawCandles[stroke.points[i].position].low && 
                chart.rawCandles[stroke.points[i].position].low > chart.rawCandles[stroke.points[i - 4].position].low)
            {
                float rise = chart.rawCandles[stroke.points[i - 3].position].high - chart.rawCandles[stroke.points[i - 4].position].low;
                float riseAgain = chart.rawCandles[stroke.points[i - 1].position].high - chart.rawCandles[stroke.points[i - 4].position].low;
                float fall = chart.rawCandles[stroke.points[i - 3].position].high - chart.rawCandles[stroke.points[i].position].low;
            	if (riseAgain > rise * 1.618)
            	{
                    continue;
                }
            	if (fall < rise * 0.382)
                {
                    outputs[stroke.points[i].position] = 4;
                }
                else if (fall < rise * 0.5)
                {
                    outputs[stroke.points[i].position] = 3;
                }
                else if (fall < rise * 0.618)
                {
                    outputs[stroke.points[i].position] = 2;
                }
                else
                {
                    outputs[stroke.points[i].position] = 1;
                }
            }
        }

        for (int i = 0; i < dataLen; i++)
        {
            pOut[i] = outputs[i];
        }

    }
}



//=============================================================================
// �������15��: �ɱʵ׷��κ���һ���׷��ͣ���һ�׷��δ��׷���ͣ��
//OOR:= ROUND((1 - O / H) * 10000) * 10000;
//CCR:= ROUND((1 - C / H) * 10000);
//OC:= OOR + CCR;
//=============================================================================
void Func15(int dataLen, float* pOut, float* pHigh, float* pLow, float* pOC)
{
    if (dataLen < 10) {
        for (int i = 0; i < dataLen; i++) {
            pOut[i] = 0;
        }
    }
    else {
        Chart chart;
        for (int i = 0; i < dataLen; i++) {
            float high = pHigh[i];
            float low = pLow[i];
            float open = high * (1 - round(pOC[i] / 10000.) / 10000.);
            float close = high * (1 - (int(pOC[i]) % 10000) / 10000.);

            chart.addCandle(high, low, open, close);
        }
        chart.mergeCandles();

        Stroke stroke;
        stroke.scanForStrokes(chart.chanCandles);

        Stroke frac;
        frac.scanForFractals(chart.chanCandles);
    	
        // �׷���ͣ�ٱ�1����֤�׷��μ�2
        vector<int> outputs(dataLen, 0);
        int fracIndex = 0;
        for (int i = 0; i < stroke.points.size(); i++)
        {
            int bottomStatus = 0;
	        if (stroke.points[i].bullish < 0 && stroke.points[i].index < chart.chanCandles.size() - 2)
	        {
                float checkHigh = chart.chanCandles[stroke.points[i].index + 1].high;
                int start = chart.chanCandles[stroke.points[i].index + 2].startPosition;
	        	int end = chart.chanCandles[stroke.points[i].index + 2].endPosition;
	        	for (int ci = start; ci <= end; ci++)
	        	{
	        		if (chart.rawCandles[ci].close > checkHigh)
	        		{
                        outputs[ci] = 1;
                        bottomStatus++;
                        break;
	        		}
	        	}
	        } else
	        {
                continue;
	        }
            int strokeEnd = dataLen;
        	if (i < stroke.points.size() - 1)
        	{
                strokeEnd = stroke.points[i + 1].position;
        	}
            int fracCount = 0;
        	while (fracIndex < frac.points.size())
        	{
        		if (frac.points[fracIndex].position <= stroke.points[i].position || frac.points[fracIndex].bullish > 0)
        		{
                    fracIndex++;
                    continue;
        		}
        		if (frac.points[fracIndex].position > strokeEnd)
        		{
                    break;
        		}
                if (frac.points[fracIndex].index < chart.chanCandles.size() - 1 && fracCount < 2) {
                    outputs[chart.chanCandles[frac.points[fracIndex].index + 1].startPosition] = bottomStatus + 2;
                    fracCount++;
                }
                fracIndex++;
        	}
        	
        }

        for (int i = 0; i < dataLen; i++)
        {
            pOut[i] = outputs[i];
        }
    }
}



//=============================================================================
// �������16��: �����������ص�����
//OOR:= ROUND((1 - O / H) * 10000) * 10000;
//CCR:= ROUND((1 - C / H) * 10000);
//OC:= OOR + CCR;
//=============================================================================
void Func16(int dataLen, float* pOut, float* pHigh, float* pLow, float* pOC)
{
    if (dataLen < 10) {
        for (int i = 0; i < dataLen; i++) {
            pOut[i] = 0;
        }
    }
    else {
        Chart chart;
        for (int i = 0; i < dataLen; i++) {
            float high = pHigh[i];
            float low = pLow[i];
            float open = high * (1 - round(pOC[i] / 10000.) / 10000.);
            float close = high * (1 - (int(pOC[i]) % 10000) / 10000.);

            chart.addCandle(high, low, open, close);
        }
        chart.mergeCandles();

        Stroke stroke;
        stroke.scanForStrokes(chart.chanCandles);
    	
        vector<float> points(dataLen, 0);
        for (auto point : stroke.points)
        {
            points[point.position] = point.bullish;
        }

        Wave wave;
        wave.createTrends(chart.rawCandles, &points[0], dataLen);
        wave.scanForLongShakes();

        int shakeIndex = 0;
        vector<int> outputs(dataLen, 0);

        if (!wave.shakes.empty()) {
            for (int i = 1; i < dataLen; i++)
            {
                while (shakeIndex < wave.shakes.size() - 1 && wave.shakes[shakeIndex + 1].start < i)
                {
                    shakeIndex++;
                }
                if (chart.rawCandles[i].close > wave.shakes[shakeIndex].high && chart.rawCandles[i - 1].close < wave.shakes[shakeIndex].high)
                {
                    outputs[i] = 1;
                }
                if (outputs[i - 1] == 1 && chart.rawCandles[i].close > chart.rawCandles[i - 1].close)
                {
                    outputs[i] = 2;
                }
            }
        }
    	
        for (int i = 0; i < dataLen; i++)
        {
            pOut[i] = outputs[i];
        }
    }
}




//=============================================================================
// �������17�ţ�RSIָ�걳��
//=============================================================================
void Func17(int dataLen, float* pOut, float* pClose, float* pHL, float* pIn)
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

        vector<int> signals = findRsiDivergence(chart.rawCandles, pIn, dataLen);
        for (int i = 0; i < dataLen; i++)
        {
            pOut[i] = signals[i];
        }
    }
}




//=============================================================================
// �������18�ţ�����ߵ�����-������
//HR:= ROUND((H / C - 1) * 10000) * 10000;
//LR:= ROUND((1 - L / C) * 10000);
//HL:= HR + LR;
//=============================================================================
void Func18(int dataLen, float* pOut, float* pEnds, float* pClose, float* pHL)
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
        wave.scanForShrinkShakes();

        for (unsigned int i = 0; i < wave.shakes.size(); i++) {
            for (int pos = wave.shakes[i].start; pos < wave.shakes[i].end + 1; pos++) {
                pOut[pos] = wave.shakes[i].high;
            }
        }
    }
}

//=============================================================================
// �������19�ţ�����͵�����-������
//=============================================================================
void Func19(int dataLen, float* pOut, float* pEnds, float* pClose, float* pHL)
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
        wave.scanForShrinkShakes();

        for (unsigned int i = 0; i < wave.shakes.size(); i++) {
            for (int pos = wave.shakes[i].start; pos < wave.shakes[i].end + 1; pos++) {
                pOut[pos] = wave.shakes[i].low;
            }
        }
    }
}

//=============================================================================
// �������20�ţ�������㡢�յ��ź� - ������
//=============================================================================
void Func20(int dataLen, float* pOut, float* pEnds, float* pClose, float* pHL)
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
        wave.scanForShrinkShakes();

        for (unsigned int i = 0; i < wave.shakes.size(); i++) {
            pOut[wave.shakes[i].start] = 1;
            pOut[wave.shakes[i].end] = 1;
        }
    }
}



//=============================================================================
// �������21��: �������ϱ�������������±��սᣬ������� - ������
//OOR:= ROUND((1 - O / H) * 10000) * 10000;
//CCR:= ROUND((1 - C / H) * 10000);
//OC:= OOR + CCR;
//=============================================================================
void Func21(int dataLen, float* pOut, float* pHigh, float* pLow, float* pOC)
{
    if (dataLen < 10) {
        for (int i = 0; i < dataLen; i++) {
            pOut[i] = 0;
        }
    }
    else {
        Chart chart;
        for (int i = 0; i < dataLen; i++) {
            float high = pHigh[i];
            float low = pLow[i];
            float open = high * (1 - round(pOC[i] / 10000.) / 10000.);
            float close = high * (1 - (int(pOC[i]) % 10000) / 10000.);

            chart.addCandle(high, low, open, close);
        }
        chart.mergeCandles();

        Stroke stroke;
        stroke.scanForStrokes(chart.chanCandles);

        vector<float> points(dataLen, 0);
        for (auto point : stroke.points)
        {
            points[point.position] = point.bullish;
        }

        Wave wave;
        wave.createTrends(chart.rawCandles, &points[0], dataLen);
        wave.scanForShrinkShakes();

        int shakeIndex = 0;
        vector<int> outputs(dataLen, 0);
        for (int i = 2; i < stroke.points.size(); i++)
        {
            while (shakeIndex < wave.shakes.size() && wave.shakes[shakeIndex].end < stroke.points[i - 2].position)
            {
                shakeIndex++;
            }
            if (stroke.points[i].bullish < 0 && chart.rawCandles[stroke.points[i - 2].position].low < chart.rawCandles[stroke.points[i].position].low &&
                shakeIndex < wave.shakes.size() && wave.shakes[shakeIndex].high < chart.rawCandles[stroke.points[i].position].low)
            {
                outputs[stroke.points[i].position] = 1;
            }
        }

        for (int i = 0; i < dataLen; i++)
        {
            pOut[i] = outputs[i];
        }

    }
}


//=============================================================================
// �������22�ţ������߽���ָ�걳��
//=============================================================================
void Func22(int dataLen, float* pOut, float* pPrice, float* pFast, float* pSlow)
{
    if (dataLen < 10) {
        for (int i = 0; i < dataLen; i++) {
            pOut[i] = 0;
        }
    }
    else {
        vector<int> signals = findCrossDivergence(pPrice, pFast, pSlow, dataLen);
        for (int i = 0; i < dataLen; i++)
        {
            pOut[i] = signals[i];
        }
    }
}


//=============================================================================
// �������23�ţ��˲���
//=============================================================================
void Func23(int dataLen, float* pfOUT, float* pIn, float* pParam, float* pVoid)
{
    for (int i = 0; i < dataLen; i++) {
        float alpha = pParam[i];
        if (i == 0) {
            pfOUT[i] = pIn[i];
        }
        else if (i == 1) {
            pfOUT[i] = alpha * pIn[i] + (1 - alpha) * pIn[i - 1];
        }
        else {
            pfOUT[i] = (alpha - alpha * alpha / 4.0) * pIn[i] + (alpha * alpha / 2.0) * pIn[i - 1] - (alpha - 0.75 * alpha * alpha) * pIn[i - 2] + 2 * (1 - alpha) * pfOUT[i - 1] - (1 - alpha) * (1 - alpha) * pfOUT[i - 2];
        }
    }
}


//=============================================================================
// �������24�ţ��˲���
//=============================================================================
void Func24(int dataLen, float* pOut, float* pIn, float* pParam, float* pTrend)
{
    vector<float> filts(dataLen, 0);
    vector<float> dma(dataLen, 0);
    for (int i = 0; i < dataLen; i++) {
        float length = pParam[i];
        float useTrend = pTrend[i] > 0;
        float a = exp(-1.414 * 3.14159 / (0.5 * length));
        float b = 2 * a * cos(1.414 * 180 / (0.5 * length));
        float c = 1 - b + a * a;
    	if (i == 0)
    	{
            filts[i] = c * pIn[i];
    	}
        else if (i == 1)
        {
            filts[i] = 0.5 * c * (pIn[i] + pIn[i - 1]) + b * filts[i - 1];
        }
        else
        {
            filts[i] = 0.5 * c * (pIn[i] + pIn[i - 1]) + b * filts[i - 1] - a * a * filts[i - 2];
        }
    	if (i <= length)
    	{
            continue;
    	}
        float slope = (filts[i - length] - filts[i]) / length;
        float deltaSum = 0;
    	for (int j = 1; j <= length; j++)
    	{
    		if (useTrend)
    		{
                deltaSum += filts[i] - filts[i - j];
    		}
            else
            {
	            deltaSum += filts[i] - filts[i - j] + j * slope;
            }
    	}
        deltaSum = deltaSum / length;
        dma[i] = 0.04 * deltaSum * deltaSum + 0.96 * dma[i - 1];
        pOut[i] = (dma[i] != 0) ? deltaSum / sqrt(dma[i]) : 0;
    }
}


//=============================================================================
// �������25�ţ�����ָ�걳��
//=============================================================================
void Func25(int dataLen, float* pOut, float* pPrice, float* pInd, float* pVoid)
{
    if (dataLen < 10) {
        for (int i = 0; i < dataLen; i++) {
            pOut[i] = 0;
        }
    }
    else {
        
        vector<int> signals = findSingleLineDivergence(pPrice, pInd, dataLen);
        for (int i = 0; i < dataLen; i++)
        {
            pOut[i] = signals[i];
        }
    }
}


//=============================================================================
// �������26��: �ϳ�����Ļز�
//OOR:= ROUND((1 - O / H) * 10000) * 10000;
//CCR:= ROUND((1 - C / H) * 10000);
//OC:= OOR + CCR;
//=============================================================================
void Func26(int dataLen, float* pOut, float* pHigh, float* pLow, float* pOC)
{
    if (dataLen < 10) {
        for (int i = 0; i < dataLen; i++) {
            pOut[i] = 0;
        }
    }
    else {
        Chart chart;
        for (int i = 0; i < dataLen; i++) {
            float high = pHigh[i];
            float low = pLow[i];
            float open = high * (1 - round(pOC[i] / 10000.) / 10000.);
            float close = high * (1 - (int(pOC[i]) % 10000) / 10000.);

            chart.addCandle(high, low, open, close);
        }
        chart.mergeCandles();

        Stroke stroke;
        stroke.scanForStrokes(chart.chanCandles);

        vector<float> points(dataLen, 0);
        for (auto point : stroke.points)
        {
            points[point.position] = point.bullish;
        }

        Wave wave;
        wave.createTrends(chart.rawCandles, &points[0], dataLen);
        wave.scanForLongShakes();

        int shakeIndex = 0;
        vector<int> outputs(dataLen, 0);
        for (int i = 2; i < stroke.points.size(); i++)
        {
            while (shakeIndex < wave.shakes.size() && wave.shakes[shakeIndex].end < stroke.points[i - 2].position)
            {
                shakeIndex++;
            }
            if (stroke.points[i].bullish < 0 && chart.rawCandles[stroke.points[i - 2].position].low < chart.rawCandles[stroke.points[i].position].low &&
                shakeIndex < wave.shakes.size() && wave.shakes[shakeIndex].high < chart.rawCandles[stroke.points[i].position].low)
            {
                outputs[stroke.points[i].position] = 1;
            }
        }

        for (int i = 0; i < dataLen; i++)
        {
            pOut[i] = outputs[i];
        }

    }
}


//=============================================================================
// �������27��: ��ǰ��״̬
//OOR:= ROUND((1 - O / H) * 10000) * 10000;
//CCR:= ROUND((1 - C / H) * 10000);
//OC:= OOR + CCR;
//=============================================================================
void Func27(int dataLen, float* pOut, float* pHigh, float* pLow, float* pOC)
{
    if (dataLen < 10) {
        for (int i = 0; i < dataLen; i++) {
            pOut[i] = 0;
        }
    }
    else {
        Chart chart;
        for (int i = 0; i < dataLen; i++) {
            float high = pHigh[i];
            float low = pLow[i];
            float open = high * (1 - round(pOC[i] / 10000.) / 10000.);
            float close = high * (1 - (int(pOC[i]) % 10000) / 10000.);

            chart.addCandle(high, low, open, close);
        }
        chart.mergeCandles();

        Stroke stroke;
        stroke.scanForStrokes(chart.chanCandles);

        int index = 0;
        int direction = stroke.points[0].bullish;
        int nextPos = stroke.points[0].position;
        float finalClose = 0;
        for (int i = 0; i < dataLen; i++) {
            if (i < nextPos)
            {
                pOut[i] = direction;
            }
        	else if (i == nextPos)
        	{
                pOut[i] = direction;
                index += 1;
        		if (index < stroke.points.size())
        		{
                    direction = stroke.points[index].bullish;
                    nextPos = stroke.points[index].position;
        		}
                else if (finalClose == 0)
                {
                    finalClose = chart.rawCandles[nextPos].close;
                }
        	}
            else if (i > nextPos)
            {
	            if (chart.rawCandles[i].close > finalClose)
	            {
                    pOut[i] = 1;
	            }
            	else
	            {
                    pOut[i] = -1;
	            }
            }
        }

    }
}


//=============================================================================
// �������28��: ��ǰ�߶�״̬
//OOR:= ROUND((1 - O / H) * 10000) * 10000;
//CCR:= ROUND((1 - C / H) * 10000);
//OC:= OOR + CCR;
//=============================================================================
void Func28(int dataLen, float* pOut, float* pHigh, float* pLow, float* pOC)
{
    if (dataLen < 10) {
        for (int i = 0; i < dataLen; i++) {
            pOut[i] = 0;
        }
    }
    else {
        Chart chart;
        for (int i = 0; i < dataLen; i++) {
            float high = pHigh[i];
            float low = pLow[i];
            float open = high * (1 - round(pOC[i] / 10000.) / 10000.);
            float close = high * (1 - (int(pOC[i]) % 10000) / 10000.);

            chart.addCandle(high, low, open, close);
        }
        chart.mergeCandles();

        Stroke stroke;
        stroke.scanForStrokes(chart.chanCandles);

        Stroke trend;
        trend.scanForTrends(chart.rawCandles, stroke.points);
    	    	
        int index = 0;
        int direction = trend.points[0].bullish;
        int nextPos = trend.points[0].position;
        for (int i = 0; i < dataLen; i++) {
            if (i < nextPos)
            {
                pOut[i] = direction;
            }
            else if (i == nextPos)
            {
                pOut[i] = direction;
                index += 1;
                if (index < trend.points.size())
                {
                    direction = trend.points[index].bullish;
                    nextPos = trend.points[index].position;
                }
            }
            else if (i > nextPos)
            {
                pOut[i] = direction;
            }
        }

    }
}

//=============================================================================
// �������29��: ��ǰ����״̬ -- ������
//OOR:= ROUND((1 - O / H) * 10000) * 10000;
//CCR:= ROUND((1 - C / H) * 10000);
//OC:= OOR + CCR;
//=============================================================================
void Func29(int dataLen, float* pOut, float* pHigh, float* pLow, float* pOC)
{
    if (dataLen < 10) {
        for (int i = 0; i < dataLen; i++) {
            pOut[i] = 0;
        }
    }
    else {
        Chart chart;
        for (int i = 0; i < dataLen; i++) {
            float high = pHigh[i];
            float low = pLow[i];
            float open = high * (1 - round(pOC[i] / 10000.) / 10000.);
            float close = high * (1 - (int(pOC[i]) % 10000) / 10000.);

            chart.addCandle(high, low, open, close);
        }
        chart.mergeCandles();

        Stroke stroke;
        stroke.scanForStrokes(chart.chanCandles);

        vector<float> points(dataLen, 0);
        for (auto point : stroke.points)
        {
            points[point.position] = point.bullish;
        }

        Wave wave;
        wave.createTrends(chart.rawCandles, &points[0], dataLen);
        wave.scanForShrinkShakes();

        int shakeIndex = 0;
        vector<int> outputs(dataLen, 0);
    	
        for (int i = 0; i < dataLen; i++)
        {
            if (shakeIndex < wave.shakes.size() - 1 && wave.shakes[shakeIndex].end < i)
            {
                shakeIndex++;
            }
            if (chart.rawCandles[i].close < wave.shakes[shakeIndex].low)
            {
                outputs[i] = -1;
            }
            else if (chart.rawCandles[i].close > wave.shakes[shakeIndex].high)
            {
                outputs[i] = 1;
            }
            else
            {
                outputs[i] = 0;
            }
        }

        for (int i = 0; i < dataLen; i++)
        {
            pOut[i] = outputs[i];
        }

    }
}


//=============================================================================
// �������30��: �ؼ���λ
//OOR:= ROUND((1 - O / H) * 10000) * 10000;
//CCR:= ROUND((1 - C / H) * 10000);
//OC:= OOR + CCR;
//=============================================================================
void Func30(int dataLen, float* pOut, float* pHigh, float* pLow, float* pOC)
{
    if (dataLen < 10) {
        for (int i = 0; i < dataLen; i++) {
            pOut[i] = 0;
        }
    }
    else {
        Chart chart;
        for (int i = 0; i < dataLen; i++) {
            float high = pHigh[i];
            float low = pLow[i];
            float open = high * (1 - round(pOC[i] / 10000.) / 10000.);
            float close = high * (1 - (int(pOC[i]) % 10000) / 10000.);

            chart.addCandle(high, low, open, close);
        }
        chart.mergeCandles();

        vector<float> pivotLines = findPivotLines(chart);
        int numLines = pivotLines.size();
        float lastClose = chart.rawCandles[dataLen - 1].close;
        if (numLines > 0) {
            int nearest = 0;
            float minimum = abs(pivotLines[0] - lastClose);
            for (int i = 1; i < numLines; i++)
            {
                float diff = abs(pivotLines[i] - lastClose);
                if (diff < minimum)
                {
                    minimum = diff;
                    nearest = i;
                }
            }

            vector<int> indexes;
            if (nearest == 0)
            {
                for (int i = 0; i < numLines; i++)
                {
                    indexes.push_back(i);
                }
            }
            else if (nearest == numLines - 1)
            {
                for (int i = numLines - 1; i >= 0; i--)
                {
                    indexes.push_back(i);
                }
            }
            else
            {
                bool direction = lastClose > pivotLines[nearest];
                indexes.push_back(nearest);
                int upper = nearest + 1;
                int lower = nearest - 1;
                while (upper < numLines || lower >= 0)
                {
                    if (direction)
                    {
                        if (upper < numLines)
                        {
                            indexes.push_back(upper);
                        }
                        if (lower >= 0)
                        {
                            indexes.push_back(lower);
                        }
                    }
                    else
                    {
                        if (lower >= 0)
                        {
                            indexes.push_back(lower);
                        }
                        if (upper < numLines)
                        {
                            indexes.push_back(upper);
                        }
                    }
                    upper++;
                    lower--;
                }
            }

            for (int i = 0; i < indexes.size(); i++)
            {
                int fbound = dataLen - i * (30 + 1);
                int pbound = dataLen - (i + 1) * (30 + 1) + 1;
                if (fbound > 0)
                {
                    for (int j = max(0, pbound); j < fbound; j++)
                    {
                        pOut[j] = pivotLines[indexes[i]];
                    }
                }
            }
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
    {9,(pPluginFUNC)&Func9},
    {10,(pPluginFUNC)&Func10},
    {11,(pPluginFUNC)&Func11},
    {12,(pPluginFUNC)&Func12},
    {13,(pPluginFUNC)&Func13},
    {14,(pPluginFUNC)&Func14},
    {15,(pPluginFUNC)&Func15},
    {16,(pPluginFUNC)&Func16},
    {17,(pPluginFUNC)&Func17},
    {18,(pPluginFUNC)&Func18},
    {19,(pPluginFUNC)&Func19},
    {20,(pPluginFUNC)&Func20},
    {21,(pPluginFUNC)&Func21},
    {22,(pPluginFUNC)&Func22},
    {23,(pPluginFUNC)&Func23},
    {24,(pPluginFUNC)&Func24},
    {25,(pPluginFUNC)&Func25},
    {26,(pPluginFUNC)&Func26},
	{27,(pPluginFUNC)&Func27},
    {28,(pPluginFUNC)&Func28},
    {29,(pPluginFUNC)&Func29},
    {30,(pPluginFUNC)&Func30},
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
