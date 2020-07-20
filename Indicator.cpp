#include "pch.h"
#include "Indicator.h"


vector<int> exceedEarlyHigh(vector<Candle>& candles, vector<int>& points)
{
	vector<int> output(candles.size(), 0);
	bool compare = false;
	int lastHigh = 0;
	int secondHigh = 0;
	int lastLow = 0;
	int secondLow = 0;
	for (int i = 0; i < candles.size(); i++)
	{
		if (compare)
		{
			if (candles[i].close > candles[lastHigh].high)
			{
				output[i] = 1;
				if (candles[lastLow].low > candles[secondLow].low)
				{
					output[i] = 2;
				}
				compare = false;
			}
		}
		if (points[i] > 0)
		{
			secondHigh = lastHigh;
			lastHigh = i;
			compare = false;
		}
		else if (points[i] < 0)
		{
			secondLow = lastLow;
			lastLow = i;
			compare = candles[lastHigh].high >= candles[secondHigh].high || candles[lastLow].low > (candles[secondLow].low - (candles[lastHigh].high - candles[secondLow].low) * 0.5);
		}
		
	}
	return output;
}

vector<int> reachEarlyLow(vector<Candle>& candles, vector<int>& points)
{
	vector<int> output(candles.size(), 0);
	bool compare = false;
	int lastHigh = 0;
	int secondHigh = 0;
	int thirdHigh = 0;
	int lastLow = 0;
	int secondLow = 0;
	int lastDirection = 0;
	for (int i = 0; i < candles.size(); i++)
	{
		if (compare)
		{
			if (candles[i].close > candles[secondHigh].high
				&& candles[i].low < candles[secondHigh].high * 1.03 
				&& candles[i].close > candles[i-1].close)
			{
				output[i] = 1;
			}
			if (candles[i].close < candles[secondHigh].high)
			{
				compare = false;
			}
		}
		if (points[i] > 0)
		{
			lastDirection = 1;
			secondHigh = lastHigh;
			lastHigh = i;
			compare = candles[lastHigh].high > candles[secondHigh].high + (candles[secondHigh].high - candles[lastLow].low) * 0.236;
		}
		else if (points[i] < 0)
		{
			lastDirection = -1;
			secondLow = lastLow;
			lastLow = i;
		}

	}
	return output;
}
