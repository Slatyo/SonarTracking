#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// MAX_DEPTH_IN_CM defines the mapping point for the canvas
// Since I currently do not have plans to support more then 50meters we leave it at that
#define MAX_DEPTH_IN_CM 5000 
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define ULTRASONIC_TRIG_PIN 11
#define ULTRASONIC_ECHO_PIN 12

// Number of previous filtered distance values to consider
#define BUFFER_SIZE 5  

// If we have a change larger 5 cm over the past iterations we detect an anomaly
// This is experimental 
#define CHANGE_THRESHOLD_IN_CM 5 

// Duration the anomaly is shown on the screen
#define ANOMALY_DURATION 2000  

// Flag to indicate if anomaly is cached
bool anomaly_cached = false;  
bool anomaly_detected = false;

// Time when anomaly was first detected
unsigned long anomaly_start_time;  

// Array to store previous filtered distances
int prev_filtered_distances[BUFFER_SIZE];  

// Index for circular buffer
int prev_index = 0;

// Stores the filtered distance value from the previous iteration of the loop
int prev_filtered_distance_cm = 0;

// Stores the previous y-coordinate of the data point on the OLED display. 
// It is used to draw a line between the previous and current data points when visualizing the distance measurements on the display
int prev_y = 0;


// Specify the number of readings to be stored in the readings array
#define NUM_READINGS 5  

int readings_water[NUM_READINGS];
int index_water = 0;
static long total_water = 0;

int readings_air[NUM_READINGS];
int index_air = 0;
static long total_air = 0;

void setup() {
  Serial.begin(9600);

  // Ultrasonic sensor pins
  pinMode(ULTRASONIC_TRIG_PIN, OUTPUT);
  pinMode(ULTRASONIC_ECHO_PIN, INPUT);

  // Initialize OLED with the I2C address 0x3C
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0,0);
  display.display();
}

void loop() {
  long duration = readUltrasonicSensor(ULTRASONIC_TRIG_PIN, ULTRASONIC_ECHO_PIN);

  // Currently I am just assuming the speed in which the ultrasonic wave travels
  // Once we have the DS18B20 installed we gain the ability to improve the calculation for speed
  // So we currently go with 1500 meters per second for water and 340 meters for air
  int distance_cm_water = calculateDistance(duration, 0.15);
  int distance_cm_air = calculateDistance(duration, 0.034);


  int filtered_distance_cm_water = applyMovingAverageFilter(distance_cm_water, readings_water, index_water, total_water);
  int filtered_distance_cm_air = applyMovingAverageFilter(distance_cm_air, readings_air, index_air, total_air);

  detectAnomaly(filtered_distance_cm_water);

  updateDisplay(filtered_distance_cm_water, filtered_distance_cm_air);

  delay(500);
}

long readUltrasonicSensor(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  return pulseIn(echoPin, HIGH);
}

int calculateDistance(long duration, float speed) {
  return duration * speed / 2;
}

bool detectAnomaly(int filtered_distance_cm) {
  if (anomaly_cached && !anomaly_detected && (millis() - anomaly_start_time >= ANOMALY_DURATION)) {
    anomaly_cached = false; 
  }
  // Store current filtered distance in circular buffer
  prev_filtered_distances[prev_index] = filtered_distance_cm;
  prev_index = (prev_index + 1) % BUFFER_SIZE;

  int change = calculateAverageChange(filtered_distance_cm);
  anomaly_detected = (change > CHANGE_THRESHOLD_IN_CM);

  updateAnomalyCache(anomaly_detected);

  return anomaly_detected;
}

void updateDisplay(int filtered_distance_cm_water, int filtered_distance_cm_air) {
  shiftGraphContent();
  drawNewDataPoint(filtered_distance_cm_water);
  updateTopRow(filtered_distance_cm_water, filtered_distance_cm_air);
  display.display();
}

int calculateAverageChange(int filtered_distance_cm) {
  // Find the minimum and maximum values in the previous filtered distances
  int min_distance = prev_filtered_distances[0];
  int max_distance = prev_filtered_distances[0];
  for (int i = 1; i < BUFFER_SIZE; i++) {
    min_distance = min(min_distance, prev_filtered_distances[i]);
    max_distance = max(max_distance, prev_filtered_distances[i]);
  }

  return (max_distance - min_distance) / (BUFFER_SIZE - 1);
}


void updateAnomalyCache(bool anomaly_detected) {
  if (anomaly_detected && !anomaly_cached) {
    anomaly_cached = true;
    anomaly_start_time = millis(); 
  }
}

void shiftGraphContent() {
  // Shift previous graph content to the left
  for (int y = 0; y < SCREEN_HEIGHT; y++) {
    for (int x = 0; x < SCREEN_WIDTH - 1; x++) {
      display.drawPixel(x, y, display.getPixel(x + 1, y));
    }
  }

  // Clear the rightmost column
  for (int y = 0; y < SCREEN_HEIGHT; y++) {
    display.drawPixel(SCREEN_WIDTH - 1, y, SSD1306_BLACK);
  }
}

void drawNewDataPoint(int filtered_distance_cm_water) {
  // Calculate the y-coordinate for the current data point
  int y = map(filtered_distance_cm_water, 0, MAX_DEPTH_IN_CM, SCREEN_HEIGHT - 9, 0);
  y = constrain(y, 0, SCREEN_HEIGHT - 9);
  display.drawPixel(SCREEN_WIDTH - 1, y, SSD1306_WHITE);
  display.drawLine(SCREEN_WIDTH - 2, prev_y, SCREEN_WIDTH - 1, y, SSD1306_WHITE);
  prev_y = y;
  prev_filtered_distance_cm = filtered_distance_cm_water;
}

void drawAnomalyIndicator() {
  if (anomaly_detected || anomaly_cached) {
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);
    display.setCursor(49, 0);
    display.print("!!!");
    display.display();
  } 
}

void updateTopRow(int filtered_distance_cm_water, int filtered_distance_cm_air) {
  display.fillRect(0, 0, SCREEN_WIDTH, 8, SSD1306_BLACK);
  display.setCursor(0, 0);
  display.print(filtered_distance_cm_water);
  display.print(" cm");
  display.setCursor(80, 0);
  display.print(filtered_distance_cm_air);
  display.print(" cm");


  drawAnomalyIndicator();
}

// Simple moving average filter to smooth out a series of input values over time by creating a circular buffer
int applyMovingAverageFilter(int newValue, int *readings, int &index, long &total) {
  total -= readings[index];
  readings[index] = newValue;
  total += newValue;
  index = (index + 1) % NUM_READINGS;

  return total / NUM_READINGS;
}
