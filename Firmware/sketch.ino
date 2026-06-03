#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// I2C Pins cho ESP32-S3
#define I2C_SDA 8
#define I2C_SCL 9

// OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// GPIO ESP32-S3
#define TRIG_PIN 5
#define ECHO_PIN 6
#define RAIN_SWITCH 4 

#define LED_GREEN 15
#define LED_YELLOW 16
#define LED_RED 17
#define BUZZER_PIN 18

// WIFI
const char* ssid = "CirkitWifi";
const char* password = "";

// ĐƯỜNG DẪN LOCALTUNNEL 
const char* serverUrl = "https://tram-canh-bao-ngap-123.loca.lt/api/data";

// ==========================================
// BIẾN TOÀN CỤC CHIA SẺ GIỮA 2 NHÂN (CORE)
// ==========================================
volatile float shared_distance = 0;
volatile float shared_waterLevel = 0;
volatile bool shared_isRaining = false;
volatile int shared_alertLevel = 0; // 0: An Toan, 1: Chu y, 2: Bao dong, 3: Nguy hiem

// Bộ định thời cho Core 1 (Cảm biến)
unsigned long lastSensorRead = 0;
const unsigned long sensorInterval = 500; // Đọc cảm biến mỗi 0.5s (Cực mượt)

// ==========================================
// TASK CHẠY TRÊN CORE 0: CHUYÊN GỬI WEB
// ==========================================
void taskSendWeb(void * pvParameters) {
  for(;;) { // Vòng lặp vô hạn của Task
    if(WiFi.status() == WL_CONNECTED) {
      WiFiClientSecure client;
      client.setInsecure(); 
      HTTPClient http;
      
      http.begin(client, serverUrl);
      http.addHeader("Content-Type", "application/json");
      http.addHeader("Bypass-Tunnel-Reminder", "true");

      // Chuyển đổi mã cảnh báo thành chữ
      String rainStr = shared_isRaining ? "CO MUA" : "KHONG MUA";
      String statusStr = "AN TOAN";
      if(shared_alertLevel == 1) statusStr = "CHU Y";
      else if(shared_alertLevel == 2) statusStr = "BAO DONG";
      else if(shared_alertLevel == 3) statusStr = "NGUY HIEM";

      // Lắp ghép JSON
      String jsonPayload = 
      "{"
      "\"distance\":" + String(shared_distance, 1) + 
      ",\"water_level\":" + String(shared_waterLevel, 1) + 
      ",\"is_raining\":" + String(shared_isRaining ? "true" : "false") + 
      ",\"rain_status\":\"" + rainStr + "\"" + 
      ",\"status\":\"" + statusStr + "\""
      "}";

      int responseCode = http.POST(jsonPayload);
      
      if(responseCode > 0) {
        Serial.print("[Core 0] Gui Web OK - Ma: ");
        Serial.println(responseCode);
      } else {
        Serial.println("[Core 0] Gui Web That bai!");
      }
      http.end();
    }
    
    // Đợi 5 giây mới gửi lại (Sử dụng hàm delay của FreeRTOS, không gây đơ hệ thống)
    vTaskDelay(2000 / portTICK_PERIOD_MS); 
  }
}

void setup() {
  Serial.begin(9600);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(RAIN_SWITCH, INPUT_PULLUP);

  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  Wire.begin(I2C_SDA, I2C_SCL);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)){
    Serial.println("Khong tim thay OLED");
    while(true);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,10);
  display.println("Khoi dong...");
  display.display();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
  }

  // KHỞI TẠO LUỒNG GỬI WEB CHẠY SONG SONG TRÊN CORE 0
  xTaskCreatePinnedToCore(
    taskSendWeb,   // Tên hàm thực thi
    "TaskWeb",     // Tên định danh
    10000,         // Dung lượng RAM cấp cho Task
    NULL,          // Tham số truyền vào
    1,             // Mức độ ưu tiên
    NULL,          // Task handle
    0              // Chạy trên Core 0
  );
}

// ==========================================
// LOOP CHẠY TRÊN CORE 1: XỬ LÝ CẢM BIẾN (SIÊU MƯỢT)
// ==========================================
void loop() {
  // Thay thế delay() bằng kỹ thuật millis() 
  if (millis() - lastSensorRead >= sensorInterval) {
    lastSensorRead = millis();

    // 1. ĐO MỰC NƯỚC CÓ TIMEOUT (30ms = max ~5 mét) để chống lag
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    long duration = pulseIn(ECHO_PIN, HIGH, 30000); 
    
    // Nếu timeout (bằng 0), giữ nguyên giá trị cũ tránh màn hình nháy
    if (duration > 0) {
      shared_distance = duration * 0.034 / 2;
      shared_waterLevel = 150.0 - shared_distance;
      if(shared_waterLevel < 0) shared_waterLevel = 0;
    }

    // 2. ĐỌC CẢM BIẾN MƯA
    shared_isRaining = (digitalRead(RAIN_SWITCH) == LOW);

    // 3. XỬ LÝ CẢNH BÁO NHANH
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_YELLOW, LOW);
    digitalWrite(LED_RED, LOW);
    noTone(BUZZER_PIN);

    String statusStr = "";
    if (shared_distance > 100) {
      shared_alertLevel = 0;
      statusStr = "AN TOAN";
      digitalWrite(LED_GREEN, HIGH);
    } 
    else if (shared_distance > 50) {
      shared_alertLevel = 1;
      statusStr = "CHU Y";
      digitalWrite(LED_YELLOW, HIGH);
    } 
    else if (shared_distance > 20) {
      shared_alertLevel = 2;
      statusStr = "BAO DONG";
      digitalWrite(LED_RED, HIGH);
    } 
    else {
      shared_alertLevel = 3;
      statusStr = "NGUY HIEM";
      digitalWrite(LED_RED, HIGH);
      tone(BUZZER_PIN, 1200);
    }

    // 4. HIỂN THỊ OLED TỨC THÌ
    // ==========================================
    // 4. HIỂN THỊ OLED TỨC THÌ
    // ==========================================
    String rainStatus = shared_isRaining ? "CO MUA" : "KHONG MUA";
    display.clearDisplay();
    
    // BẮT BUỘC PHẢI CÓ LỆNH NÀY ĐỂ RESET VỀ CHỮ NHỎ
    display.setTextSize(1); 
    display.setTextColor(WHITE);

    // Dòng 1 (Y = 0)
    display.setCursor(0, 0);
    display.print("Mua: ");
    display.print(rainStatus);

    // Dòng 2 (Y = 15)
    display.setCursor(0, 15);
    display.print("Khoang cach: ");
    display.print(shared_distance, 1);
    display.print(" cm");

    // Dòng 3 (Y = 30)
    display.setCursor(0, 30);
    display.print("Muc nuoc: ");
    display.print(shared_waterLevel, 1);
    display.print(" cm");

    // Dòng 4: Trạng thái (Phóng to lên một xíu ở dưới cùng)
    display.setTextSize(2);
    display.setCursor(0, 48); // Đẩy xuống sát mép dưới
    display.print(statusStr);

    display.display();
  }
}