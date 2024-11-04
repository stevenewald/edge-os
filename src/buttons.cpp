#include "drivers/buttons.hpp"

namespace edge::drivers {
ButtonController button_controller;

void handle_callback()
{
    button_controller.handle_callback();
}
} // namespace edge::drivers
