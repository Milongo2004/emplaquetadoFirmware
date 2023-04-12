/*
  Fecha de creación: 31 DE ENERO DE 2023
  por: Juan Camilo Alvarez Jaramillo

  lee ambos, tag y teclado, el teclado es lento. borra con *
  entrega a la estación de almacén granel
  tara bien.
  pendiente: que si oprime el envío, envíe a base de datos y al regresar conserve el id
  en pantalla y para otro envío.

  si presiona la letra D, empieza a medir peso y espera envío.

  cuando no ha presionado la D no es necesario que mida peso, mas sí que muestre cantidad de cajas de esa referencia.

   cuando inicia, conecta Wifi y luego inmediatamente consulta los nombres de los emplaquetadores
   a los cuales se puede acceder después con la variable respuesta 9. pendiente borrar
   porción en enviarDAtos donde aparece la variabla para que no se modifique.

   el primer paso despues de conectar WiFi es solicitar la identificación.


   consulto el nombre de la persona y saludo en pantalla: nombre+bienvenid@

*/

//#include <EEPROM.h>//https://github.com/espressif/arduino-esp32/tree/master/libraries/EEPROM

#include "HX711.h" //This library can be obtained here http://librarymanager/All#Avia_HX711

#define LOADCELL_DOUT_PIN  16
#define LOADCELL_SCK_PIN  17

HX711 scale;
float calibration_factor = -276; //-7050 worked for my 440lb max scale setup

#define tareButton 39
String masa;

//en vez de teclado tomo unos botones y les asigno sus acciones

#define WIFI_PIN 35

//incluyo librerías Y defino variables para el control del RFID
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN          21
#define RST_PIN         22
int ledPin = 4;
int ledWiFi = 5;
String datorfid;
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance
MFRC522::StatusCode status; //variable to get card status

byte buffer[18];  //data transfer buffer (16+2 bytes data+CRC)
byte size = sizeof(buffer);
String dataL;
char dataChar[18];
String datoParaServidor;
byte longitudId = 6;
int contadorTeclado = 0;
byte cuentaEnvio = 0;
String datoTag;
char cadena[2];
byte cadenaMolde = 3;
char idProduccion[16];
char idProduccion2[16];
char idP[16];
char id_Molde[4];

int cuentaRevision = 0;
int lecturaJuegos = 0;

int idProduccionMenor = 0;
int idProduccionMayor = 0;
String idProduccionString = "";
String idProduccionString2 = "";

uint8_t pageAddr = 0x06;  //In this example we will write/read 16 bytes (page 6,7,8 and 9).
//Ultraligth mem = 16 pages. 4 bytes per page.
//Pages 0 to 4 are for special functions.
String num_molde;
String cant_moldes;
String respuesta;
String faltan;
String juegosParaRestar;
unsigned long nowtime = 0;

////incluyo librerías Y defino variables para el control del teclado
#include<Keypad.h>

const byte lineas = 4;
const byte columnas = 4;

char teclas [lineas][columnas] = {
  {'1', '4', '7', '*'},
  {'2', '5', '8', '0'},
  {'3', '6', '9', '#'},
  {'A', 'B', 'C', 'D'}
};

byte pinesLineas[lineas] = {13, 12, 14, 27};
byte pinesColumnas[columnas] = {26, 25, 33, 32};

Keypad teclado = Keypad (makeKeymap (teclas), pinesLineas, pinesColumnas, lineas, columnas);


//DISPLAY LCD

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define I2C_SDA 15
#define I2C_SCL 2

int lcdColumns = 16;
int lcdRows = 2;
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);

//incluyo librerías y defino variables para el Webhost y la conexion WiFi.

#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiManager.h>
#include <WiFiMulti.h>
#include <stdio.h>
#include <Separador.h>



//#define EEPROM_SIZE 60

//String mypass;
//String myssid;
////char pass[30] ="0AAD82AC98C77AB9";
////char ssid[30] ="HOME-29CF";
//char pass[30] = "milo2004";
//char ssid[30] = "MOVISTAR WIFI6786";
int address1 = 0;
int address2 = 30;
int contwifi = 0;

//WiFiManager wifiManager;

Separador separar;

int cuentaLecturas = 0;

int contadorSalida = 0;

String strhost = "trazabilidadmasterdent.online";

String strurl = "/control/interaccion_arduino.php?pre_php=";

char host[48];
int err;
int temp = 0;
float tempFloat;
float hum = 6;//este es el dato correspondiente a la estacion en la tabla3 del Webhost
String datos = "";
uint32_t chipID = 0;
String chipid = "";
float datatemp = 26;
float datahum = 42;
int ID = 1001;
String datagps = "6.169252;-75.590535";
String prueba = "prueba1001";
String P;//variable juegos de mala calidad
char str_val[6];
char str_final[16];
char str_code[8];//es posible que este dato no sea necesario con lectura real de RFID tag
unsigned long lastTime = 0;

byte proceso = 0; //proceso correspondiente a la actualización de la estación en la tabla rotulos2
int idMolde = 0;
byte casilla = 0;

String distancia = "";
String rotulo = "";
String idEmplaquetador = "";
int claseTag = 0; //1=molde, 2=rotulo.

int descontados = 0;
int salida = 0;
int asignados = 0;

byte idNombre = 0;



const byte pulsadorDerecha = 34;
const byte pulsadorIzquierda = 39;

WiFiClient client;

//int num_respuesta = 4;
//int num_respuesta2 = 3;

int num_respuesta = 1;
int num_respuesta2 = 4;

String respuesta9 = "";
String respId = "";
String respRef = "";


//const char* ssid = "PRODUCCION_MASTERDENT_EXT";
//const char* password = "M4ST3RD3NT2021.";

//const char* ssid = "Masterdent- wifi prod";
//const char* password = "Masterdent2022*";

const char* ssid = "Masterdent Bodega";
const char* password = "Masterdent2022";

//const char* ssid = "Masterdent1";
//const char* password = "milo2004";

//const char* ssid = "PRODUCCION_MASTERDENT";
//const char* password = "M4ST3RD3NT2021.";


// función para iniciar la conexión WiFi

void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");

  unsigned long timeout = millis();
  int j = 0;
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    lcd.setCursor(0, 0);
    lcd.print("Conectando WiFi");
    lcd.setCursor(j, 1);
    lcd.print(".");
    j++;
    digitalWrite(ledWiFi, HIGH);
    delay(100);
    digitalWrite(ledWiFi, LOW);
    delay(700);
    if (j == 16) {
      lcd.clear();
      j = 0;
    }
    if (millis() - timeout > 15000) {
      ESP.restart();
    }

  }
  Serial.print("IP local: ");
  Serial.println(WiFi.localIP());
}

//////////////////////////////////////////////////////////////////////////////////////

void setup() {

  pinMode(ledPin, OUTPUT);
  pinMode(ledWiFi, OUTPUT);
  pinMode(WIFI_PIN, INPUT);
  pinMode(tareButton, INPUT);


  Serial.begin (115200);
  Wire.begin(I2C_SDA, I2C_SCL);
  SPI.begin(); // inicio SPI bus

  lcd.init();
  lcd.backlight();



  //  //Init EEPROM
  //  EEPROM.begin(EEPROM_SIZE);
  //  leerEEPROM();
  //
  //  conectarWifi();


  //fragmento de código para calibración de báscula

  Serial.println("HX711 calibration sketch");
  Serial.println("Remove all weight from scale");
  Serial.println("After readings begin, place known weight on scale");
  Serial.println("Press + or a to increase calibration factor");
  Serial.println("Press - or z to decrease calibration factor");



  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(calibration_factor); //Adjust to this calibration factor
  scale.tare();  //Reset the scale to 0

  long zero_factor = scale.read_average(); //Get a baseline reading
  Serial.print("Zero factor: "); //This can be used to remove the need to tare the scale. Useful in permanent scale projects.
  Serial.println(zero_factor);


  initWiFi();

  Serial.println("");
  Serial.println("WiFi connected");
  digitalWrite (ledWiFi, HIGH);
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  SPI.begin(); // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522 card
  Serial.println(F("Sketch has been started!"));

  lcd.clear();
  Serial.println("");
  Serial.println("WiFi connected");
  lcd.setCursor(0, 0);
  lcd.print("WiFi conectado");



  //Serial.println("IP address: ");
  //Serial.println(WiFi.localIP());


  strhost.toCharArray(host, 49);



  for (int i = 0; i < 17; i = i + 8) {
    chipID |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
  }
  Serial.print("Chip ID: "); Serial.println(chipID);
  chipid = String(chipID);
  delay(1000);


  Serial.println("Seleccione la tarea a realizar");
  Serial.println("presione 'A' ingresar material a la nevera");
  Serial.println("presione 'B' sacar material de la nevera");
  Serial.println("presione 'C' eliminar material endurecido");
  Serial.println("presione A y B para crear nuevos tag de colores");
  memcpy(buffer, " ", 16);
  bool status;


  proceso = 14;
  consultarNombres();



  //ciclo para listar en el monitor Serial el nombre de los Trabajadores.
  /*
    for (byte i = 0; i < 50; i++) {

      Serial.print(i + 1);
      Serial.print("=");
      Serial.println(separar.separa(respuesta9, '*', i));

    }
  */


  leerIdNombre();

}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void loop() {

  //revisarWiFi();
  esperaEnvio();
  //leerTag();


}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



void leerTag() {
  //
  //

  int lectura = 0;
  salida = 0;
  //String datoTag;
  datoTag = "";
  cuentaEnvio = 0;//****************************
  char tecla;
  //si no hay un tag presente no haga nada.
  while (lectura == 0) {
    size = sizeof(buffer);

    leerTeclado();
    if (salida > 0) {
      return;
    }
    /*
      tecla  = teclado.getKey();
      if (tecla != NO_KEY) {
      if (tecla == 'D') {//cambio la tecla de salida a D(close)/ reservo la C para reportar calidad
        lectura++;
        cuentaEnvio++;
        salida++;
        leerMasa();
      }
    */
    /*
       en el momento no considero necesario activar la lectura de masa
       con otra tecla distinta a la de salir de la lectura
      else if (tecla == 'C') {
      leerMasa();
      }

      }
    */

    if ( mfrc522.PICC_IsNewCardPresent())
    {
      if ( mfrc522.PICC_ReadCardSerial())
      {
        //break;


        //a continuación se ejecuta leer
        // Read data ***************************************************
        Serial.println(F("leyendo datos ... "));
        //data in 4 block is readed at once.
        status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(pageAddr, buffer, &size);
        if (status != MFRC522::STATUS_OK) {
          Serial.print(F("MIFARE_Read() failed: "));
          Serial.println(mfrc522.GetStatusCodeName(status));
          return;
        }
        lcd.clear();
        Serial.print(F("Dato leído: "));
        cuentaLecturas = 1;
        lcd.setCursor(0, 0);
        lcd.print("Dato leido: ");
        //Dump a byte array to Serial
        for (byte i = 0; i < 10; i++) {
          Serial.write(buffer[i]);
          dataChar[i] = buffer[i];
          //codigoRFID+=String(char(buffer[i]));
          if (dataChar[i] != '\n') {//utilizo el \n para no rellenar el dato con el signo parecido a la E con ' ' se rellena
            lcd.setCursor(i, 1);
            lcd.print(dataChar[i]);
          }

          buffer[i] = ' ';


        }

        Serial.println();
        Serial.println("FIN DE LA LECTURA");

        digitalWrite(ledPin, HIGH);
        delay(700);
        digitalWrite(ledPin, LOW);
        lcd.clear();

        mfrc522.PICC_HaltA();//cierra la comunicación con el lector.
        mfrc522.PCD_StopCrypto1();//tomado del unificado de tarjeta común con 2 bloques.

        datoTag = String(dataChar);
        Serial.print("valor de datoTag=");
        Serial.println(datoTag);

        //después de que datoTag toma el valor de dataChar, vacío dataChar para que no conserve datos viejos

        for (byte i = 0; i < 10; i++) {
          dataChar[i] = '\0';
        }

        Serial.print("valor de datoTag=");
        Serial.println(datoTag);

        rotulo = datoTag;

        Serial.print("valor de la variable rotulo despues de la funcion char array to string = ");
        Serial.println (rotulo);

        lectura = 1;
        faltan = "";

      }

    }
    else {

      revisarWiFi();
      /*
        if (digitalRead(tareButton) == HIGH) {
        scale.tare();
        }
      */
      if (digitalRead(WIFI_PIN) == HIGH) {
        //hum = 2;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Enviando:");
        //lcd.setCursor(0, 1);
        //lcd.print("-->Acabado");
        cuentaEnvio++;
        digitalWrite(ledPin, HIGH);
        delay(700);
        digitalWrite(ledPin, LOW);
        Serial.print("rótulos enviados");
        Serial.println(rotulo);
        proceso = 8;
        distancia = idNombre;
        enviarDatos();
        //lcd.clear();
        return;

      }
      /*
            Serial.print("Reading: ");
            Serial.println(scale.get_units(), 0);
            //lcd.clear();

            lcd.setCursor(0, 1);
            //lcd.print(scale.get_units(), 0);
            masa = String(scale.get_units(), 0);

            lcd.print("   " + (String(scale.get_units(), 0)) + " gramos    ");
      */


    }
  }//cieerre de while no hay lectura

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void enviarDatos() {
  unsigned long timecontrol = millis();
  unsigned long deltatime = timecontrol - nowtime;
  //if (deltatime >= 5000)
  //{
  Serial.print("connecting to ");
  Serial.println(host);

  if (client.connect(host, 80)) {
    Serial.println("connected to server");

    //client.print("GET https://esp32sensoresiot.000webhostapp.com/control/conexion_arduino.php?pre_php="); // Enviamos los datos por GET
    client.print("GET https://trazabilidadmasterdent.online/control/interaccion_arduino.php?pre_php="); // Enviamos los datos por GET
    client.print(P);
    client.print("&hum_php=");
    client.print(hum, 2);
    client.print("&temp_php=");
    client.print(temp, DEC);
    client.print("&proceso_php=");
    client.print(proceso, DEC);
    client.print("&cuentaLecturas_php=");
    client.print(cuentaLecturas, DEC);
    client.print("&dist_php=");
    client.print(distancia);
    client.print("&rotulo_php=");
    client.print(rotulo);
    client.println(" HTTP/1.0");
    //client.println("Host: esp32sensoresiot.000webhostapp.com");
    client.println("Host: trazabilidadmasterdent.online");
    client.println("Connection: close");
    client.println();
    Serial.println("Envio con exito (al archivo controller/index y models/herramienta)");
    Serial.print("/ proceso:");
    Serial.print (proceso);
    Serial.print("- hum/estación:" );
    Serial.print (hum);
    Serial.print("-/juegos/temp/gramos:" );
    Serial.print(temp);
    Serial.print("-pre/idMolde: " );
    Serial.print(P );
    Serial.print("-dist/cod_molde/idEmplaquetador:" );
    Serial.print(distancia);
    Serial.print("-rotulo:");
    Serial.print(rotulo);
    Serial.print("-cuentaLecturas:");
    Serial.println(cuentaLecturas);
    lcd.setCursor(0, 0);
    lcd.print("Envio Exitoso");

    //"https://esp32sensoresiot.000webhostapp.com/control/conexion_arduino.php?pre_php=10.2&hum_php=20.3&temp_php=30.1&dist_php=cerca" 7/usar en postman.com
  }

  Serial.println("% send to Server");

  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 7000) {
      Serial.println(">>> Client Timeout !");
      ESP.restart();
      client.stop();
      client.flush();
      return;
    }
  }
  // Read all the lines of the reply from server and print them to Serial
  while (client.available()) {
    String line = client.readStringUntil('\r');
    Serial.println(line);
    //if (line.length() < 250) {
    num_molde = separar.separa(line, ',', num_respuesta);//1
    cant_moldes = separar.separa(line, ',', num_respuesta2);//4
    respuesta = cant_moldes;
    Serial.println("codigo obtenido");
    Serial.println(num_molde);
    //Serial.println("cantidad de moldes a asignar");
    //Serial.println(cant_moldes);

    if (cant_moldes == "rotuloOK,") {
      temp = 0;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Respuesta=");
      lcd.setCursor(0, 1);
      lcd.print("Ingreso Exitoso!");
      //      lcd.setCursor(0, 1);
      //      lcd.print(num_molde);
      digitalWrite(ledPin, LOW);
      delay(150);
      digitalWrite(ledPin, HIGH);
      delay(150);
      digitalWrite(ledPin, LOW);
      delay(150);
      digitalWrite(ledPin, HIGH);
      delay(150);
      digitalWrite(ledPin, LOW);
      delay(150);
      digitalWrite(ledPin, HIGH);
      delay(150);
      digitalWrite(ledPin, LOW);

    }
    else {
      //      lcd.clear();
      //      lcd.setCursor(0, 0);
      //      lcd.print("Respuesta=");
      //      lcd.setCursor(0, 1);
      //      lcd.print("------->      ");
    }

    //}
    /*else {
      Serial.println("la línea de respuesta es demasiado grande= ERROR");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("ERROR");
      ESP.restart();
      return;
      }
    */
  }

  //memset(buffer, 0, sizeof(buffer));
  memset(dataChar, 0, sizeof(dataChar));//vacío el dataChar

  if (num_molde.length() <= 10) { //si el dato recibido en la respuesta es de molde o rótulo lo guardo en dataChar.
    faltan = num_molde;
    num_molde.toCharArray(dataChar, 18);
    Serial.println("longitud de num_molde menor a 10");
    Serial.println("dato num_molde guardado en dataChar");
  }
  else {
    respuesta9 = num_molde;
    //Serial.println(respuesta9);
  }
  //memcpy(buffer, dataChar, 16); //guarda en el buffer el dato en arreglo. hacer esto cuando le presiono la b.
  Serial.println("wait 5 sec...");
  delay(500);//remplazar por el if de arriba.
  //distancia = "";
  //rotulo="";
  nowtime = timecontrol;

}


////////////////////////////////////////////////////////////////////////////////////


void consultarNombres() {
  unsigned long timecontrol = millis();
  unsigned long deltatime = timecontrol - nowtime;
  //if (deltatime >= 5000)
  //{
  Serial.print("connecting to ");
  Serial.println(host);

  if (client.connect(host, 80)) {
    Serial.println("connected to server");

    //client.print("GET https://esp32sensoresiot.000webhostapp.com/control/conexion_arduino.php?pre_php="); // Enviamos los datos por GET
    client.print("GET https://trazabilidadmasterdent.online/control/interaccion_arduino.php?pre_php="); // Enviamos los datos por GET
    client.print(P);
    client.print("&hum_php=");
    client.print(hum, 2);
    client.print("&temp_php=");
    client.print(temp, DEC);
    client.print("&proceso_php=");
    client.print(proceso, DEC);
    client.print("&dist_php=");
    client.print(distancia);
    client.print("&rotulo_php=");
    client.print(rotulo);
    client.println(" HTTP/1.0");
    //client.println("Host: esp32sensoresiot.000webhostapp.com");
    client.println("Host: trazabilidadmasterdent.online");
    client.println("Connection: close");
    client.println();
    Serial.println("Envio con exito (al archivo controller/index y models/herramienta)");
    //lcd.setCursor(0, 0);
    //lcd.print("Envio Exitoso");

    //"https://esp32sensoresiot.000webhostapp.com/control/conexion_arduino.php?pre_php=10.2&hum_php=20.3&temp_php=30.1&dist_php=cerca" 7/usar en postman.com
  }

  Serial.println("% send to Server");

  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 7000) {
      Serial.println(">>> Client Timeout !");
      ESP.restart();
      client.stop();
      client.flush();
      return;
    }
  }
  // Read all the lines of the reply from server and print them to Serial
  while (client.available()) {
    String line = client.readStringUntil('\r');
    Serial.println(line);
    //if (line.length() < 250) {
    num_molde = separar.separa(line, ',', num_respuesta);

    respuesta = cant_moldes;
    Serial.println("codigo obtenido");
    Serial.println(num_molde);

  }

  //memset(buffer, 0, sizeof(buffer));
  memset(dataChar, 0, sizeof(dataChar));//vacío el dataChar


  respuesta9 = num_molde;

  Serial.println(respuesta9);

  if (respuesta9 != "") {
    lcd.setCursor(0, 0);
    lcd.print("Identifiquese!");
  }
  else {
    ESP.restart();
  }
  //memcpy(buffer, dataChar, 16); //guarda en el buffer el dato en arreglo. hacer esto cuando le presiono la b.
  Serial.println("wait 5 sec...");
  delay(500);//remplazar por el if de arriba.
  //distancia = "";
  //rotulo="";
  nowtime = timecontrol;
}

////////////////////////////////////////////////////////////////////////////////////

void revisarWiFi() {
  int j = 0;
  if (WiFi.status() != WL_CONNECTED) {
    digitalWrite(ledWiFi, LOW);
    WiFi.begin(ssid, password);

    unsigned long timeout = millis();
    while (WiFi.status() != WL_CONNECTED) {
      digitalWrite(ledWiFi, HIGH);
      delay(100);
      digitalWrite(ledWiFi, LOW);
      delay(700);
      Serial.print(".");
      lcd.setCursor(0, 0);
      lcd.print("Conectando WiFi");
      lcd.setCursor(j, 1);
      lcd.print(".");
      j++;
      if (j == 16) {
        lcd.clear();
        j = 0;
      }
      if (millis() - timeout > 15000) {
        ESP.restart();
      }

    }
    Serial.println("");
    Serial.println("WiFi connected");
    digitalWrite (ledWiFi, HIGH);
  }
  else {
    digitalWrite(ledWiFi, HIGH);
    return;
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void leerTeclado() {
  char tecla ;

  tecla  = teclado.getKey();
  if (tecla != NO_KEY) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("ID=");

    if (tecla == '*') {

      if (contadorTeclado > 0) {

        contadorTeclado--;
        int largoString;
        largoString = idProduccionString.length();
        idProduccionString.remove(largoString - 1);
        lcd.setCursor(4, 0);
        lcd.print(idProduccionString);
        lcd.setCursor(contadorTeclado + 4 , 0);
        lcd.print(" ");

      }
      else {

      }
    }
    else if (tecla == '#') {
      //aquí guardo el string en un rótulo,
      idEmplaquetador = datoTag;//tiene significado en la función de leerNombres
      rotulo = datoTag;
      cuentaLecturas = 1;
      contadorTeclado = 0;
      idProduccionString = "";
      faltan = "";


      lcd.setCursor(4, 0);
      lcd.print(datoTag + "->OK");
      Serial.print("valor del rótulo después de digitar uno = ");
      Serial.println(rotulo);
    }
    else if (tecla == 'D') {
      //rotulo = datoTag;
      cuentaLecturas = 1;
      contadorTeclado = 0;
      //idProduccionString = "";
      leerMasa();
      //rotulo = "";
      lcd.clear();
      //return;
    }
    else if (tecla == 'C') {
      //rotulo = datoTag;
      cuentaLecturas = 1;
      contadorTeclado = 0;
      //idProduccionString = "";
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("J.Descartados");
      //descuentaJuegosPorCalidad();
      registrarCalidadPorMasa();
      //rotulo = "";


      //return;
    }

    else if (tecla == 'B') {
      //rotulo = datoTag;
      cuentaLecturas = 1;
      contadorTeclado = 0;
      //idProduccionString = "";
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("J.Descartados");
      descuentaJuegosPorCalidad();
      //registrarCalidadPorMasa();
      //rotulo = "";


      //return;
    }
    else {
      idProduccion[contadorTeclado] = tecla;
      lcd.setCursor(4, 0);
      lcd.print(idProduccionString);
      lcd.setCursor(contadorTeclado + 4 , 0);
      lcd.print(tecla);


      if (contadorTeclado > 6) {
        idProduccion [contadorTeclado] = '\0';
      }

      idProduccionString += idProduccion[contadorTeclado];
      Serial.print ("idProduccionString");
      Serial.print(" = ");
      Serial.println (idProduccionString);
      contadorTeclado++;

    }

  }
  //datoTag =idProduccionString;
  datoTag = idProduccionString;

}

//*************************************************************

void esperaEnvio() {
  lcd.clear();
  cuentaEnvio = 0;


  while (cuentaEnvio == 0) {


    //      Serial.print(" gramos"); //Change this to kg and re-adjust the calibration factor if you follow SI units like a sane person
    //      Serial.print(" calibration_factor: ");
    //      Serial.print(calibration_factor);
    //      Serial.print(" Dato RFID");
    //      Serial.print(datoTag);
    //      Serial.print(" Variable masa: ");
    //      Serial.print(masa);
    //      Serial.println();

    lcd.setCursor(0, 0);
    lcd.print("Rotulo=");
    lcd.setCursor(7, 0);
    lcd.print(rotulo);
    if (faltan != "") {
      lcd.setCursor(0, 1);
      lcd.print("Faltan " + faltan + " cajas");
    }

    leerTag();
  }
}
////////////////////////////////////////////

void leerIdNombre() {
  //
  //

  int lectura = 0;

  //String datoTag;
  datoTag = "";

  //si no hay un tag presente no haga nada.
  while (lectura == 0) {
    size = sizeof(buffer);

    if ( mfrc522.PICC_IsNewCardPresent())
    {
      if ( mfrc522.PICC_ReadCardSerial())
      {

        //a continuación se ejecuta leer
        // Read data ***************************************************
        Serial.println(F("leyendo datos ... "));
        //data in 4 block is readed at once.
        status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(pageAddr, buffer, &size);
        if (status != MFRC522::STATUS_OK) {
          Serial.print(F("MIFARE_Read() failed: "));
          Serial.println(mfrc522.GetStatusCodeName(status));
          return;
        }
        lcd.clear();
        Serial.print(F("Dato leído: "));

        for (byte i = 0; i < 10; i++) {
          Serial.write(buffer[i]);
          dataChar[i] = buffer[i];
          //codigoRFID+=String(char(buffer[i]));
          if (dataChar[i] != '\n') {//utilizo el \n para no rellenar el dato con el signo parecido a la E con ' ' se rellena

          }

          buffer[i] = ' ';

        }

        Serial.println();
        Serial.println("FIN DE LA LECTURA");

        digitalWrite(ledPin, HIGH);
        delay(700);
        digitalWrite(ledPin, LOW);
        lcd.clear();

        mfrc522.PICC_HaltA();//cierra la comunicación con el lector.
        mfrc522.PCD_StopCrypto1();//tomado del unificado de tarjeta común con 2 bloques.

        datoTag = String(dataChar);
        Serial.print("valor de datoTag=");
        Serial.println(datoTag);


        //después de que datoTag toma el valor de dataChar, vacío dataChar para que no conserve datos viejos

        for (byte i = 0; i < 10; i++) {
          dataChar[i] = '\0';
        }

        Serial.print("valor de datoTag=");
        Serial.println(datoTag);



        idEmplaquetador = datoTag;


        Serial.print("valor de la variable id emplaquetador despues de la funcion char array to string = ");
        Serial.println (idEmplaquetador);

        lectura = 1;

      }

    }
    else {
      idEmplaquetador = "";
      leerTeclado();
      if (idEmplaquetador != "") {
        cuentaLecturas = 0;
        rotulo = "";
        lectura = 1;
      }
      else {

      }

      revisarWiFi();


    }
  }//cieerre de while no hay lectura

  idNombre = String(idEmplaquetador).toInt();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("¡Bienvenid@!");
  lcd.setCursor(0, 1);
  if (idNombre == 0) {

  }
  else {
    lcd.print(separar.separa(respuesta9, '*', idNombre - 1));
  }

  delay(1500);
  lcd.clear();


}

/////////////////////////////////////////////////////77

void leerMasa() {
  //cuentaEnvio=0;
  int lecturaMasa = 0; //variable para indicar el envío de un peso a producto a granel
  // se modifica al presionar botón de envío.

  while (lecturaMasa == 0) {
    Serial.print("Reading: ");
    Serial.println(scale.get_units(), 0);
    //lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("Rotulo=");
    lcd.setCursor(7, 0);
    lcd.print(rotulo);
    lcd.setCursor(0, 1);
    //lcd.print(scale.get_units(), 0);
    masa = String(scale.get_units(), 0);
    temp = String(masa).toInt();

    lcd.print("Sobra: " + (String(scale.get_units(), 0)) + " gr      ");

    if (digitalRead(tareButton) == HIGH) {
      scale.tare();
      unsigned long tareTime = millis();
      while (digitalRead(tareButton) == HIGH) {

        if (millis() - tareTime > 1200) {
          Serial.println(">>> menú principal !");
          //ESP.restart();
          //client.stop();
          //client.flush();
          lecturaMasa++;
          salida++;
          return;
        }
      }
    }

    if (digitalRead(WIFI_PIN) == HIGH) {
      //hum = 2;
      lecturaMasa++;
      salida++;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Enviando:");
      //lcd.setCursor(0, 1);
      //lcd.print("-->Acabado");
      //cuentaEnvio++;
      digitalWrite(ledPin, HIGH);
      delay(700);
      digitalWrite(ledPin, LOW);
      Serial.print("rótulos enviados");
      Serial.println(rotulo);
      proceso = 13;
      distancia = idNombre;

      enviarDatos();
      rotulo = "";
      faltan = "";
      //lcd.clear();
      //return;

    }

  }
}

/////////////////////////////////////////////////////77

void descuentaJuegosPorCalidad() {
  //cuentaEnvio=0;
  lecturaJuegos = 0; //variable para indicar el envío de un peso a producto a granel
  // se modifica al presionar botón de envío.

  while (lecturaJuegos == 0) {


    TecladoDescuentaJuegos();

    if (digitalRead(WIFI_PIN) == HIGH) {
      //hum = 2;
      lecturaJuegos++;
      salida++;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Enviando:");
      //lcd.setCursor(0, 1);
      //lcd.print("-->Acabado");
      //cuentaEnvio++;
      digitalWrite(ledPin, HIGH);
      delay(700);
      digitalWrite(ledPin, LOW);
      Serial.print("rótulos enviados");
      Serial.println(rotulo);
      proceso = 8;
      P = juegosParaRestar;
      distancia = idNombre;

      enviarDatos();
      //rotulo = "";
      faltan = "";
      juegosParaRestar = "";
      P = "";
      lcd.clear();
      //return;
      leerMasa();
      //rotulo = "";
      lcd.clear();

    }

  }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void TecladoDescuentaJuegos() {
  char tecla ;

  tecla  = teclado.getKey();
  if (tecla != NO_KEY) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("J.Descartados=");
    //lcd.setCursor(0, 1);
    //lcd.print("=");

    if (tecla == '*') {

      if (contadorTeclado > 0) {

        contadorTeclado--;
        int largoString;
        largoString = idProduccionString.length();
        idProduccionString.remove(largoString - 1);
        lcd.setCursor(5, 1);
        lcd.print(idProduccionString);
        lcd.setCursor(contadorTeclado + 5 , 1);
        lcd.print(" ");

      }
      else {

      }
    }
    else if (tecla == '#') {
      //aquí guardo el string en un rótulo,
      juegosParaRestar = datoTag;//tiene significado en la función de leerNombres
      datoTag = "";
      cuentaLecturas = 1;
      contadorTeclado = 0;
      idProduccionString = "";




      lcd.setCursor(5, 1);
      lcd.print(juegosParaRestar + "->OK");
      Serial.print("Juegos de dientes a restar por mala calidad ");
      Serial.println(juegosParaRestar);
    }

    else if (tecla == 'A') {
      datoTag = "";
      lecturaJuegos++;
      salida++;
      contadorTeclado = 0;
      idProduccionString = "";
      lcd.clear();



    }


    else {
      idProduccion[contadorTeclado] = tecla;
      lcd.setCursor(5, 1);
      lcd.print(idProduccionString);
      lcd.setCursor(contadorTeclado + 5, 1);
      lcd.print(tecla);


      if (contadorTeclado > 6) {
        idProduccion [contadorTeclado] = '\0';
      }

      idProduccionString += idProduccion[contadorTeclado];
      Serial.print ("idProduccionString");
      Serial.print(" = ");
      Serial.println (idProduccionString);
      contadorTeclado++;

    }

  }
  //datoTag =idProduccionString;
  datoTag = idProduccionString;

}

/////////////////////////////////////////////////////77

void registrarCalidadPorMasa() {
  //cuentaEnvio=0;
  masa = "";
  String masaSobra = "";
  int lecturaMasa = 0; //variable para indicar el envío de un peso a producto a granel
  // se modifica al presionar botón de envío.
  String problemaDeCalidad = "";
  while (lecturaMasa <= 4) {
    Serial.print("Reading: ");
    Serial.println(scale.get_units(), 0);
    //lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("Rotulo=");
    lcd.setCursor(7, 0);
    lcd.print(rotulo);
    lcd.setCursor(0, 1);
    //lcd.print(scale.get_units(), 0);


    //temp = String(masa).toInt();
    if (lecturaMasa == 0) {
      problemaDeCalidad = "Beta";
    }
    else if (lecturaMasa == 1) {
      problemaDeCalidad = "Sucio";
    }
    else if (lecturaMasa == 2) {
      problemaDeCalidad = "Plast";
    }
    else if (lecturaMasa == 3) {
      problemaDeCalidad = "Otros";
    }
    else {
      problemaDeCalidad = "Sobra";
    }
    lcd.print(problemaDeCalidad + ": " + (String(scale.get_units(), 0)) + " gr       ");

    if (digitalRead(tareButton) == HIGH) {
      scale.tare();
      unsigned long tareTime = millis();
      while (digitalRead(tareButton) == HIGH) {

        if (millis() - tareTime > 1200) {
          Serial.println(">>> menú principal !");
          //ESP.restart();
          //client.stop();
          //client.flush();
          lecturaMasa = 5;
          salida++;
          lcd.clear();
          return;
        }
      }
    }

    if (digitalRead(WIFI_PIN) == HIGH) {
      //hum = 2;
      if (lecturaMasa >= 4) {
        lecturaMasa++;
        salida++;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Enviando:");
        //lcd.setCursor(0, 1);
        //lcd.print("-->Acabado");
        //cuentaEnvio++;
        digitalWrite(ledPin, HIGH);
        delay(700);
        digitalWrite(ledPin, LOW);
        Serial.print("rótulos enviados");
        Serial.println(rotulo);
        proceso = 15;
        distancia = idNombre;
        P = masa;
        masaSobra = String(scale.get_units(), 0);
        temp = String(masaSobra).toInt();
        enviarDatos();
        masa = "";
        rotulo = "";
        faltan = "";
        P = "";
        lcd.clear();
        //return;
        //leerMasa();
        //rotulo = "";
        //lcd.clear();
      }
    }

    char tecla ;

    tecla  = teclado.getKey();
    if (tecla != NO_KEY) {
      lcd.setCursor(13, 1);
      lcd.print("OK!");
      if (lecturaMasa<4){
      lecturaMasa++;
      delay(500);
      if (masa == "") {
        masa = String(scale.get_units(), 0);
      }
      else {
        masa = masa + "$" + String(scale.get_units(), 0);
      }
    }
    }

  }
}
