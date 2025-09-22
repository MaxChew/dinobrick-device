// Timing variables for the loading dots
unsigned long lastDotTime = 0;
const long dotInterval = 500; // milliseconds between dots
int dotCount = 0;

void showLoadingScreen() {
  if (millis() - lastDotTime >= dotInterval) {
    sprite.loadFont(INTER_REGULAR_16, LittleFS);
    sprite.fillSprite(TFT_BLACK);
    sprite.setTextDatum(MC_DATUM);
    sprite.setTextColor(TFT_WHITE, TFT_BLACK);

    String label = "Loading";
    int label_height = sprite.fontHeight();
    sprite.drawString(label, sprite.width() / 2, sprite.height() / 2);

    for (int i = 0; i < dotCount; i++) {
      sprite.drawString(".", ((sprite.width() - sprite.textWidth(label)) / 2) + sprite.textWidth(label) + (i * 10), (sprite.height() / 2) + label_height - 10);
    }
    
    sprite.pushSprite(0, 0);
    
    dotCount++;
    if (dotCount > 3) {
      dotCount = 0; // Reset dots to create the animation effect
    }
    
    lastDotTime = millis();
  }
}