#include "TimeStamp.h"
#include <sys/time.h>
using namespace mymuduo;
const int TimeStamp::kMicroSecondsPerSecond = 1000 * 1000;
TimeStamp::TimeStamp(int64_t microSecondsSinceEpoch)
    :m_microSecondsSinceEpoch(microSecondsSinceEpoch)
{
}

TimeStamp TimeStamp::now()
{
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return TimeStamp((int64_t)tv.tv_sec* kMicroSecondsPerSecond + tv.tv_usec);
}
