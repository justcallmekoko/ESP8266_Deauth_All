# ESP8266 Deauth All
Arduino code for your ESP8266. Deauthenticate all clients from all access points

## Table of Condiments
  - [How it works](#how-it-works)
  - [Hardware](#hardware)
  - [Setup](#setup)
  - [Issues](#issues)
  - [Trash Code](#trash-code)

## How it works
  1. Scan all channels twice
  2. Clean out any APs from the last scan that did not show up in the current scan
  3. Deauthenticate all devices for one minute by sending broadcast deauth frames from all APs
  4. Repeat
  
## Hardware
  * [WEMOS D1 Mini](https://www.amazon.com/Makerfocus-NodeMcu-Development-ESP8266-Compatible/dp/B01N3P763C/ref=sr_1_3?ie=UTF8&qid=1531324588&sr=8-3&keywords=wemos+d1+mini)

## Setup
1. Add URLs to board manager URLs (File, Preferences, Additional Boards Manager URLs):  
   * `http://arduino.esp8266.com/stable/package_esp8266com_index.json`
   * `http://phpsecu.re/esp8266/package_deauther_index.json`
2. Install ESP8266 boards (Tools, Boards Manager):  
   * `arduino-esp8266-deauther`
   * `esp8266`
3. Copy and paste the [code](https://github.com/the-red-team/ESP8266_Deauth_All/blob/master/deauth_all.ino) into your Arduino IDE
4. Upload

## Issues
Some devices are able to ignore deauth frames sent to broadcast so this may not work against all devices in your area

## Trash Code
So this code is complete garbage, but it works.  
There are a lot of repeats which could have just been used in a function (Searching for known MACs)  
but I said, "Fuck it" and just made this.
