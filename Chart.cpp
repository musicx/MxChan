#include "pch.h"
#include "Chart.h"

using namespace std;

bool contains(Candle x, Candle y) {
    if (x.high >= y.high && x.low <= y.low) {
        return true;
    }
    else {
        return false;
    }
}

bool higher(Candle x, Candle y) {
    if (x.high >= y.high && x.low >= y.low) {
        return true;
    }
    else {
        return false;
    }
}


void Chart::addCandle(float high, float low, float open, float close)
{
    Candle rawCandle;
    rawCandle.high = high;
    rawCandle.low = low;
    rawCandle.open = open;
    rawCandle.close = close;
    if (this->rawCandles.empty()) {
        rawCandle.startPosition = 0;
    }
    else {
        rawCandle.startPosition = this->rawCandles.back().endPosition + 1;
    }
    rawCandle.endPosition = rawCandle.startPosition;
    rawCandle.topPosition = rawCandle.startPosition;
    // 保存原始K线
    this->rawCandles.push_back(rawCandle);

}


//TODO: 
void Chart::mergeCandles() {
    for (int index = 0; index < this->rawCandles.size(); index++) {
        Candle rawCandle = this->rawCandles[index];
        if (this->chanCandles.empty()) {
            this->chanCandles.push_back(rawCandle);
            continue;
        }

        Candle lastCandle = this->chanCandles.back();
        bool bullish;
        if (this->chanCandles.size() > 1) {
            Candle secondCandle = this->chanCandles[this->chanCandles.size() - 2];
            bullish = higher(lastCandle, secondCandle);
        }
        else {
            bullish = !higher(lastCandle, rawCandle);
        }

        if (contains(rawCandle, lastCandle) || contains(lastCandle, rawCandle))
        {
            this->chanCandles.pop_back();
            if (bullish)
            {
                lastCandle.high = max(lastCandle.high, rawCandle.high);
                lastCandle.low = max(lastCandle.low, rawCandle.low);
                lastCandle.close = max(lastCandle.close, rawCandle.close);
                lastCandle.open = max(lastCandle.open, rawCandle.open);
            }
            else
            {
                lastCandle.high = min(lastCandle.high, rawCandle.high);
                lastCandle.low = min(lastCandle.low, rawCandle.low);
                lastCandle.close = min(lastCandle.close, rawCandle.close);
                lastCandle.open = min(lastCandle.open, rawCandle.open);
            }
            if (contains(rawCandle, lastCandle)) {
                lastCandle.topPosition = lastCandle.endPosition + rawCandle.topPosition - rawCandle.startPosition + 1;
            }
            lastCandle.endPosition = rawCandle.endPosition;
            this->chanCandles.push_back(lastCandle);
        }
        else
        {
            this->chanCandles.push_back(rawCandle);
        }

    }
}

