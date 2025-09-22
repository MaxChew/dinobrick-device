// WIFI Settings
const char* ssid = "";
const char* password = "";

// Others
const char* logoPath = "/dinobrick_logo.png";
const char* imagePath = "/image.png";
unsigned long updateInterval = 15; //by minutes
unsigned long lastUpdated = 0;
bool isConnected = false;
String errorMessage;

// State
AppState currentState = STATE_CONNECTING_WIFI;

// Crypto API URL
String coin = "btc";
String currency = "USD";
String apiKey = "dk_wDwnBGB9aGDjCRRznprwp5qaz9Drgman"; 
const String coinApiURL = "https://dinocrypto.com/api/v1/cryptocurrencies/" + coin + "?currency=" + currency + "&apikey=" + apiKey;

// Crypto Object
struct {
  String name; // Bitcoin
  String icon; // Icon URL - 25 x 25 sample: https://coin-images.coingecko.com/coins/images/1/thumb/bitcoin.png?1696501400
  String symbol; // BTC
  String currency; // USD
  String price; // formatted price from API 1000 = 1,000 
  float change; // percentage changes
  String last_updated;
} cryptocurrency;