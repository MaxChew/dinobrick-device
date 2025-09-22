void showErrorScreen() {
  sprite.loadFont(INTER_REGULAR_16, LittleFS);
  sprite.fillSprite(TFT_BLACK);
  sprite.setTextDatum(MC_DATUM);
  sprite.setTextColor(TFT_RED, TFT_BLACK);
  
  int label_height = sprite.fontHeight();
  if(errorMessage != NULL) {
    sprite.drawString(errorMessage, sprite.width() / 2, (sprite.height() / 2) - (label_height / 2) - 5);
  }
  sprite.drawString("Check network setting & restart.", sprite.width() / 2, (sprite.height() / 2) + (label_height / 2) + 5);
  sprite.pushSprite(0, 0);
}