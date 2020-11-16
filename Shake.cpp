#include "pch.h"
#include "Shake.h"

void Wave::createTrends(vector<Candle>& candles, float* pEnds, int nCount)
{
	bool startNewTrend = true;
	Trend currentTrend;
	for (int i = 0; i < nCount; i++) {
		if (startNewTrend) {
			Trend newTrend;
			newTrend.start = i == 0 ? i : (i - 1);
			currentTrend = newTrend;
			startNewTrend = false;
		}
		if (pEnds[i] > 0 || pEnds[i] < 0) {
			currentTrend.end = i;
			currentTrend.direction = pEnds[i] > 0 ? 1 : -1;
			if (currentTrend.start == 0) {
				currentTrend.high = candles[currentTrend.end].high;
				currentTrend.low = candles[0].low;
			}
			else {
				currentTrend.high = currentTrend.direction > 0 ? candles[currentTrend.end].high : candles[currentTrend.start].high;
				currentTrend.low = currentTrend.direction > 0 ? candles[currentTrend.start].low : candles[currentTrend.end].low;
			}
			this->trends.push_back(currentTrend);
			startNewTrend = true;
		} 
	}
	this->candles = candles;
}

void Wave::scanForSimpleShakes()
{
	float tolerance = 0.5;
	int index = 1;
	while (index < this->trends.size() - 2)
	{
		Trend pastTrend = this->trends[index - 1];
		Trend currentTrend = this->trends[index];
		Trend nextTrend = this->trends[index + 2];
		if (pastTrend.high- pastTrend.low < currentTrend.high - currentTrend.low)
		{
			// 前笔更短，起始点可能是买卖点，放弃
			index++;
			continue;
		}
		float shakeHigh = min(currentTrend.high, nextTrend.high);
		float shakeLow = max(currentTrend.low, nextTrend.low);
		float shakeSpan = shakeHigh - shakeLow;
		if (shakeLow >= shakeHigh)
		{
			// 无重叠，过滤
			index++;
			continue;
		}
		Shake shake;
		shake.high = shakeHigh;
		shake.low = shakeLow;
		shake.start = currentTrend.start + 1;
		shake.end = nextTrend.end - 1;

		int checkIndex = index + 3;
		while (checkIndex < this->trends.size())
		{
			Trend checkTrend = this->trends[checkIndex];
			float checkSpan = checkTrend.high - checkTrend.low;
			float overlap = min(shakeHigh - checkTrend.low, checkTrend.high - shakeLow);
			if (overlap <= 0 || overlap < shakeSpan * tolerance || overlap < checkSpan * tolerance)
			{
				// 笔较中枢过大或过小，不加入当前中枢
				break;
			}
			shake.end = checkTrend.end - 1;
			checkIndex++;
		}

		this->shakes.push_back(shake);
		index = checkIndex;
	}
}


void Wave::scanForLongShakes()
{
	int index = 1;
	while (index < this->trends.size() - 2)
	{
		Trend pastTrend = this->trends[index - 1];
		Trend currentTrend = this->trends[index];
		Trend nextTrend = this->trends[index + 2];
		if (pastTrend.high - pastTrend.low < currentTrend.high - currentTrend.low)
		{
			// 前笔更短，起始点可能是买卖点，放弃
			index++;
			continue;
		}
		float shakeHigh = min(currentTrend.high, nextTrend.high);
		float shakeLow = max(currentTrend.low, nextTrend.low);
		if (shakeLow >= shakeHigh)
		{
			// 无重叠，过滤
			index++;
			continue;
		}
		Shake shake;
		shake.high = shakeHigh;
		shake.low = shakeLow;
		shake.start = currentTrend.start + 1;
		shake.end = nextTrend.end - 1;
		shake.counts = 3;

		int checkIndex = index + 3;
		while (checkIndex < this->trends.size())
		{
			Trend checkTrend = this->trends[checkIndex];
			if (checkTrend.low > shakeHigh || checkTrend.high < shakeLow)
			{
				// 出现三买三卖，中枢结束
				break;
			}
			shake.end = checkTrend.end - 1;
			shake.counts++;
			checkIndex++;
		}

		this->shakes.push_back(shake);
		index = checkIndex;
	}
	for (int i = this->shakes.size() - 1; i >= 0; i--)
	{
		if (this->shakes[i].counts < 3)
		{
			this->shakes.erase(this->shakes.begin() + i);
		}
	}
}



void Wave::scanForShrinkShakes()
{
	int index = 1;
	while (index < this->trends.size() - 2)
	{
		Trend pastTrend = this->trends[index - 1];
		Trend currentTrend = this->trends[index];
		Trend nextTrend = this->trends[index + 2];
		if (pastTrend.high - pastTrend.low < currentTrend.high - currentTrend.low)
		{
			// 前笔更短，起始点可能是买卖点，放弃
			index++;
			continue;
		}
		float shakeHigh = min(currentTrend.high, nextTrend.high);
		float shakeLow = max(currentTrend.low, nextTrend.low);
		if (shakeLow >= shakeHigh)
		{
			// 无重叠，过滤
			index++;
			continue;
		}
		Shake shake;
		shake.high = shakeHigh;
		shake.low = shakeLow;
		shake.start = currentTrend.start + 1;
		shake.end = nextTrend.end - 1;
		shake.counts = 3;

		int checkIndex = index + 3;
		while (checkIndex < this->trends.size())
		{
			Trend checkTrend = this->trends[checkIndex];
			if (checkTrend.low > shake.high || checkTrend.high < shake.low)
			{
				// 出现三买三卖，中枢结束
				break;
			}
			if ((shakeLow <= 4 && shakeHigh < shakeLow + 0.005) || (shakeLow > 4 && shakeLow <= 10 && shakeHigh < shakeLow + 0.02) || (shakeLow > 10 && shakeHigh < shakeLow * 1.0025))
			{
				break;
			}
			shakeHigh = (checkTrend.high < shake.high) ? checkTrend.high : shake.high;
			shakeLow = (checkTrend.low > shake.low) ? checkTrend.low : shake.low;
			if (shakeHigh > shakeLow * 1.01)
			{
				shake.high = shakeHigh;
				shake.low = shakeLow;
			} 
			shake.end = checkTrend.end - 1;
			shake.counts++;
			checkIndex++;
		}

		this->shakes.push_back(shake);
		index = checkIndex;
	}
	for (int i = this->shakes.size() - 1; i >= 0; i--)
	{
		if (this->shakes[i].counts < 3)
		{
			this->shakes.erase(this->shakes.begin() + i);
		}
	}
}




void Wave::scanForShakeChannels()
{
	const float maxTolerance = 0.618;
	//float maxInnerTolerance = 0.146;  // 0.09+0.146=0.236
	if (this->trends.size() < 4) {
		return;
	}
	unsigned int base = 0;
	while (base < this->trends.size() - 2) {
		Trend currentTrend = this->trends[base];
		const Trend markTrend = this->trends[base + 1];
		Trend nextTrend = this->trends[base + 2];

		float channelHigh = min(currentTrend.high, nextTrend.high);
		float channelLow = max(currentTrend.low, nextTrend.low);
		if (channelHigh <= channelLow) {
			base++;
			continue;
		}
		if (currentTrend.direction > 0 && abs(currentTrend.high - nextTrend.high) / (channelHigh - channelLow) < maxTolerance && (channelHigh - markTrend.low) / (channelHigh - channelLow) < (1 + maxTolerance) ||
			currentTrend.direction < 0 && abs(currentTrend.low - nextTrend.low) / (channelHigh - channelLow) < maxTolerance && (markTrend.high - channelLow) / (channelHigh - channelLow) < (1 + maxTolerance)) {
			vector<float> highs = { markTrend.high };
			vector<float> lows = { markTrend.low };
			if (currentTrend.direction > 0) {
				highs.push_back(nextTrend.high);
			}
			else {
				lows.push_back(nextTrend.low);
			}
			unsigned int front = base + 3;
			while (front < this->trends.size()) {
				Trend frontTrend = this->trends[front];
				if (frontTrend.direction > 0 && abs(frontTrend.high - channelHigh) / (channelHigh - channelLow) < maxTolerance) {
					channelHigh = min(channelHigh, frontTrend.high);
					bool qualify = true;
					for (auto high : highs) {
						if ((high - channelHigh) / (channelHigh - channelLow) > maxTolerance) {
							qualify = false;
						}
					}
					if (qualify) {
						highs.push_back(frontTrend.high);
						front++;
					}
					else {
						break;
					}
				}
				else if (frontTrend.direction < 0 && abs(frontTrend.low - channelLow) / (channelHigh - channelLow) < maxTolerance) {
					channelLow = max(channelLow, frontTrend.low);
					bool qualify = true;
					for (auto low : lows) {
						if ((channelLow - low) / (channelHigh - channelLow) > maxTolerance) {
							qualify = false;
						}
					}
					if (qualify) {
						lows.push_back(frontTrend.low);
						front++;
					}
					else {
						break;
					}
				}
				else {
					break;
				}
			}
			if (highs.size() + lows.size() > 3) {
				channelHigh = markTrend.high;
				channelLow = markTrend.low;
				for (auto high : highs) {
					if (high < channelHigh) {
						channelHigh = high;
					}
				}
				for (auto low : lows) {
					if (low > channelLow) {
						channelLow = low;
					}
				}

				Shake shake;
				shake.high = channelHigh;
				shake.low = channelLow;
				shake.start = markTrend.start;
				shake.end = this->trends[front - 1].end;

				bool found = false;
				for (int pos = currentTrend.end; pos >= currentTrend.start; pos--) {
					if (currentTrend.direction > 0 && this->candles[pos].close < channelLow ||
						currentTrend.direction < 0 && this->candles[pos].close > channelHigh) {
						shake.start = pos + 1;
						found = true;
						break;
					}
				}
				if (!found) {
					shake.start = currentTrend.start;
				}
				found = false;
				if (front < this->trends.size()) {
					for (int pos = this->trends[front].start; pos <= this->trends[front].end; pos++) {
						if (this->candles[pos].close > channelHigh || this->candles[pos].close < channelLow) {
							shake.end = pos;
							found = true;
							break;
						}
					}
					if (!found) {
						shake.end = this->trends[front].end;
					}
				}
				this->shakes.push_back(shake);
				base = front - 1;
			}
			else {
				base++;
			}
		}
		else {
			base++;
		}

	}
	// merge continous shakes 
	int shakeIndex = 1;
	while (shakeIndex < this->shakes.size()) {
		Shake pastShake = this->shakes[shakeIndex - 1];
		Shake thisShake = this->shakes[shakeIndex];

		if (pastShake.end >= thisShake.start && 
			((pastShake.high > thisShake.low && pastShake.low < thisShake.low) || (pastShake.high > thisShake.high && pastShake.low < thisShake.high))) {
			this->shakes.erase(this->shakes.begin() + shakeIndex - 1, this->shakes.begin() + shakeIndex + 1);
			pastShake.end = thisShake.end;
			pastShake.high = min(pastShake.high, thisShake.high);
			pastShake.low = max(pastShake.low, thisShake.low);
			this->shakes.insert(this->shakes.begin() + shakeIndex - 1, pastShake);
		}
		else {
			shakeIndex++;
		}
	}
}



