#include <fstream>
#include <iostream>

class Led {
   private:
    std::fstream file;

   public:
    Led() = delete;
    Led(std::fstream&);
    void turn_capslock_on();
    void turn_capslock_off();

    ~Led();
};