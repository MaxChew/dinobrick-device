void showStartupScreen() {
  sprite.fillSprite(TFT_BLACK);
  sprite.setTextDatum(ML_DATUM);
  drawLocalPNG(logoPath, (sprite.width() - 99) / 2, (sprite.height() - 79) / 2, true);
  sprite.pushSprite(0, 0);
  delay(4000);
}