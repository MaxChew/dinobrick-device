#include <time.h>

// DayTime
int utcOffset = 8;
const char* ntpServer = "time.google.com";
const long gmtOffset_sec = 3600*utcOffset; // UTC+8
const int daylightOffset_sec = 0; // Daylight Saving Time

void setupTime() {
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

String formatTimeAgo(unsigned long diffMinutes) {
  if (diffMinutes < 1)
    return "just now";

  if (diffMinutes < 60)
    return String(diffMinutes) + "m ago";

  if (diffMinutes < 1440)  // 60 * 24
    return String(diffMinutes / 60) + "h ago";

  return String(diffMinutes / 1440) + "d ago";
}