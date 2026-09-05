Hardware requirement :
1. x1 ESP32 or anyboard that have WIFI host/connect funtion.
2. x8 Jumper wires.
3. x1 SD card adapter/reader.
4. Anything to cover your hardware (3D printer recommended )

Software requirement :
- Arduino IDE
  Board requirement :
  - ESP32 (ver. 2.0.17 ) Or your board
  
  Libary requirement :
  - AsyncTCP
  - ESPAsyncWebServer

Reminder and problems solve :
if your SD card module use 5V and after upload the code to your board it might not work try change your wire between 5V and 3.3V then try again (when you cannect to PC)
after you use power source from anything but PC you need to change your VCC pin back to 5V

Wiring :
