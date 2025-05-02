# Project Name: TrioPrint

## Overview
TrioPrint is a 3D printing project designed to enhance the functionality and usability of 3D printers. It leverages embedded systems and custom firmware to provide advanced features and improved performance.

## Features
- **Custom Firmware**: Tailored for 3D printing applications.
- **Device Integration**: Supports multiple hardware components.
- **Optimized Performance**: Ensures smooth and efficient operation.

## File Structure
- `device.h`: Hardware-specific definitions and configurations.
- `trioprint.ino`: Core logic and application entry point.
- `README.md`: Project documentation.

## Getting Started
1. Navigate to the `web` folder and run `compile.py`.
2. Upload `compiled.html.gz` to the SD card.
3. Connect the SD card to the ESP32 and set `PIN_SPI_SS` in `config.h` to your SD card's chip select pin.
4. Build and flash the firmware to the ESP32.
5. Connect to the WiFi network `trioprint` (password: `trioprint`) and open a browser at `192.168.1.1`.
6. Complete the necessary configuration.
7. Test the setup by uploading a file to the SD card and ensuring all connections are correct.
8. Connect the ESP32 to the printer's UART port (not USB). Locate the serial port on the motherboard and optionally connect 3.3V and ground to power the ESP32 when the printer is powered.
9. Power up the printer and start using TrioPrint. A dedicated tablet can be used as the user interface.
10. For updates, use the OTA feature.

## Dependencies
- [Dependency 1](#): Description of dependency.
- [Dependency 2](#): Description of dependency.

## Contributing
Contributions are welcome! Please follow the [contribution guidelines](CONTRIBUTING.md).

## License
This project is licensed under the [MIT License](LICENSE).

## Contact
For questions or support, please contact [your-email@example.com].
