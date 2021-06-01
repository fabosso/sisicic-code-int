/**
    Header que indica el pinout de los nodos LoRa.
    @file pinout.h
    @author Franco Abosso
    @author Julio Donadello
    @version 1.6 29/03/2021
*/

/*
        Pinout simplificado del nodo.
        Fuente: http://busyducks.com/ascii-art-arduinos
                         +-----+
            +------------| USB |------------+
            |            +-----+            |
            | [ ]D13/SCK        MISO/D12[ ] | - Reservado para RA-02.
            | [ ]3.3V           MOSI/D11[ ]~| - Reservado para RA-02.
            | [ ]V.ref     ___    SS/D10[ ]~| - Reservado para RA-02.
RS232 (1) - | [ ]A0       / N \       D9[ ]~| - RS232 (1).
RS232 (1) - | [ ]A1      /  A  \      D8[ ] | - RS232 (1).
RS232 (1) - | [ ]A2      \  N  /      D7[ ] | - RS232 (1).
RS232 (2) - | [ ]A3       \_0_/       D6[ ]~| - Cable SparkOn.
RS232 (2) - | [ ]A4/SDA               D5[ ]~| - Cable SparkOn.
RS232 (2) - | [ ]A5/SCL               D4[ ] | - Reservado para RA-02.
RS232 (2) - | [ ]A6              INT1/D3[ ]~| - Reservado para RA-02.
RS232 (2) - | [ ]A7              INT0/D2[ ] | - Reservado para RA-02.
            | [ ]5V                  GND[ ] |
            | [ ]RST                 RST[ ] |
            | [ ]GND   5V MOSI GND   TX1[ ] |  - RS232 (2).
            | [ ]Vin   [ ] [ ] [ ]   RX1[ ] |  - RS232 (2).
            |          [ ] [ ] [ ]          |
            |          MISO SCK RST         |
            |                               |
            +-------------------------------+
*/

// Pinout RA-02.
#define NSS_PIN 10
#define RESET_PIN -1
#define DIO0_PIN 2
#define DIO1_PIN 3
#define DIO2_PIN 4
#define RXTX_PIN -1

/*
    Sensores y actuadores del nodo interior:
        - Puerto RS232 (1):
            - Sensor de tensión = A1.
            - Sensor de temperatura = D8.
            - Actuador buzzer (y LED) = D9.
            - Actuador iluminación cabina = D7.
*/

// Pinout sensores y actuadores.
#define TENSION_PIN A1
#define BUZZER_PIN 9
// #define TEMPERATURA_PIN 8
#define RELE_PIN 7
#define PRESENCIA_ECHO_PIN 6
#define PRESENCIA_TRIG_PIN 5

// Definición de modo de trabajo.
#define BUZZER_ACTIVO HIGH
#define BUZZER_INACTIVO LOW
#define RELE_ACTIVO HIGH        // definido según jumper de módulo
#define RELE_INACTIVO LOW       // definido según jumper de módulo
#define PRESENCIA_ACTIVO HIGH
#define PRESENCIA_ACTIVO LOW

// Instanciamiento de objetos relacionados al pinout.
EnergyMonitor eMon;
#ifdef TEMPERATURA_PIN
    OneWire oneWireObjeto(TEMPERATURA_PIN);
    DallasTemperature sensorDS18B20(&oneWireObjeto);
#endif
NewPing sonar(PRESENCIA_TRIG_PIN, PRESENCIA_ECHO_PIN, 300);

/**
    setupPinout() determina las I/Os digitales, inicializa al DS18B20
    y calibra el módulo sensor de tensión.
*/
void setupPinout() {
    #ifdef BUZZER_PIN
        pinMode(BUZZER_PIN, OUTPUT);
    #endif
    #ifdef TEMPERATURA_PIN
        pinMode(TEMPERATURA_PIN, INPUT);
    #endif
    #ifdef RELE_PIN
        pinMode(RELE_PIN, OUTPUT);
    #endif
    #ifdef PRESENCIA_TRIG_PIN
        pinMode(PRESENCIA_TRIG_PIN, OUTPUT);
    #endif
    #ifdef PRESENCIA_ECHO_PIN
        pinMode(PRESENCIA_ECHO_PIN, INPUT);
    #endif

    digitalWrite(BUZZER_PIN, BUZZER_INACTIVO);
    digitalWrite(RELE_PIN, RELE_ACTIVO);

    #ifdef TEMPERATURA_PIN
        sensorDS18B20.begin();
    #endif
    eMon.voltage(TENSION_PIN, 226.0, 1.7);
}
