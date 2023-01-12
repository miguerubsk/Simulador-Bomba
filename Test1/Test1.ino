/*
 * Copyright (C) 2023 Miguel González García
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * 
 * Autor: Miguel González García
 * 
 */

#include <SPI.h>      // incluye libreria bus SPI
#include <MFRC522.h>  // incluye libreria especifica para MFRC522
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>  //Librería para controlar el LCD mediante pines I2C

#define RST_PIN 9       // constante para referenciar pin de reset
#define SS_PIN 10       // constante para referenciar pin de slave select
#define TIEMPO_MAX 100  //Constante que determina el temporizador de la bomba

MFRC522 mfrc522(SS_PIN, RST_PIN); // crea objeto mfrc522 enviando pines de slave select y reset

uint8_t LecturaUID[4]; // crea array para almacenar el UID leido
uint8_t activador[4] = {0x63, 0x7B, 0xEB, 0xA0}; // UID del activador
uint8_t desactivador[4] = {0x73, 0x35, 0xB0, 0x0E}; // UID del desactivador
int tiempo; //Tiempo en segundos para la explosión
boolean activa = false; //Estado por defecto de la bomba
int ini, fin;
LiquidCrystal_I2C lcd(0x3F, 16, 2); //Crear el objeto lcd  dirección  0x3F y 16 columnas x 2 filas

void setup() {
    Serial.begin(9600); // inicializa comunicacion por monitor serie a 9600 bps
    SPI.begin(); // inicializa bus SPI
    mfrc522.PCD_Init(); // inicializa modulo lector
    tiempo = TIEMPO_MAX;
    ini = millis();

    lcd.init(); // Inicializar el LCD


    lcd.backlight(); //Encender la luz de fondo.
    lcd.print("Listo");
}

void loop() {

    if (NFCRead()) {
        switch (comprobarUID(LecturaUID)) {
            case 1:
                activa = true;
                break;
            case 0:
                activa = false;
                break;
            default:
                break;
        }
    }
    if (activa) {
        if (tiempo <= 0) {
            activa = false;
            lcd.setCursor(0, 1);
            lcd.print("BUUUUM!!!!!");
            tiempo = TIEMPO_MAX;
        } else {
            fin = millis();
            if (fin - ini >= 1000) {
                lcd.setCursor(0, 1);
                lcd.println(tiempo--);
                ini = fin;
            }
        }

    }
}

bool NFCRead() {
    memset(LecturaUID, 0, sizeof (uint8_t)*4);
    if (!mfrc522.PICC_IsNewCardPresent())
        return false;

    if (!mfrc522.PICC_ReadCardSerial())
        return false;
    lcd.setCursor(0, 0);
    lcd.print("UID:");
    for (int i = 0; i < mfrc522.uid.size; i++) {
        if (mfrc522.uid.uidByte[i] < 0x10) {
            lcd.print(" 0");
        } else {
            lcd.print(" ");
        }
        lcd.print(mfrc522.uid.uidByte[i], HEX);
        LecturaUID[i] = mfrc522.uid.uidByte[i];
    }
    //Serial.print("\n");
    mfrc522.PICC_HaltA();
    return true;
}

int comprobarUID(byte lectura[]) {
    bool esActivador = true;
    bool esDesactivador = true;
    for (uint8_t i = 0; i < mfrc522.uid.size; i++) {
        if (lectura[i] != activador[i])
            esActivador = false;
    }
    for (uint8_t i = 0; i < mfrc522.uid.size; i++) {
        if (lectura[i] != activador[i])
            esDesactivador = false;
    }


    if (esActivador)
        return 1;
    if (esDesactivador)
        return 0;
    return -1;
}
