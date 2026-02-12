#include "Equinox.h"

WebServer server(80);

bool credentials_saved = false;
bool time_synced = false;
unsigned long connect_start_time = 0;
int timezone_offset = 0;
String timezone_name = "";

void handle_root() {
    String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1'><title>Equinox WiFi</title>";
    html += "<style>body{font-family:sans-serif;max-width:350px;margin:40px auto;padding:20px;background:#1a1a2e;color:#eee}";
    html += "h1{color:#00d4ff;text-align:center}label{display:block;margin:10px 0 5px;font-weight:bold}";
    html += "input,select{width:100%;padding:10px;border-radius:5px;border:none;background:#16213e;color:#fff;box-sizing:border-box}";
    html += "button{width:100%;padding:12px;margin-top:15px;background:#00d4ff;border:none;border-radius:5px;color:#fff;font-weight:bold;cursor:pointer}</style></head>";
    html += "<body><h1>WiFi Setup</h1><form method='POST' action='/save'><label>Network Name</label><input type='text' name='ssid' required>";
    html += "<label>Password</label><input type='text' name='pass'>";
    html += "<label>Timezone</label><select name='timezone'>";
    html += "<option value='-12:00'>-12:00 (Baker Island)</option>";
    html += "<option value='-11:00'>-11:00 (Samoa)</option>";
    html += "<option value='-10:00'>-10:00 (Hawaii)</option>";
    html += "<option value='-09:00'>-09:00 (Alaska)</option>";
    html += "<option value='-08:00'>-08:00 (Pacific)</option>";
    html += "<option value='-07:00'>-07:00 (Mountain)</option>";
    html += "<option value='-06:00'>-06:00 (Central)</option>";
    html += "<option value='-05:00'>-05:00 (Eastern)</option>";
    html += "<option value='-04:00'>-04:00 (Atlantic)</option>";
    html += "<option value='-03:00'>-03:00 (Buenos Aires)</option>";
    html += "<option value='-02:00'>-02:00 (Mid-Atlantic)</option>";
    html += "<option value='-01:00'>-01:00 (Azores)</option>";
    html += "<option value='+00:00'>+00:00 (UTC)</option>";
    html += "<option value='+01:00' selected>+01:00 (Paris, Berlin)</option>";
    html += "<option value='+02:00'>+02:00 (Cairo)</option>";
    html += "<option value='+03:00'>+03:00 (Moscow)</option>";
    html += "<option value='+04:00'>+04:00 (Dubai)</option>";
    html += "<option value='+05:00'>+05:00 (Karachi)</option>";
    html += "<option value='+05:30'>+05:30 (Mumbai)</option>";
    html += "<option value='+06:00'>+06:00 (Dhaka)</option>";
    html += "<option value='+07:00'>+07:00 (Bangkok)</option>";
    html += "<option value='+08:00'>+08:00 (Shanghai)</option>";
    html += "<option value='+09:00'>+09:00 (Tokyo)</option>";
    html += "<option value='+10:00'>+10:00 (Sydney)</option>";
    html += "<option value='+11:00'>+11:00 (Solomon)</option>";
    html += "<option value='+12:00'>+12:00 (Auckland)</option>";
    html += "<option value='+13:00'>+13:00 (Samoa)</option>";
    html += "</select><button type='submit'>Save</button></form></body></html>";
    server.send(200, "text/html", html);
}

void handle_save() {
    String ssid = server.arg("ssid");
    String pass = server.arg("pass");
    String timezone = server.arg("timezone");
    if (ssid.length() > 0) {
        pref_set_string("ssid", ssid);
        pref_set_string("pass", pass);
        pref_set_string("timezone", timezone);
        pref_set_int("boot_count", 0);
        server.send(200, "text/html", "<!DOCTYPE html><html><head><meta charset='UTF-8'><title>Saved</title><meta http-equiv='refresh' content='1;url=/'><style>body{font-family:sans-serif;max-width:350px;margin:40px auto;padding:20px;background:#1a1a2e;color:#eee;text-align:center}h1{color:#00ff88}</style></head><body><h1>Saved!</h1><p>Restarting...</p></body></html>");
        delay(100);
        ESP.restart();
        while (1);
    } else {
        server.send(400, "text/html", "<h1>Error</h1>");
    }
}

void sync_time(int timezone_offset) {
    configTime(timezone_offset, 0, "pool.ntp.org", "time.google.com");
    struct tm timeinfo;
    if (getLocalTime(&timeinfo)) {
        Serial.println("Time synced");
        time_synced = true;
        hours = timeinfo.tm_hour;
        minutes = timeinfo.tm_min;
        seconds = timeinfo.tm_sec;
        set_builtin_led(0, 255, 0);
    }
}
