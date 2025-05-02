# TrioPrint

## Overview
Trioprint is a 3D printing project designed to enhance the capabilities of non-networked 3D printers. By utilizing an ESP32-based extension board and custom software, Trioprint adds features such as remote control, monitoring, and automation, improving the performance and user experience of your 3D printer. Trioprint includes usefull web interface. Files can be uploaded using drag & drop. Print can be monitored in real time and can be paused and stoped. An emergency stop functionality is included in case print fails.

## Features
- **ESP32 Extension Board**: Integrates seamlessly with your 3D printer, enabling network connectivity and remote control.
- **Custom Software**: Offers advanced features like remote monitoring, automation, and real-time status updates.
- **Optimized Printing**: Enhances the overall performance and efficiency of your 3D printer.
- **mDNS** which allows to key the name defined in web browser and connect only with bonjour installed on compute
- **Fail safe mode (Access point)** is enabled if cannot connect to defined station at boot.

> [!WARNING]
>### Disclaimer
> The software is provided 'as is,' without any warranty of any kind, expressed or implied, including but not limited to the warranties of merchantability, fitness for a particular purpose, and non-infringement. In no event shall the authors or copyright holders be liable for any claim, damages, or other liability, whether in an action of contract, tort, or otherwise, arising from, out of, or in connection with the software or the use or other dealings in the software.
>It is essential that you carefully read and understand this disclaimer before using this software and its components. If you do not agree with any part of this disclaimer, please refrain from using the software.  


## File Structure
- `web/`: Web interface files.
  - `main/`: Contains core web files including `index.html`, `index.css`, and `compile.py`.
  - `config/`: Configuration interface files and `compress.py` for web interface setup. This interface is stored localy on the esp thus even if you delete the main html page, you have way to recover.

## Getting Started

1. **Compile Web Interface**:
   - Navigate to the `web/main` folder and run `compile.py` to generate the compiled HTML.
   - Upload the `compiled.html.gz` file to your SD card.

2. **Generate Config Files**:
   - Navigate to `web/config` and run `compress.py` to generate the compiled configuration files.

3. **Hardware Setup**:
   - Insert the SD card into the ESP32 and configure `PIN_SPI_SS` in `config.h` to match the SD card's chip select pin.
   - Set correct EMS and STOP commands in `config.h` default is for Ender 3 v3 SE
   - Flash the firmware to the ESP32 using the Arduino IDE. Ensure you have the necessary libraries installed.

4. **Network Configuration**:
   - Connect to the Wi-Fi network `trioprint` (password: `trioprint`).
   - Access the configuration page via a browser at `192.168.1.1`. If not prompted, navigate to `/server/config`.

5. **Printer Connection**:
   - Connect the ESP32 to the printer's UART port (not USB).
   - Optionally, connect 3.3V and ground to power the ESP32 from the printer when powered on.

6. **Testing and Operation**:
   - Power up the printer and test the setup by uploading a file to the SD card and checking the printer's connectivity at `[IP]/` or `http://[name].local`. You can check ip via serial if serial debug is enabled. (laptop, pc or Android phone can be used).
   - Use a dedicated tablet or any device with a browser as the user interface. Personally I use old ipad. The only place where I allow Apple device to be used.

7. **Updates**:
   - For firmware updates, use the OTA feature or connect via USB to the ESP32 for updates after assembling the printer. Printers own USB port can't be used!

## Dependencies
- [ESP32 Arduino Core](https://github.com/espressif/arduino-esp32): Required for ESP32 development.
- [SdFat Library](https://github.com/greiman/SdFat): For SD card operations.
- [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer): For handling web server requests.

## Contributing
Contributions are welcome! Feel free to open issues or submit pull requests.

## License
This project is licensed under the [MIT License](LICENSE).

## Contact
For questions or support, contact [your-email@example.com].
