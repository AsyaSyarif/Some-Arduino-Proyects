/*
Proyecto de control de acceso para el fichaje de personal a nivel industrial.
 
 Version: 1.0.0
 Fecha: 18/02/2015
 
 Desarrollado por: Marcos Casado Herrero @marcoscasado6
 
PINOUT:
Boton Entrada D5
Boton Salida D4
Led Rojo D3
Led Verde D2


RC522 MODULE    Uno/Nano     MEGA
SDA             D10          D9
SCK             D13          D52
MOSI            D11          D51
MISO            D12          D50
IRQ             N/A          N/A
GND             GND          GND
RST             D9           D8
3.3V            3.3V         3.3V

 */

#include <Ethernet.h>
#include <SPI.h>
#include <RFID.h>
#include <Keypad.h>
#include <SD.h>

#define BTN_IN 5
#define BTN_OUT 4
#define SDA 9
#define RESET 8
#define GREEN 2
#define RED 3

//Declaraciones Varias
RFID rfid(SDA, RESET);
int reason=0;
int id [5];

EthernetClient client;
// Direccion mac del dispositivo
byte mac_address[] = {
  0x90,0xA2,0xDA,0x0D,0xF4,0x61};
// Direcciones IP del cliente y el servidor
IPAddress server_ip(192,168,1,43);

//Constructor teclado
const byte ROWS = 4; // Four rows
const byte COLS = 4; // Three columns
// Define the Keymap
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'#','0','*','D'}
};
// Connect keypad ROW0, ROW1, ROW2 and ROW3 to these Arduino pins.
byte rowPins[ROWS] = { 9, 8, 7, 6 };
// Connect keypad COL0, COL1 and COL2 to these Arduino pins.
byte colPins[COLS] = { 12, 11, 10 }; 

// Create the Keypad
Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );



void setup(){
  
  Serial.begin(9600);
  Serial.println("Iniciando...");
  SPI.begin();
  Serial.println("SPI ... [OK]");
  rfid.init();
  Serial.println("RFID ... [OK]");
  
  pinMode(RED,OUTPUT);
  pinMode(GREEN,OUTPUT);
  pinMode(BTN_IN,INPUT);
  pinMode(BTN_OUT,INPUT);

  delay(100);
  
  Serial.println("Cargando modulo Ethernet ...");
  Ethernet.begin(mac_address);
  Serial.println("Ethernet ... [OK]");
  Serial.print("Direccion IP establecida: ");
  Serial.println(Ethernet.localIP()); 

  delay(100); // Tiempo de cortesia
}


void loop (){
 
 //Bucle de entradas
 if (digitalRead(BTN_IN)==1){
   digitalWrite(GREEN,HIGH);
   reason = 10; // Razon de entrada
   
     while(!rfid.isCard()){
     readCard();
     }
 } 
 
 //Bucle de salidas
  if (digitalRead(BTN_OUT)==1){
   digitalWrite(GREEN,HIGH);
   reason = 20; // Razon de salida
   
     while(!rfid.isCard()){
     readCard();
     }
 } 
}


//Metodo para leer la tarjeta
void readCard() {

  if (rfid.isCard()) {
    if (rfid.readCardSerial()) {

      for(int i=0; i<=4 ; i++) {
        id[i] = rfid.serNum[i];    
      }
      String card_id = String(id[0])+String(id[1]) +String(id[2]) +String(id[3]) +String(id[4]);      
      Serial.println("Numero de Empleado: " + card_id); 
      delay(200);
      httpRequest(card_id); //Lanzamos la peticion GET
      delay(1000); // para que no se imprima constantemente el mismo numero 
      
    }
  }
  rfid.halt();
}


//Metodo para ejecutar una consulta SQL
void httpRequest(String card_id) {
  Serial.println("Iniciando conexion HTTP...");
  // Se conecta al servidor en el puerto 80 (web)
  if (client.connect(server_ip, 80)) {
    // Envia el requerimiento al servidor via GET
    
    client.print("GET /connect.php?id_card=");
    client.print(card_id); 
    client.print("&reason=");
    client.print(reason);
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println("192.168.1.1");
    client.println("User-Agent: Arduino-Ethernet");
    client.println("Connection: close");
    client.println();
    //Aqui encender LED verde
    client.stop();
    Serial.println("Conexion 192.68.1.1:80 ... [OK]");
  } 
  else {
    // Si la conexion fallo se desconecta
    Serial.println("Conexion 192.68.1.1:80 ... [ERROR]");
    Serial.println("Desconectando...");
    //Aqui encender LED rojo
    client.stop();
  }
}



