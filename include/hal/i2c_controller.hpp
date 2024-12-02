#pragma once

namespace edge::aidan
{

class I2CController 
{
public:
    ~I2CController();
    static I2CController& get();
private:
    I2CController();
};

} // namespace edge::aidan
