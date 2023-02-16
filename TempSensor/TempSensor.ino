#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// Set up WiFi
const char* ssid = "network";  // Enter SSID here
const char* password = "password";  //Enter Password here

ESP8266WebServer server(80);

// Set up screen
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Set up DHT sensor
#define DHTPIN D7
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

// Temperature, humidity variables
float t = 0.0;
float h = 0.0;

String ArduinoIP = "";

void setup() {
  // Setup serial port
  Serial.begin(115200);
  delay(100);

  // Setup Sensor
  pinMode(DHTPIN, INPUT);
  dht.begin();

  // Setup Display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }

  delay(2000);
  display.clearDisplay();
  display.setTextColor(WHITE);

  //Begin WiFi
  WiFi.begin(ssid, password);

  //check wi-fi is connected to wi-fi network
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.print("Got IP: ");

  ArduinoIP = WiFi.localIP().toString().c_str();
  Serial.println(WiFi.localIP());

  server.on("/", HandleOnConnect);
  server.onNotFound(HandleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

String fish1_1 = "  __ ";
String fish1_2 = "\\/ o\\";
String fish1_3 = "/\\__/";

String fish2_1 = " __  ";
String fish2_2 = "/o \\/";
String fish2_3 = "\\__/\\";

int16_t fishY1 = 10;
int16_t fishY2 = 17;
int16_t fishY3 = 24;
int16_t fishX = 0;

int whatFishToRender = 1;
bool fishDirection = true; // true -> left to right   false -> right to left
int fishPosition = 0;  // 0 - 9

void CalculatePosition() {
  if (fishDirection == true) {
    whatFishToRender = 1;
    fishX += 10;
    fishPosition += 1;
  }
  else {
    whatFishToRender = 2;
    fishX -= 10;
    fishPosition -= 1;
  }

  if (fishPosition == 9) {
    fishDirection = !fishDirection;
  }
  if (fishPosition == 0) {
    fishDirection = !fishDirection;
  }
}

void RenderFishy() {
  CalculatePosition();

  if (whatFishToRender == 1) {
    display.setCursor(fishX, fishY1);
    display.print(fish1_1);
    display.setCursor(fishX, fishY2);
    display.print(fish1_2);
    display.setCursor(fishX, fishY3);
    display.print(fish1_3);
  }
  else {
    display.setCursor(fishX, fishY1);
    display.print(fish2_1);
    display.setCursor(fishX, fishY2);
    display.print(fish2_2);
    display.setCursor(fishX, fishY3);
    display.print(fish2_3);
  }
}

void DisplayValues(float &t, float &h) {
  // Clear display
  display.clearDisplay();

  // Setup display
  display.setTextSize(1);

  // Arduino IP
  display.setCursor(0, 0);
  display.print("IP: ");
  display.print(ArduinoIP);

  RenderFishy();

  // display temperature
  display.setCursor(0, 42);
  display.print("Temperature: ");
  display.print(t);
  display.print(" ");
  display.cp437(true);
  display.write(167);
  display.print("C");

  // display humidity
  display.setCursor(0, 55);
  display.print("Humidity: ");
  display.print(h);
  display.print(" %");

  display.display();
}

void HandleOnConnect() {
  server.send(200, "text/html", SendHTML(t, h));
}


void HandleNotFound() {
  server.send(404, "text/plain", "Not found");
}

String SendHTML(float t, float h) {
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr += "<title>ESP8266 with DHT11 Sensor</title>\n";
  ptr += "<style>html { display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr += "body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;}\n";
  ptr += "p {font-size: 24px;color: #444444;margin-bottom: 10px;}\n";
  ptr += "</style>\n";
  ptr += "</head>\n";
  ptr += "<body>\n";
  ptr += "<div id=\"webpage\">\n";
  ptr += "<p>Temperature: ";
  ptr += t;
  ptr += " &#8451;</p>";
  ptr += "<p>Humidity: ";
  ptr += h;
  ptr += " %</p>";
  ptr += "</div>\n";
  ptr += "</body>\n";
  ptr += "</html>\n";
  return ptr;
}

void loop() {
  delay(1000);

  //read temperature and humidity
  t = dht.readTemperature();
  h = dht.readHumidity();

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
  }

  DisplayValues(t, h);

  server.handleClient();
}
