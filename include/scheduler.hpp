#pragma once

namespace edge {

class Scheduler {
    static constexpr auto MAX_PROCESSES = 50;
    static constexpr auto QUANTUM_MILLIS = 10;

    struct process {
        void (*entrypoint)(void);
    };

    etl::vector<process, MAX_PROCESSES> processes_;

public:
private:
    Scheduler() = default;
};
} // namespace edge
