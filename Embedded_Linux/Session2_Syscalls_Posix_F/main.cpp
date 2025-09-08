#include <fstream>

class Led {
   private:
    std::fstream file;

   public:
    Led() = delete;
    Led(std::fstream& f) : file(std::move(f)) {}

    void turn_led_on() { file << 1; }

    void turn_led_off() { file << 0; }

    ~Led() { file.close(); }
};

int main() {
    std::fstream file("brightness");

    Led my_led(file);

    my_led.turn_led_on();

    return 0;
}