# ESP8266 Deauth All
Arduino code for your ESP8266. Deauthenticate all clients from all access points

# Trash Code
So this code is complete garbage, but it works.  
There are a lot of repeats which could have just been used in a function (Searching for known MACs)  
but I said, "Fuck it" and just made this.

# How it works
* Scans all channels twice
* Cleans out any APs from the last scan that did not show up in this scan
* Deauthenticates all for one minute by sending broadcast deauth frames from all APs
* Repeat

# Issues
Some devices are able to ignore deauth frames sent to broadcast so this may not work against all devices in your area
