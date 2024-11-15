#pragma once

#include "config.hpp"

namespace edge {
class MpuController {
    MpuController() { }

    ~MpuController() { disable_mpu(); };

    void disable_mpu() const;

public:
    void initialize_mpu() const;

    MpuController(const MpuController&) = delete;
    MpuController(MpuController&&) = delete;
    MpuController& operator=(const MpuController&) = delete;
    MpuController& operator=(MpuController&&) = delete;

    static MpuController& get();

    void set_program_stack_start(const unsigned* stack_ptr) const;
};
} // namespace edge
