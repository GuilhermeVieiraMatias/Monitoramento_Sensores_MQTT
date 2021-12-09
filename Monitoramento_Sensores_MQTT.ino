/* Programa para Artigo FilipeFlop:
 * Monitor de temperatura e luminosidade.
 * Autor: Guilherme Vieira Matias
*/
//=====================================================================
// --- Inclusão de bibliotecas ---
#include <ESP8266WiFi.h> 
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "DHT.h"
// ======================================================================
// --- Dados de Acesso do seu roteador ---
#define WLAN_SSID       "Guilherme - Casa" // Informação da SSID do seu roteador
#define WLAN_PASS       "Gui@2021"   // senha de acesso do seu roteador
// ======================================================================
// --- Dados de Acesso da Plataforma Adafruit IO ---
#define AIO_SERVER      "io.adafruit.com"     // manter fixo
#define AIO_SERVERPORT  1883                  // manter fixo
#define AIO_USERNAME    "GuilhermeVieiraMatias"        // sua informação
#define AIO_KEY         "aio_zYKV48trL5bO2VD3ZUyUpa9CSwIm" // sua informação
// ======================================================================
// --- Mapeamento de Hardware ---
#define DHTPIN 4
#define LEDB 15
#define DHTTYPE DHT11
#define LDR A0
DHT dht(DHTPIN, DHTTYPE);
 
WiFiClient client; // cria uma classe WiFiClient com o ESP8266 para conexão com o servidor MQTT
 
// Configura a classe MQTT passando para o WiFi cliente e Servidor MQTT os detalhes do login
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
 
// O endereçamento para enviar os arquivos seguem o formato: <username>/feeds/<feedname>
Adafruit_MQTT_Publish temperatura = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/temperatura");
Adafruit_MQTT_Publish Lampada = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Lampada");
Adafruit_MQTT_Publish umidade = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/umidade");
Adafruit_MQTT_Publish ldr = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/ldr");
 
// ======================================================================
// --- Variáveis Globais ---
float umd = 0;
float tmp = 0;
int ldrValor = 0;
// ======================================================================
// --- Void Setup ---
void setup() {
   
  Serial.begin(115200); // inicia comunicação serial com velocidade 115200
 
  Serial.println(F("Monitorar a Temperatura de um Ambiente - Adafruit MQTT")); // escreve na serial
   
  // Conecta ao ponto de acesso WiFi
  Serial.println(); Serial.println();
  Serial.print("Conectando ao ");
  Serial.println(WLAN_SSID);
 
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
 
  Serial.println("WiFi Conectado");
  Serial.println("IP endereço: "); Serial.println(WiFi.localIP());
   
// ======================================================================
// --- Configuração IO ---
  pinMode(DHTPIN, OUTPUT); // pino D2 como saída para DHT11
  pinMode(LEDB, OUTPUT);
  dht.begin();
}

// ======================================================================

// --- Função para Conexão / Reconexão MQTT ---
  void MQTT_connect() {
    int8_t ret;
   
    // para de tentar reconexão quando consegue
    if (mqtt.connected()) {
      return;
    }
    Serial.print("Conectando ao MQTT... "); // imprime na serial
    // tenta conexão 5 vezes. Depois WatchDogTime!
    uint8_t retries = 5;
    while ((ret = mqtt.connect()) != 0) { // conectará quando retornar 0
      Serial.println(mqtt.connectErrorString(ret));
      Serial.println("Nova tentativa de conexão MQTT em 5 segundos...");
      mqtt.disconnect();
      delay(5000);  // aguarda 5 segundos
      retries--;
      if (retries == 0) { // trava e espera reset
        while (1);
      }
    }
    Serial.println("MQTT Conectado!"); // imprime na serial
  }
// ======================================================================
// --- void loop ---
void loop() {
  MQTT_connect();   // chama função para conexão com MQTT server
                  
  // Leitura do sensor LDR
  ldrValor = analogRead(LDR);
  Serial.print("LDR:");
  Serial.println(ldrValor);

  if(ldrValor < 25){
    
    Serial.println("Lampada Desligada!!");
    Lampada.publish(0);                       // publica o valor 0 no feed "Lampada"
    
    }else{
    
      Serial.println("Lampada Ligada!!");
      Lampada.publish(1);                     // publica o valor 1 no feed "Lampada"
    }
   
  // Leitura da temperatura e umidade
  umd = dht.readHumidity();
  tmp = dht.readTemperature();
 
  Serial.print("umidade:"); // imprime "umidade:"
  Serial.println(umd);  // imprime a variavel umidade
  Serial.print("temperatura:");    // imprime "temperatura:"
  Serial.println(tmp);  // imprime a variavel temperatura
  
  ldr.publish(ldrValor); 
  temperatura.publish(tmp);     // publica variável "tmp" no feed "temperatura"
  umidade.publish(umd);         // publica variável "umd" no feed "umidade"

   
  delay(5000); // aguarda 5 segundos
  /* Observação: A plataforma Adafruit IO só permite 30 publicações/minuto
   * na conta gratuita. Então é importante não ultrapassar ou sua leitura 
   * na dashboard será instável e incorreta.*/
}
 
// ======================================================================

// --- FIM ---
