/**
    Rutina principal que obtiene los valores sensados y los modula
    y transmite sobre LoRa@433MHz en una string especialmente diseñada.
    @file nodo-sisicic.ino
    @author Franco Abosso
    @author Julio Donadello
    @version 1.1 29/03/2021
*/

/// Headers iniciales (preceden a la declaración de variables).

// Header que contiene constantes relevantes al accionar de este programa.
#include "constants.h"          // Biblioteca propia.

// Bibliotecas necesarias para manejar al SX1278.
#include <SPI.h>                // https://www.arduino.cc/en/reference/SPI
#include <LoRa.h>               // https://github.com/sandeepmistry/arduino-LoRa

// Bibliotecas necesarias para manejar al DS18B20.
#include <OneWire.h>            // https://www.pjrc.com/teensy/td_libs_OneWire.html
#include <DallasTemperature.h>  // https://www.milesburton.com/Dallas_Temperature_Control_Library

// Biblioteca necesaria para calcular los valores RMS de los sensores de tensión y corriente.
#include <EmonLib.h>            // https://learn.openenergymonitor.org/electricity-monitoring/ctac/how-to-build-an-arduino-energy-monitor

// Biblioteca necesaria para reservar espacios de memoria fijos para las Strings utilizadas.
#include <StringReserveCheck.h> // https://www.forward.com.au/pfod/ArduinoProgramming/ArduinoStrings/index.html

/// Declaración de variables globales.

/**
    voltages es un array de floats que contiene los valores de tensión medidos entre cada
    transmisión LoRa. El tamaño del array depende del intervalo de tiempo entre cada transmisión
    LoRa y el intervalo de tiempo entre cada medición.
    Una vez realizada la transmisión, todos los valores de este array vuelven a ponerse en 0.
*/
float voltages[ARRAY_SIZE] = {0.0};

/**
    temperatures es un array de floats que contiene los valores de temperatura medidos entre cada
    transmisión LoRa. El tamaño del array depende del intervalo de tiempo entre cada transmisión
    LoRa y el intervalo de tiempo entre cada medición.
    Una vez realizada la transmisión, todos los valores de este array vuelven a ponerse en 0.
*/
float temperatures[ARRAY_SIZE] = {0.0};

/**
    index es una variable de tipo int utilizado para recorrer los arrays de medición.
    Una vez realizada la transmisión, vuelve a ponerse en 0.
*/
int index = 0;

/**
    dayTime es un flag que se pone en true o en false remotamente.
    Si dayTime es false, significa que hay luz ambiental exterior, por lo que no
    es necesario apagar la luz cuando se detecte la apertura de la puerta.
*/
bool dayTime = true;

/**
    presenciaChanged es un flag que se pone en true o false dependendiendo
    del cambio de estado del sensor de presencia.
*/
bool presenciaChanged = false;

/**
    presenciaLast es un flag que almacena el último cambio de estado del sensor
    de preresencia
*/
bool presenciaLast = false;

/**
    refreshRequested contiene SENSORS_QTY variables booleanas que representan la necesidad
    de refrescar los valores de los arrays de medición. Estos tienen un orden arbitrario:
    { Tensión, Temperatura }
    Una vez refrescado, cada uno de estos booleanos vuelve a ponerse en false.
*/
bool refreshRequested[SENSORS_QTY] = {false, false};

/**
    outcomingFull es una string que contiene el mensaje LoRa de salida preformateado especialmente
    para que, posteriormente, el concentrador LoRa pueda decodificarla.
*/
String outcomingFull;

/**
    incomingFull es una string que contiene el mensaje LoRa de entrada, incluyendo
    el identificador de nodo.
*/
String incomingFull;

/**
    greaterThanStr (>) es el delimitador entre el identificador de nodo y la carga útil
    del mensaje de entrada LoRa. Se carga en memoria una única vez en una String constante
    para evitar problemas de memoria heap.
*/
const String greaterThanStr = ">";

/**
    receiverStr es una string que sólo contiene el identificador de nodo 
    recibido en un mensaje LoRa entrante.
*/
String receiverStr;

/**
    incomingPayload es una string que contiene sólo la carga útil del mensaje LoRa de entrada,
    utilizada sólo cuando el identificador de nodo coincide con DEVICE_ID o con BROADCAST_ID.
*/
String incomingPayload;

/**
    knownCommands es un array de Strings que contiene los comandos LoRa que
    se pueden ejecutar.
*/
const String knownCommands[KNOWN_COMMANDS_SIZE] = {
    "startAlert",   // inicia una alerta con el siguiente llamado a función: startAlert(750, 10);
    "daytime",      // alerta a la cabina que es de día.
    "nighttime"    // alerta a la cabina que es de noche.
};

/// Headers finales (proceden a la declaración de variables).

#include "pinout.h"             // Biblioteca propia.
#include "alerts.h"             // Biblioteca propia.
#include "timing_helpers.h"     // Biblioteca propia.
#include "sensors.h"            // Biblioteca propia.
#include "actuators.h"          // Biblioteca propia.
#include "array_helpers.h"      // Biblioteca propia.
#include "LoRa_helpers.h"       // Biblioteca propia.

/// Funciones principales.

/**
    setup() lleva a cabo las siguientes tareas:
        - setea el pinout,
        - inicializa el periférico serial,
        - reserva espacios de memoria para las Strings,
        - inicializa el módulo LoRa.
    Si después de realizar estas tareas no se "cuelga", da inicio
    a una alerta "exitosa".
*/
void setup() {
    setupPinout();
    #if DEBUG_LEVEL >= 1
        Serial.begin(SERIAL_BPS);
    #endif
    reserveMemory();
    LoRaInitialize();
    startAlert(133, 3);
}

/**
    loop() determina las tareas que cumple el programa:
        - cada LORA_TIMEOUT segundos, envía un payload LoRa.
        - si no está ocupado con eso:
            - se ocupa de disparar las alertas preestablecidas.
            - cada 2 segundos, refresca el estado de los sensores.
    Esta función se repite hasta que se le dé un reset al programa.
*/
void loop() {
    if (runEvery(sec2ms(LORA_TIMEOUT), 1)) {
        // Deja de refrescar TODOS los sensores.
        stopRefreshingAllSensors();

        // Compone la carga útil de LoRa.
        composeLoRaPayload(voltages, temperatures, outcomingFull);

        #if DEBUG_LEVEL >= 1
            Serial.print("Payload LoRa encolado!: ");
            Serial.println(outcomingFull);
        #endif

        // Componer y enviar paquete.
        LoRa.beginPacket();
        LoRa.print(outcomingFull);
        LoRa.endPacket();

        // Pone al módulo LoRa en modo recepción.
        LoRa.receive();

        // Inicia la alerta preestablecida.
        startAlert(133, 3);

        // Reestablece los arrays de medición.
        cleanupArray(voltages, ARRAY_SIZE);
        cleanupArray(temperatures, ARRAY_SIZE);

        // Reestablece el index de los arrays de medición.
        index = 0;
    }

    callbackAlert();
    callbackLoRaCommand();
    callbackPresencia();
    callbackLights();


    if(runEvery(sec2ms(TIMEOUT_READ_SENSORS), 2)) {
        // Refresca TODOS los sensores.
        refreshAllSensors();
        index++;
    }

    if (refreshRequested[0] && !resetAlert && pitidosRestantes == 0) {
        // Obtiene un nuevo valor de tensión.
        #ifndef TENSION_MOCK
            eMon.calcVI(EMON_CROSSINGS, EMON_TIMEOUT);
        #endif
        getNewVoltage();
    }
    if (refreshRequested[1] && !resetAlert && pitidosRestantes == 0) {
        // Obtiene un nuevo valor de temperatura.
        getNewTemperature();
    }
}
