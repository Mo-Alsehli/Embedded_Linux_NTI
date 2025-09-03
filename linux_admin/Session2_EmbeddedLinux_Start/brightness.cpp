#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>

const char* max_brightness_pwd = "/sys/devices/pci0000:00/0000:00:02.0/drm/card1/card1-eDP-1/intel_backlight/max_brightness";
const char* brightness_pwd = "/sys/devices/pci0000:00/0000:00:02.0/drm/card1/card1-eDP-1/intel_backlight/brightness";

int max_brightness = 0;

int main(int argc, char* argv[]) {
    std::ifstream file(max_brightness_pwd);

    file >> max_brightness;

    file.close();

    std::ofstream brightness(brightness_pwd);
    if (argc > 1) {
        if (std::stoi(argv[1]) < max_brightness && std::stoi(argv[1]) > 20000) {
            brightness << argv[1];
        }
    } else {
        std::cout << "Please Enter Brightness\n";
    }

    brightness.close();

    return 0;
}