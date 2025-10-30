#pragma once
#include <stdint.h>
#include <string>
class Timestamp
{
public:
    Timestamp()
        : microSecondsSinceEpoch_(0)
    {
    }
    explicit Timestamp(int64_t microSecondsSinceEpochArg)
        : microSecondsSinceEpoch_(microSecondsSinceEpochArg)
    {
    }
    void swap(Timestamp &that)
    {
        std::swap(microSecondsSinceEpoch_, that.microSecondsSinceEpoch_);
    }
    std::string toString() const;
    std::string toFormattedString(bool showMicroseconds = true) const;
    int64_t microSecondsSinceEpoch() const { return microSecondsSinceEpoch_; }
    time_t secondsSinceEpoch() const
    {
        return static_cast<time_t>(microSecondsSinceEpoch_ / kMicroSecondsPerSecond);
    }
    static Timestamp now();
    static Timestamp invalid()
    {
        return Timestamp();
    }
    static Timestamp fromUnixTime(time_t t, int microseconds)
    {
        return Timestamp(static_cast<int64_t>(t) * kMicroSecondsPerSecond + microseconds);
    }
    static Timestamp fromUnixTime(time_t t)
    {
        return fromUnixTime(t, 0);
    }

    static const int kMicroSecondsPerSecond = 1000 * 1000;

private:
    int64_t microSecondsSinceEpoch_;
};