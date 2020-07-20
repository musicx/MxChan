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

/*
void Wave::scanForShakes()
{
	float maxOuterTolerance = 1.236;
	float maxInnerTolerance = 0.09;  // 0.09+0.146=0.236
	if (this->trends.size() < 4) {
		return;
	}
	for (unsigned int i = 0; i < this->trends.size() - 3; i++) {
		bool found = false;
		int highEdgeCount = 0;
		int lowEdgeCount = 0;
		int highHalfCount = 0;
		int lowHalfCount = 0;
		int trendCount = 0;
		Trend currentTrend = this->trends[i];
		Trend markTrend = this->trends[i + 1];
		Trend nextTrend = this->trends[i + 2];

		if (currentTrend.direction > 0 && (nextTrend.high - markTrend.low) / (currentTrend.high - markTrend.low) < maxOuterTolerance ||
			currentTrend.direction < 0 && (markTrend.high - nextTrend.low) / (markTrend.high - currentTrend.low) < maxOuterTolerance) {
			float freezeHigh = 0;
			float freezeLow = 0;
			if (currentTrend.direction > 0) {
				freezeHigh = currentTrend.high;
				freezeLow = markTrend.low;
			}
			else {
				freezeHigh = markTrend.high;
				freezeLow = currentTrend.low;
			}
			trendCount++;
			if (nextTrend.direction > 0 && abs(nextTrend.high - freezeHigh) / (freezeHigh - freezeLow) < maxInnerTolerance) {
				highEdgeCount++;
			}
			if (nextTrend.direction < 0 && abs(freezeLow - nextTrend.low) / (freezeHigh - freezeLow) < maxInnerTolerance) {
				lowEdgeCount++;
			}
			if (nextTrend.low > (freezeHigh + freezeLow) / 2) {
				highHalfCount++;
			}
			if (nextTrend.high < (freezeHigh + freezeLow) / 2) {
				lowHalfCount++;
			}
			unsigned int idx = i + 3;
			while (idx < this->trends.size()) {
				if (this->trends[idx].direction > 0 && (this->trends[idx].high - freezeLow) / (freezeHigh - freezeLow) < maxOuterTolerance ||
					this->trends[idx].direction < 0 && (freezeHigh - this->trends[idx].low) / (freezeHigh - freezeLow) < maxOuterTolerance) {
					found = true;
					trendCount++;
					if (this->trends[idx].direction > 0 && abs(this->trends[idx].high - freezeHigh) / (freezeHigh - freezeLow) < maxInnerTolerance) {
						highEdgeCount++;
					}
					if (this->trends[idx].direction < 0 && abs(freezeLow - this->trends[idx].low) / (freezeHigh - freezeLow) < maxInnerTolerance) {
						lowEdgeCount++;
					}
					if (this->trends[idx].low > (freezeHigh + freezeLow) / 2) {
						highHalfCount++;
					}
					if (this->trends[idx].high < (freezeHigh + freezeLow) / 2) {
						lowHalfCount++;
					}
					idx++;
				}
				else {
					break;
				}
			}
			if (found && highEdgeCount > 0 && lowEdgeCount > 0 && (highHalfCount + lowHalfCount) * 1.0 / trendCount < 0.3) {
				Shake shake;
				shake.high = freezeHigh;
				shake.low = freezeLow;
				shake.start = currentTrend.start;
				shake.end = this->trends[idx - 1].end;
				shake.highEdgeCount = highEdgeCount;
				shake.lowEdgeCount = lowEdgeCount;
				shake.highHalfCount = highHalfCount;
				shake.lowHalfCount = lowHalfCount;
				shake.trendCount = trendCount;
				for (int pos = currentTrend.end; pos >= currentTrend.start; pos--) {
					if (currentTrend.direction > 0 && this->candles[pos].close < freezeLow || 
						currentTrend.direction < 0 && this->candles[pos].close > freezeHigh) {
						shake.start = pos + 1;
					}
				}
				for (int pos = this->trends[idx - 1].start; pos <= this->trends[idx - 1].end; pos++) {
					if (this->candles[pos].close > freezeHigh || this->candles[pos].close < freezeLow) {
						shake.end = pos;
					}
				}
				this->shakes.push_back(shake);
			}
		}
	}
}
*/

// TODO: 000928   000948  overlap problem
void Wave::scanForShakeChannels()
{
	const float maxTolerance = 0.618;
	//float maxInnerTolerance = 0.146;  // 0.09+0.146=0.236
	if (this->trends.size() < 4) {
		return;
	}
	unsigned int base = 0;
	while (base < this->trends.size()) {
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



