#pragma once

#include <chrono>

namespace Time
{

template
<
    typename Clock = std::chrono::high_resolution_clock,
    typename Duration = std::chrono::microseconds
>
class Travel
{
public:
    using TimePoint = typename Clock::time_point;

    Travel() : start{ Clock::now() }
    {
    }

    auto distance()
    {
        return std::chrono::duration_cast<Duration>(Clock::now() - start).count();
    }

    Travel(Travel const&) = delete;
    Travel& operator=(Travel const&) = delete;
    Travel(Travel&&) = delete;
    Travel& operator=(Travel&&) = delete;

private:
    TimePoint start;
};

} // namespace Time
