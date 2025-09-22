// #include "esp_task_wdt.h"

// Global variables to PNG
PNG png;
fs::File pngfile;
int g_offsetX = 0;
int g_offsetY = 0;

void * pngOpen(const char *filename, int32_t *size) {
  Serial.printf("Attempting to open %s\n", filename);
  pngfile = LittleFS.open(filename, "r");
  *size = pngfile.size();
  return &pngfile;
}

void pngClose(void *handle) {
  fs::File pngfile = *((fs::File*)handle);
  if (pngfile) pngfile.close();
}

int32_t pngRead(PNGFILE *page, uint8_t *buffer, int32_t length) {
  if (!pngfile) return 0;
  page = page; // Avoid warning
  return pngfile.read(buffer, length);
}

int32_t pngSeek(PNGFILE *page, int32_t position) {
  if (!pngfile) return 0;
  page = page; // Avoid warning
  return pngfile.seek(position);
}

void drawPngLine(PNGDRAW *pDraw) {
  // yield(); 
  uint16_t lineBuffer[MAX_IMAGE_WIDTH];
  png.getLineAsRGB565(pDraw, lineBuffer, PNG_RGB565_BIG_ENDIAN, 0xffffffff);
  sprite.pushImage(g_offsetX, g_offsetY + pDraw->y, pDraw->iWidth, 1, lineBuffer);
  // tft.pushImage(g_offsetX, g_offsetY + pDraw->y, pDraw->iWidth, 1, lineBuffer);
  // yield(); 
}

// Downloads the PNG file from the given URL and saves it to LittleFS.
bool downloadPngToLittleFS(const String& url, const char* path) {
  WiFiClientSecure *client = new WiFiClientSecure;
  client->setInsecure(); 

  HTTPClient http;
  http.begin(*client, url);
  int httpCode = http.GET();

  if (httpCode > 0) { // HTTP_CODE_OK is 200
    // Open the destination file on LittleFS for writing
    fs::File file = LittleFS.open(path, "w");
    if (!file) {
      Serial.println("Failed to open file for writing on LittleFS.");
      http.end();
      return false;
    }

    int len = http.getSize();
    const size_t buffSize = 512;  // A medium-size buffer to balance speed and memory
    uint8_t buffer[buffSize];
    int bytesRead = 0;
    WiFiClient* stream = http.getStreamPtr();

    Serial.println("Downloading image and writing to LittleFS...");
    // Read data in chunks until all bytes have been received
    while (http.connected() && (len > 0 || len == -1)) {
      size_t size = stream->available();
      if (size) {
        int c = stream->readBytes(buffer, ((size > buffSize) ? buffSize : size));
        file.write(buffer, c);
        if (len > 0) len -= c;
      }

      delay(1); // Yield to background tasks
    }

    file.close();
    http.end();
    Serial.println("PNG image downloaded and stored in LittleFS.");
    return true;
  } else {
    Serial.print("Failed to download PNG. HTTP error code: ");
    Serial.println(httpCode);
    http.end();
    return false;
  }
}

// Draw PNG icon at specified coordinates
void drawLocalPNG(const char* path, int xpos, int ypos, bool center = false) {
  fs::File pngFile = LittleFS.open(path, "r");

  if (!pngFile) {
    Serial.println(path);
    Serial.println("Failed to open PNG file from LittleFS.");
    return;
  }
  
  g_offsetX = xpos;
  g_offsetY = ypos;

  // The x and y offset determine where the image will be rendered on the display.
  int res = png.open(path, pngOpen, pngClose, pngRead, pngSeek, drawPngLine);
  if (res != PNG_SUCCESS) {
    Serial.printf("Error opening PNG file: %d\n", res);
    pngFile.close();
    return;
  }
  Serial.println("Decoding PNG image...");

  // Temporarily disable the watchdog timer for the current task
  // esp_task_wdt_deinit();
  // esp_task_wdt_init(10, false); // Initialize with a generous timeout and disable panic
  
  res = png.decode(NULL, 0);  // NULL decodes the full image at once.

  // Re-enable the watchdog timer with its original settings
  // esp_task_wdt_init(3, true); // Assuming a default of 3 seconds and panic mode enabled

  if (res != PNG_SUCCESS) {
    Serial.printf("Error decoding PNG: %d\n", res);
  } else {
    Serial.println("PNG image displayed successfully!");
  }
  png.close();
  pngFile.close();
}