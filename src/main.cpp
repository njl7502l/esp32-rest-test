#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <vector>
#include "Arduino_JSON.h"
#include "pass.h"
#include "wifi.h"
#include "time.h"
#include "yearprd.h"

class GitHubActivity {
public:
  int total;
  std::vector<YearPrd> yearPeriods;

  void sum() {
    total = 0;
    for (uint i = 0; i < yearPeriods.size(); i++) {
      if (yearPeriods[i].updateStatus > 0) {
        total += yearPeriods[i].count;
      }
    }
  }

  void setAggregatePeriods(time_t t, int to_year) {
    struct tm *tm;
    tm = localtime(&t);
    yearPeriods.clear();
    while ((tm->tm_year + 1900) > to_year) {
      YearPrd year(t);
      yearPeriods.push_back(year);
      t -= YearPrd::oneYear;
      tm = localtime(&t);
    }
  }

  void updateAll() {
    for (uint i = 0; i < yearPeriods.size(); i++) {
      yearPeriods[i].update();
      if (yearPeriods[i].updateStatus > 0) {
        Serial.print("contribution: ");
        Serial.println(yearPeriods[i].count);
      }
    }
    sum();
  }

  void updateLastYear() {
    yearPeriods[0].update();
    sum();
  }
} activity;

void setup() {
  Serial.begin(115200);
  delay(3000);

  // wifi setup
  connectWifi();

  // Time setup
  time_t t = getJST();
  activity.setAggregatePeriods(t, 2015);
  activity.updateAll();

  Serial.print("##### Total CONTRIBUTIONS #####: ");
  Serial.println(activity.total);

  delay(500);
}

void loop() {
  activity.updateLastYear();
  Serial.print("contribution: ");
  Serial.println(activity.total);
}
