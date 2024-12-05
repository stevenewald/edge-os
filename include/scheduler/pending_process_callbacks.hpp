#pragma once
#include "config.hpp"
#include "drivers/driver_enums.hpp"

namespace edge {
// One option is to make the drivers and other things non singletons and have the
// scheduler own everything. I'm actually okay with this, but not a big deal rn and this
// works fine
// This doesn't matter rn but would matter if we were worried about shutdown and wanted
// very predictable dtor calls
// TLDR: singleton vs everything owned by scheduler, food for thought
class PendingProcessCallbacks {
public:
    static constexpr uint8_t MAX_READY_CALLBACKS = 10;

private:
    etl::array<
        etl::vector<drivers::subscribe_callback, MAX_READY_CALLBACKS>, MAX_PROCESSES>
        ready_callbacks;

    PendingProcessCallbacks() = default;
    ~PendingProcessCallbacks() = default;

public:
    PendingProcessCallbacks(const PendingProcessCallbacks&) = delete;
    PendingProcessCallbacks(PendingProcessCallbacks&&) = delete;
    PendingProcessCallbacks& operator=(const PendingProcessCallbacks&) = delete;
    PendingProcessCallbacks& operator=(PendingProcessCallbacks&&) = delete;

    void add_ready_callback(
        ProcessId process_id, ProcessCallbackPtr callback, int arg1 = 0, int arg2 = 0
    );

    etl::optional<drivers::subscribe_callback> get_ready_callback(ProcessId process_id);

    static PendingProcessCallbacks& get();
};
} // namespace edge
