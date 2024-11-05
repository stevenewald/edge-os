#include "drivers/buttons.hpp"

namespace edge::drivers {
ButtonController button_controller;

void handle_callback(int channel)
{
    button_controller.handle_callback(channel);
}
} // namespace edge::drivers
