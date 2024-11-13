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
class ProcessCallbackStorage {
public:
    static constexpr uint8_t MAX_READY_CALLBACKS = 10;

private:
    etl::array<
        etl::vector<drivers::subscribe_callback, MAX_READY_CALLBACKS>, MAX_PROCESSES>
        ready_callbacks;

    ProcessCallbackStorage() = default;
    ~ProcessCallbackStorage() = default;

public:
    ProcessCallbackStorage(const ProcessCallbackStorage&) = delete;
    ProcessCallbackStorage(ProcessCallbackStorage&&) = delete;
    ProcessCallbackStorage& operator=(const ProcessCallbackStorage&) = delete;
    ProcessCallbackStorage& operator=(ProcessCallbackStorage&&) = delete;

    void add_ready_callback(
        uint8_t process_id, drivers::SubscribeCallbackPtr callback, int arg1 = 0,
        int arg2 = 0
    );

    etl::optional<drivers::subscribe_callback> get_ready_callback(uint8_t process_id);

    static ProcessCallbackStorage& get();
};
} // namespace edge
