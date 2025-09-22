#define INTER_LIGHT_14 "inter_light_14"
#define INTER_REGULAR_16 "inter_regular_16"
#define INTER_SEMIBOLD_24 "inter_semibold_24"
#define INTER_BOLD_48 "inter_bold_48"

#define MAX_IMAGE_WIDTH 240 

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <TFT_eSPI.h>
#include <PNGdec.h>
#include <FS.h>
#include <LittleFS.h>
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite sprite = TFT_eSprite(&tft);
//screen width: 320, height: 170

#include "enums.h"
#include "config.h"
#include "get_time.h"
#include "get_image.h"
#include "screen_startup.h"
#include "screen_loading.h"
#include "screen_error.h"

// TaskHandle_t fetchTaskHandle;

void setup() {
  Serial.begin(115200);
  tft.init();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);

  sprite.createSprite(320, 170);

  if(!LittleFS.begin()){
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }
  
  // ESP32 will crash if any of the fonts are missing
  bool font_missing = false;

  if (LittleFS.exists("/inter_light_14.vlw") == false
    || LittleFS.exists("/inter_regular_16.vlw") == false
    || LittleFS.exists("/inter_semibold_24.vlw") == false
    || LittleFS.exists("/inter_bold_48.vlw") == false) {
    font_missing = true;
  }

  if (font_missing) {
    Serial.println("\r\nFont missing in LittleFS, did you upload it?");
    while(1) yield();
  } else { 
    Serial.println("\r\nFonts found OK.");
  }
  
  showStartupScreen();
  connectToWiFi();

  //if (isConnected) {
    //xTaskCreatePinnedToCore(fetchDataTask, "FetchTask", 8192, NULL, 1, &fetchTaskHandle, 1);
    //fetchCrypto();
    //drawCryptoToSprite();
  //}
}

void loop() {
  unsigned long currentMillis = millis();
  unsigned long elapsedMillis = currentMillis - lastUpdated;
  unsigned long elapsedMinutes = elapsedMillis / 60000;

  switch (currentState) {
    case STATE_LOADING:
      showLoadingScreen();

      //for initial state only
      if(lastUpdated == 0) {
        fetchCrypto();
        drawCryptoToSprite();
      }
      break;
    case STATE_ERROR:
      showErrorScreen();
      break;
    case STATE_RESULT:
      if (WiFi.status() == WL_CONNECTED) {
        if (elapsedMinutes >= updateInterval) {
            fetchCrypto();
            drawCryptoToSprite();
        }
      } else {
        connectToWiFi();
      }
      
      break;
  }

  delay(1000);
}

/*void fetchDataTask(void *parameter) {
  for (;;) {
    //use for first time only due to need to show loading screen
    if (WiFi.status() == WL_CONNECTED && lastUpdated == 0) {
      //fetchCrypto();
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}*/

void connectToWiFi() {
  sprite.loadFont(INTER_REGULAR_16, LittleFS);
  sprite.fillSprite(TFT_BLACK);
  sprite.setTextDatum(MC_DATUM);
  sprite.setTextColor(TFT_WHITE, TFT_BLACK);

  String label = "Connecting Wi-Fi";
  int label_height = sprite.fontHeight();
  sprite.drawString(label, sprite.width() / 2, sprite.height() / 2);
  sprite.pushSprite(0, 0);

  WiFi.begin(ssid, password);
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts <= 20) {
    delay(1000);
    if(attempts % 4 != 0){ 
      sprite.drawString(".", ((sprite.width() - sprite.textWidth(label)) / 2) + sprite.textWidth(label) + ((attempts % 4) * 10), (sprite.height() / 2) + label_height - 10);
    } else {
      sprite.fillSprite(TFT_BLACK);
      sprite.drawString(label, sprite.width() / 2, sprite.height() / 2);
    }
    sprite.pushSprite(0, 0);
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    isConnected = true;

    sprite.fillSprite(TFT_BLACK);
    sprite.setTextDatum(MC_DATUM);
    sprite.setTextColor(TFT_GREEN, TFT_BLACK);
    sprite.drawString("Wi-Fi Connected!", sprite.width() / 2, sprite.height() / 2);
    sprite.pushSprite(0, 0);
    delay(1000);

    currentState = STATE_LOADING;
  } else {
    isConnected = false;

    errorMessage = "Failed to connect Wi-Fi";
    currentState = STATE_ERROR;
    //showErrorScreen();
  }

  sprite.unloadFont();
}

void drawCryptoToSprite() {
  sprite.fillSprite(TFT_BLACK);
  sprite.setTextDatum(TL_DATUM);

  // Draw PNG icon
  if(cryptocurrency.icon != NULL) {
    drawLocalPNG(imagePath, 10, 10);
  }

  // Time and Date
  sprite.loadFont(INTER_LIGHT_14, LittleFS);
  sprite.setTextColor(TFT_WHITE, TFT_BLACK);
  String last_sync_ago = "Last Synced:" + cryptocurrency.last_updated;
  int last_updated_width = sprite.textWidth(last_sync_ago);

  sprite.drawString(last_sync_ago, 320 - 10 - last_updated_width, 10, 2);
  sprite.unloadFont();

  sprite.loadFont(INTER_REGULAR_16, LittleFS);
  sprite.drawString(cryptocurrency.name, 10, 45, 2);
  sprite.setTextColor(42295, TFT_BLACK);
  sprite.drawString(" (" + cryptocurrency.symbol + "-" + cryptocurrency.currency + ")", 12 + sprite.textWidth(cryptocurrency.name), 45, 2);
  sprite.unloadFont();

  // Price
  sprite.loadFont(INTER_BOLD_48, LittleFS);
  sprite.setTextColor(TFT_WHITE, TFT_BLACK);

  sprite.drawString(cryptocurrency.price, 10, 70, 2);
  sprite.unloadFont();
 
  // Change percent
  sprite.loadFont(INTER_SEMIBOLD_24, LittleFS);
  int barX = 295, barY = 45, barW = 15, barH = 135, barRadius = 6;
  if (cryptocurrency.change >= 0) {
    sprite.setTextColor(TFT_GREEN, TFT_BLACK);
    sprite.drawString("↑" + String(cryptocurrency.change, 2) + "%", 10, 130, 2);
    sprite.fillRoundRect(barX, barY, barW, barH, barRadius, TFT_GREEN);
  } else {
    sprite.setTextColor(TFT_RED, TFT_BLACK);
    sprite.drawString("↓" + String(fabsf(cryptocurrency.change), 2) + "%", 10, 130, 2);
    sprite.fillRoundRect(barX, barY, barW, barH, barRadius, TFT_RED);
  }

  sprite.unloadFont();
  sprite.pushSprite(0, 0);
}

void fetchCrypto() {
  if ((WiFi.status() == WL_CONNECTED)) {
    HTTPClient http;
    http.begin(coinApiURL);
    int httpCode = http.GET();

    if (httpCode > 0) {
      String payload = http.getString();
      DynamicJsonDocument doc(8192);
      DeserializationError error = deserializeJson(doc, payload);
      if (error) {
        Serial.print("JSON deserialization failed: ");
        Serial.println(error.c_str());
        errorMessage = "JSON deserialization failed";
        currentState = STATE_ERROR;
        return;
      }

      JsonObject data = doc["data"];

      if(doc["success"] == true) {
        cryptocurrency.name = data["name"].as<String>();
        cryptocurrency.icon = data["icon"].as<String>(); // "https://coin-images.coingecko.com/coins/images/1/thumb/bitcoin.png?1696501400"; // 
        cryptocurrency.symbol = data["symbol"].as<String>();
        cryptocurrency.currency = data["currency"].as<String>();
        cryptocurrency.price = data["price"].as<String>();
        cryptocurrency.change = data["change"].as<float>();

        cryptocurrency.last_updated = formatTimeAgo(doc["data"]["last_updated_min_ago"]);

        //download icon and store to FS
        if (cryptocurrency.icon != NULL && !downloadPngToLittleFS(cryptocurrency.icon, imagePath)) {
          Serial.println("Image download failed!");
          cryptocurrency.icon = "";
        }

        lastUpdated = millis();
        errorMessage = "";
        currentState = STATE_RESULT;
      } else {
        errorMessage = data["message"].as<String>();
        currentState = STATE_ERROR;
      }
    } else {
      errorMessage = "Error on API Call.";
      currentState = STATE_ERROR;
    }

    http.end();
  }
}