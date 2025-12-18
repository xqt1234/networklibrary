#pragma once
#include <stdlib.h>
#include <string>
namespace mymuduo
{

    class TimeStamp
    {
    private:
        int64_t m_microSecondsSinceEpoch;

    public:
        explicit TimeStamp(int64_t microSecondsSinceEpoch = 0);
        ~TimeStamp() = default;
        static TimeStamp now();
        int64_t microSinceEpoch() const { return m_microSecondsSinceEpoch; }
    };

    inline bool operator==(const TimeStamp &t1, const TimeStamp &t2)
    {
        return t1.microSinceEpoch() == t2.microSinceEpoch();
    }
}