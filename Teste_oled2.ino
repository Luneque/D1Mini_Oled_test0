#include <ESP8266WiFi.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <time.h>

// Credenciais Wi-Fi informadas
const char* ssid = "S729-1";
const char* password = "25102026";

// Configuração de Fuso Horário de São Paulo (-3h = -10800s, sem horário de verão = 0)
// Em formato POSIX TZ: "<-03>3" ou UTC-3
const char* tzInfo = "<-03>3";
const char* ntpServer1 = "a.st1.ntp.br";
const char* ntpServer2 = "b.st1.ntp.br";
const char* ntpServer3 = "pool.ntp.org";

// Display Wemos OLED Shield 64x48 (I2C)
// Pinos padrão D1 Mini: D1 (SCL / GPIO5), D2 (SDA / GPIO4)
// Controlador padrão SSD1306 64x48
// U8G2_SSD1306_64X48_ER_F_HW_I2C(rotation, [reset])
U8G2_SSD1306_64X48_ER_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

void showMessage(const char* line1, const char* line2 = nullptr, const char* line3 = nullptr) {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_5x7_tf);
  if (line1) u8g2.drawStr(0, 10, line1);
  if (line2) u8g2.drawStr(0, 24, line2);
  if (line3) u8g2.drawStr(0, 38, line3);
  u8g2.sendBuffer();
}

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("\n--- Iniciando D1 Mini OLED 64x48 (Relógio NTP SP) ---");

  // Inicia barramento I2C nos pinos nativos do shield (SDA=D2, SCL=D1)
  Wire.begin(D2, D1);
  u8g2.begin();

  showMessage("WiFi...", "Conectando");

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.print("Conectando ao WiFi");
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi Conectado!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    showMessage("WiFi OK", WiFi.localIP().toString().c_str());
  } else {
    Serial.println("\nFalha ao conectar no WiFi. Continuando para tentar reconexao...");
    showMessage("WiFi Erro", "Tentando...");
  }
  delay(1500);

  // Configuração NTP com Timezone POSIX para São Paulo
  configTime(tzInfo, ntpServer1, ntpServer2, ntpServer3);
  Serial.println("Aguardando sincronizacao NTP...");
}

void loop() {
  time_t now = time(nullptr);
  struct tm timeinfo;
  localtime_r(&now, &timeinfo);

  // timeinfo.tm_year conta a partir de 1900
  bool timeSynced = (timeinfo.tm_year > (2020 - 1900));

  u8g2.clearBuffer();

  if (!timeSynced) {
    u8g2.setFont(u8g2_font_5x7_tf);
    u8g2.drawStr(2, 16, "Sincroniz.");
    u8g2.drawStr(2, 30, "Hora NTP..");
    
    // Status do WiFi no rodapé
    if (WiFi.status() == WL_CONNECTED) {
      u8g2.drawStr(2, 44, "WiFi: OK");
    } else {
      u8g2.drawStr(2, 44, "WiFi: Recon.");
    }
    u8g2.sendBuffer();
    Serial.println("Aguardando hora do NTP...");
    delay(1000);
    return;
  }

  // Buffers formatados
  char dateStr[16];
  char timeStr[16];
  char secStr[16];

  // Ex: "27/10/26"
  snprintf(dateStr, sizeof(dateStr), "%02d/%02d/%02d", 
           timeinfo.tm_mday, 
           timeinfo.tm_mon + 1, 
           timeinfo.tm_year % 100);

  // Ex: "13:45"
  snprintf(timeStr, sizeof(timeStr), "%02d:%02d", 
           timeinfo.tm_hour, 
           timeinfo.tm_min);

  // Ex: ":30"
  snprintf(secStr, sizeof(secStr), ":%02d", timeinfo.tm_sec);

  // Renderização formatada para 64x48
  // 1. Data centralizada no topo
  u8g2.setFont(u8g2_font_5x7_tf);
  int dateWidth = u8g2.getStrWidth(dateStr);
  int dateX = (64 - dateWidth) / 2;
  u8g2.drawStr(dateX, 10, dateStr);

  // Linha separadora
  u8g2.drawHLine(2, 13, 60);

  // 2. Horas e minutos com fonte maior e nítida
  u8g2.setFont(u8g2_font_7x14B_tf);
  u8g2.drawStr(4, 32, timeStr);

  // 3. Segundos menores ao lado
  u8g2.setFont(u8g2_font_5x7_tf);
  u8g2.drawStr(43, 32, secStr);

  // 4. Rodapé indicando localização ou status WiFi
  u8g2.setFont(u8g2_font_4x6_tf);
  if (WiFi.status() == WL_CONNECTED) {
    u8g2.drawStr(10, 44, "SP - Brasil");
  } else {
    u8g2.drawStr(12, 44, "Sem Wi-Fi");
  }

  u8g2.sendBuffer();

  // Log no serial
  char fullLog[64];
  snprintf(fullLog, sizeof(fullLog), "[%s %02d:%02d:%02d] Atualizado display.", 
           dateStr, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
  Serial.println(fullLog);

  delay(500);
}
