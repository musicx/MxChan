#include "pch.h"
#include "Indicator.h"

#include <map>
#include "Stroke.h"

bool xlower(Candle x, Candle y) {
	if (x.high <= y.high && x.low <= y.low && x.high + x.low < y.high + y.low) {
		return true;
	}
	return false;
}

vector<int> exceedEarlyHigh(vector<Candle>& candles, vector<int>& points)
{
	vector<int> outputs(candles.size(), 0);
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
				outputs[i] = 1;
				if (candles[lastLow].low > candles[secondLow].low)
				{
					outputs[i] = 2;
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
	return outputs;
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

vector<int> findFracAroundShake(vector<Candle>& candles, vector<Shake>& shakes, int dataLen)
{
	vector<int> outputs(dataLen, 0);
	if (candles.size() < 10 || shakes.empty())
	{
		return outputs;
	}
	int shakeIndex = 0;
	for (int candleIndex = 0; candleIndex < candles.size() - 1; candleIndex++)
	{
		if (shakeIndex >= shakes.size() || candles[candleIndex].topPosition < shakes[shakeIndex].start)
		{
			continue;
		}
		if (shakeIndex < shakes.size() - 1 && candles[candleIndex].endPosition >= shakes[shakeIndex + 1].start)
		{
			shakeIndex++;
		}
		if (xlower(candles[candleIndex], candles[candleIndex - 1]) && xlower(candles[candleIndex], candles[candleIndex + 1]))
		{
			bool hasFrac = false;
			if (candles[candleIndex].low < (shakes[shakeIndex].low + shakes[shakeIndex].high) / 2)
			{
				// 类二及中枢延伸法下沿以下
				outputs[candles[candleIndex + 1].topPosition] = -1;
				hasFrac = true;
			}
			else if (candles[candleIndex].low > shakes[shakeIndex].high)
			{
				// 三买
				outputs[candles[candleIndex + 1].topPosition] = 1;
				hasFrac = true;
			}

			if (hasFrac && candleIndex < candles.size() - 2) 
			{
				if (candles[candleIndex + 2].close > candles[candleIndex + 1].high)
				{
					outputs[candles[candleIndex + 2].topPosition] = 2 * outputs[candles[candleIndex + 1].topPosition];
				}
			}
		}
	}
	return outputs;
}



vector<int> findNegFracAroundShake(vector<Candle>& candles, vector<Shake>& shakes, int dataLen)
{
	vector<int> outputs(dataLen, 0);
	if (candles.size() < 10 || shakes.empty())
	{
		return outputs;
	}
	int shakeIndex = 0;
	for (int candleIndex = 1; candleIndex < candles.size() - 1; candleIndex++)
	{
		if (shakeIndex >= shakes.size() || candles[candleIndex].topPosition < shakes[shakeIndex].start)
		{
			continue;
		}
		if (shakeIndex < shakes.size() - 1 && candles[candleIndex].endPosition >= shakes[shakeIndex + 1].start)
		{
			shakeIndex++;
		}
		if (xlower(candles[candleIndex - 1], candles[candleIndex]) && xlower(candles[candleIndex + 1], candles[candleIndex]))
		{
			bool hasFrac = false;
			if (candles[candleIndex].high < shakes[shakeIndex].low)
			{
				// 三卖
				outputs[candles[candleIndex + 1].endPosition] = 1;
				hasFrac = true;
			}
			else if (candles[candleIndex].high > (shakes[shakeIndex].low + shakes[shakeIndex].high) / 2)
			{
				// 类二卖及中枢延伸法上沿以上
				outputs[candles[candleIndex + 1].endPosition] = -1;
				hasFrac = true;
			}

			if (hasFrac && candleIndex < candles.size() - 2)
			{
				if (candles[candleIndex + 2].close < candles[candleIndex + 1].low)
				{
					outputs[candles[candleIndex + 2].endPosition] = 2 * outputs[candles[candleIndex + 1].endPosition];
				}
			}
		}
	}
	return outputs;
}

vector<int> findZeroDivergence(vector<Candle>& candles, float* pIn, int dataLen)
{
	vector<int> outputs(dataLen, 0);
	vector<int> bullish;
	vector<int> bearish;
	bool upper = true;
	float extreme = 0;
	int extremePos = 0;
	for (int i = 0; i < dataLen; i++)
	{
		if (i == 0)
		{
			if (pIn[i] < 0)
			{
				upper = false;
			}
			extreme = pIn[i];
			continue;
		}
		if (upper)
		{
			if (pIn[i] < 0)
			{
				upper = false;
				extreme = pIn[i];
				bullish.push_back(extremePos);
				extremePos = i;
			}
			else if (pIn[i] > extreme)
			{
				extreme = pIn[i];
				extremePos = i;
			}
		} else
		{
			if (pIn[i] > 0)
			{
				upper = true;
				extreme = pIn[i];
				bearish.push_back(extremePos);
				extremePos = i;
			}
			else if (pIn[i] < extreme)
			{
				extreme = pIn[i];
				extremePos = i;
			}
		}
	}

	const int lookBack = 5;
	for (int i = 1; i < bullish.size(); i++)
	{
		int start = i >= lookBack ? i - lookBack : 0;
		int index = i - 1;
		int current;
		int last = bullish[i];
		int count = 0;
		while (index >= start)
		{
			current = bullish[index];
			if (candles[current].close > candles[last].close)
			{
				break;
			}
			if (pIn[current] > pIn[last] && candles[current].close < candles[last].close)
			{
				last = current;
				count++;
			}
			index--;
		}
		if (count > 0)
		{
			outputs[bullish[i]] = -count;
		}
	}

	for (int i = 1; i < bearish.size(); i++)
	{
		int start = i >= lookBack ? i - lookBack : 0;
		int index = i - 1;
		int current;
		int last = bearish[i];
		int count = 0;
		while (index >= start)
		{
			current = bearish[index];
			if (candles[current].close < candles[last].close)
			{
				break;
			}
			if (pIn[current] < pIn[last] && candles[current].close > candles[last].close)
			{
				last = current;
				count++;
			}
			index--;
		}
		if (count > 0)
		{
			outputs[bearish[i]] = count;
		}
	}
	
	return outputs;
}


vector<int> findRsiDivergence(vector<Candle>& candles, float* pIn, int dataLen)
{
	vector<int> outputs(dataLen, 0);
	vector<int> bearish;
	vector<int> bullish;

	for (int i = 2; i < dataLen - 2; i++)
	{
		if (pIn[i] > pIn[i - 1] && pIn[i] > pIn[i - 2] && pIn[i] > pIn[i + 1] && pIn[i] > pIn[i + 2])
		{
			bearish.push_back(i);
		}

		if (pIn[i] < pIn[i - 1] && pIn[i] < pIn[i - 2] && pIn[i] < pIn[i + 1] && pIn[i] < pIn[i + 2])
		{
			bullish.push_back(i);
		}
	}

	if (pIn[dataLen - 2] > pIn[dataLen - 3] && pIn[dataLen - 2] > pIn[dataLen - 4] && pIn[dataLen - 2] > pIn[dataLen - 1])
	{
		bearish.push_back(dataLen - 2);
	}
	if (pIn[dataLen - 2] < pIn[dataLen - 3] && pIn[dataLen - 2] < pIn[dataLen - 4] && pIn[dataLen - 2] < pIn[dataLen - 1])
	{
		bullish.push_back(dataLen - 2);
	}

	if (pIn[dataLen - 1] > pIn[dataLen - 2] && pIn[dataLen - 1] > pIn[dataLen - 3] && pIn[dataLen - 1] > pIn[dataLen - 4])
	{
		bearish.push_back(dataLen - 1);
	}
	if (pIn[dataLen - 1] < pIn[dataLen - 2] && pIn[dataLen - 1] < pIn[dataLen - 3] && pIn[dataLen - 1] < pIn[dataLen - 4])
	{
		bullish.push_back(dataLen - 1);
	}

	for (int i = bearish.size() - 1; i > 0; i--)
	{
		int check = i - 1;
		float highPrice = candles[bearish[i]].high;
		float highInd = pIn[bearish[i]];
		while (check >= 0 && bearish[i] - bearish[check] < 120)
		{
			highPrice = candles[bearish[check]].high > highPrice ? candles[bearish[check]].high : highPrice;
			highInd = pIn[bearish[check]] > highInd ? pIn[bearish[check]] : highInd;
			if (candles[bearish[check]].high * 1.02 < candles[bearish[i]].high && pIn[bearish[check]] > pIn[bearish[i]] + 3 &&
				max(candles[bearish[check]].high, candles[bearish[i]].high) >= highPrice && max(pIn[bearish[check]], pIn[bearish[i]]) >= highInd)
			{
				outputs[bearish[i]] = bearish[i] - bearish[check];
				break;
			}
			check--;
		}
	}

	for (int i = bullish.size() - 1; i > 0; i--)
	{
		int check = i - 1;
		float lowPrice = candles[bullish[i]].low;
		float lowInd = pIn[bullish[i]];
		while (check >= 0 && bullish[i] - bullish[check] < 120)
		{
			lowPrice = candles[bullish[check]].low < lowPrice ? candles[bullish[check]].low : lowPrice;
			lowInd = pIn[bullish[check]] < lowInd ? pIn[bullish[check]] : lowInd;
			if (candles[bullish[check]].low * 0.98 > candles[bullish[i]].low && pIn[bullish[check]] < pIn[bullish[i]] - 3 &&
				min(candles[bullish[check]].low, candles[bullish[i]].low) <= lowPrice && min(pIn[bullish[check]], pIn[bullish[i]]) <= lowInd)
			{
				outputs[bullish[i]] = bullish[check] - bullish[i];
				break;
			}
			check--;
		}
	}
	
	return outputs;
}


vector<int> findCrossDivergence(float* pPrice, float* pFast, float* pSlow, int dataLen)
{
	vector<int> outputs(dataLen, 0);
	vector<int> bullish;
	vector<int> bearish;

	for(int i = 0; i < dataLen - 1; i++)
	{
		if (pFast[i] < pSlow[i] && pFast[i+1] > pSlow[i+1])
		{
			bullish.push_back(i);
		}
		if (pFast[i] > pSlow[i] && pFast[i+1] < pSlow[i+1])
		{
			bearish.push_back(i);
		}
	}

	for (int i = bullish.size() - 1; i > 0; i--)
	{
		int check = i - 1;
		float lowInd = pFast[bullish[i]];
		float lowPrice = pPrice[bullish[i]];
		while (check >= 0 && bullish[i] - bullish[check] < 120)
		{
			lowInd = pFast[bullish[check]] < lowInd ? pFast[bullish[check]] : lowInd;
			lowPrice = pPrice[bullish[check]] < lowPrice ? pPrice[bullish[check]] : lowPrice;
			if (pPrice[bullish[check]] * 0.98 > pPrice[bullish[i]] && pFast[bullish[check]] < pFast[bullish[i]] &&
				min(pFast[bullish[check]], pFast[bullish[i]]) <= lowInd && min(pPrice[bullish[check]], pPrice[bullish[i]]) <= lowPrice)
			{
				outputs[bullish[i]+1] = bullish[i] - bullish[check];
				break;
			}
			check--;
		}
	}

	for (int i = bearish.size() - 1; i > 0; i--)
	{
		int check = i - 1;
		float highInd = pFast[bearish[i]];
		float highPrice = pPrice[bearish[i]];
		while (check >= 0 && bearish[i] - bearish[check] < 120)
		{
			highInd = pFast[bearish[check]] > highInd ? pFast[bearish[check]] : highInd;
			highPrice = pPrice[bearish[check]] > highPrice ? pPrice[bearish[check]] : highPrice;
			if (pPrice[bearish[check]] * 1.02 < pPrice[bearish[i]] && pFast[bearish[check]] > pFast[bearish[i]] &&
				max(pFast[bearish[check]], pFast[bearish[i]]) >= highInd && max(pPrice[bearish[check]], pPrice[bearish[i]]) >= highPrice)
			{
				outputs[bearish[i]+1] = bearish[check] - bearish[i];
				break;
			}
			check--;
		}
	}

	return outputs;
}




vector<int> findSingleLineDivergence(float* pPrice, float* pInd, int dataLen)
{
	vector<int> outputs(dataLen, 0);
	vector<int> bearish;
	vector<int> bullish;

	for (int i = 1; i < dataLen - 1; i++)
	{
		if (pInd[i] > pInd[i - 1] && pInd[i] > pInd[i + 1])
		{
			bearish.push_back(i);
		}

		if (pInd[i] < pInd[i - 1] && pInd[i] < pInd[i + 1])
		{
			bullish.push_back(i);
		}
	}

	if (pInd[dataLen - 1] > pInd[dataLen - 2] && pInd[dataLen - 1] > pInd[dataLen - 3])
	{
		bearish.push_back(dataLen - 1);
	}
	if (pInd[dataLen - 1] < pInd[dataLen - 2] && pInd[dataLen - 1] < pInd[dataLen - 3])
	{
		bullish.push_back(dataLen - 1);
	}

	for (int i = bearish.size() - 1; i > 0; i--)
	{
		int check = i - 1;
		float highPrice = pPrice[bearish[i]];
		float highInd = pInd[bearish[i]];
		while (check >= 0 && bearish[i] - bearish[check] < 120)
		{
			highPrice = pPrice[bearish[check]] > highPrice ? pPrice[bearish[check]] : highPrice;
			highInd = pInd[bearish[check]] > highInd ? pInd[bearish[check]] : highInd;
			if (pPrice[bearish[check]] * 1.003 < pPrice[bearish[i]] && pInd[bearish[check]] > pInd[bearish[i]] &&
				max(pPrice[bearish[check]], pPrice[bearish[i]]) >= highPrice && max(pInd[bearish[check]], pInd[bearish[i]]) >= highInd)
			{
				outputs[bearish[i]] = bearish[i] - bearish[check];
				break;
			}
			check--;
		}
	}

	for (int i = bullish.size() - 1; i > 0; i--)
	{
		int check = i - 1;
		float lowPrice = pPrice[bullish[i]];
		float lowInd = pInd[bullish[i]];
		while (check >= 0 && bullish[i] - bullish[check] < 120)
		{
			lowPrice = pPrice[bullish[check]] < lowPrice ? pPrice[bullish[check]] : lowPrice;
			lowInd = pInd[bullish[check]] < lowInd ? pInd[bullish[check]] : lowInd;
			if (pPrice[bullish[check]] * 0.997 > pPrice[bullish[i]] && pInd[bullish[check]] < pInd[bullish[i]] &&
				min(pPrice[bullish[check]], pPrice[bullish[i]]) <= lowPrice && min(pInd[bullish[check]], pInd[bullish[i]]) <= lowInd)
			{
				outputs[bullish[i]] = bullish[check] - bullish[i];
				break;
			}
			check--;
		}
	}

	return outputs;
}

vector<float> findPivotLines(Chart chart)
{
	Stroke stroke;
	stroke.scanForStrokes(chart.chanCandles);
	int startPosition = max(0, chart.rawCandles.size() - 500);
	map<int, int> ranges;
	for (auto point : stroke.points)
	{
		if (point.position < startPosition)
		{
			continue;
		}
		float high = chart.chanCandles[point.index].high;
		float low = chart.chanCandles[point.index].low;
		int lastIndex = max(0, point.index - 1);
		int nextIndex = min(chart.chanCandles.size() - 1, point.index + 1);
		if (point.bullish > 0)
		{
			low = max(max(chart.chanCandles[lastIndex].open, chart.chanCandles[lastIndex].close),
				max(chart.chanCandles[nextIndex].open, chart.chanCandles[nextIndex].close));
		}
		else
		{
			high = min(min(chart.chanCandles[lastIndex].open, chart.chanCandles[lastIndex].close),
				min(chart.chanCandles[nextIndex].open, chart.chanCandles[nextIndex].close));
		}
		if (low < high)
		{
			for (int x = int(low * 100); x <= int(high * 100); x++)
			{
				if (ranges.find(x) != ranges.end())
				{
					ranges[x] += 1;
				}
				else
				{
					ranges[x] = 1;
				}
			}
		}
	}

	bool countTrend = false;
	int priceSum = 0;
	int priceCount = 0;
	int currentCount = 0;
	vector<float> highFreq;
	for (auto& item : ranges)
	{
		if (item.second != currentCount)
		{
			if (currentCount != 0 && item.second < currentCount && countTrend)
			{
				int average = priceSum / priceCount;
				highFreq.push_back(average / 100.);
				countTrend = false;
			}
			if (item.second > currentCount)
			{
				countTrend = true;
			}
			currentCount = item.second;
			priceSum = 0;
			priceCount = 0;
		}
		priceSum += item.first;
		priceCount++;
	}

	return highFreq;
}



