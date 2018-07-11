# ESP8266 Deauth All
Arduino code for your ESP8266. Deauthenticate all clients from all access points

# How it works
  1. Scans all channels twice
  2. Cleans out any APs from the last scan that did not show up in this scan
  3. Deauthenticates all for one minute by sending broadcast deauth frames from all APs
  4. Repeat

# Setup
1. Add URLs to board manager URLs (File, Preferences, Additional Boards Manager URLs):  
  * `http://arduino.esp8266.com/stable/package_esp8266com_index.json`
  * `http://phpsecu.re/esp8266/package_deauther_index.json`
2. Install ESP8266 boards (Tools, Boards Manager):  
  - `arduino-esp8266-deauther`
  - `esp8266`
3. Copy and paste the code into your Arduino IDE

# Issues
Some devices are able to ignore deauth frames sent to broadcast so this may not work against all devices in your area

# Trash Code
So this code is complete garbage, but it works.  
There are a lot of repeats which could have just been used in a function (Searching for known MACs)  
but I said, "Fuck it" and just made this.
