// Settings: Wi-Fi credentials
#define SECRET_WIFI_SSID        ""               // Replace "eduroam" for other Wi-Fi
#define SECRET_WIFI_PASSWORD    ""          // Wi-Fi or university account password
#define SECRET_WIFI_ANONYMOUSID "wlan@tu-berlin.de"     // Don't change, only used for Eduroam
#define SECRET_WIFI_EDUROAMID   "****@tu-berlin.de" // TU account name, only used for Eduroam

// Settings: MQTT
#define SECRET_MQTT_BROKER   "heide.bastla.net" // Server hostname (FQDN)
#define SECRET_MQTT_PORT     8883                 // Server TLS port
#define SECRET_MQTT_USER     "mse23"           // Server credentials
#define SECRET_MQTT_PASSWORD "goofy"
#define SECRET_MQTT_PREFIX   "cadse"              // Prefix for topic and client ID
#define SECRET_MQTT_YEAR     2023                 // Current year (beginning of course)
#define SECRET_MQTT_BOARDID  21                 // [ID]: Assigned PCB number