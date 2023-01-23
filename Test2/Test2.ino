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
#include <Wire.h> 
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>  //Librería para controlar el LCD mediante pines I2C

#define RST_PIN 9       // constante para referenciar pin de reset
#define SS_PIN 10       // constante para referenciar pin de slave select
#define TIEMPO_MAX 100  //Constante que determina el temporizador de la bomba


//Variables para la bomba
int tiempo; //Tiempo en segundos para la explosión
boolean activa = false; //Estado por defecto de la bomba
int ini, fin;

//Variables del LCD
LiquidCrystal_I2C lcd(0x3F, 16, 2); //Crear el objeto lcd  dirección  0x3F y 16 columnas x 2 filas


//Variables del teclado
const uint8_t FILAS = 4; //Número de filas del teclado
const uint8_t COLUMNAS = 4; //Número de columnas del teclado
char keys[FILAS][COLUMNAS] = {// define la distribucion de teclas
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};
uint8_t pinesFilas[FILAS] = {9, 8, 7, 6};
uint8_t pinesColumnas[COLUMNAS] = {5, 4, 3, 2};
Keypad teclado = Keypad(makeKeymap(keys), pinesFilas, pinesColumnas, FILAS, COLUMNAS);
char teclaPulsada;
char input[5];
char claveActivacion[5] = "CLAVE";
char claveDesactivacion[5] = "CLAVE";
uint8_t indice;

bool first = true;

void setup() {
    Serial.begin(9600); // inicializa comunicacion por monitor serie a 9600 bps
    SPI.begin(); // inicializa bus SPI
    tiempo = TIEMPO_MAX;
    ini = millis();
    indice = 0;
    lcd.init(); // Inicializar el LCD

    lcd.backlight(); //Encender la luz de fondo.
    lcd.print("Listo");


}

void loop() {

    //if(first){
    //    lcd.print("Intruduzca la");
    //    lcd.setCursor(0,1);
    //    lcd.print("clave de desacti");
    //    teclaPulsada = teclado.getKey();
    //    
    //    if(tecla){
    //      
    //      if(indice == 0){
    //        lcd.clear();
    //        lcd.setCursor(0, 0);
    //      }
    //      
    //      claveDesactivacion[indice] = teclaPulsada;
    //      indice++;
    //      lcd.print(teclapulsada);
    //      if(indice >= 4)
    //    }
    //}else{
    //  if (activa) {
    //        if (tiempo <= 0) {
    //            activa = false;
    //            lcd.setCursor(0, 1);
    //            lcd.print("BUUUUM!!!!!");
    //            tiempo = TIEMPO_MAX;
    //        } else {
    //            fin = millis();
    //            if (fin - ini >= 1000) {
    //                lcd.setCursor(0, 1);
    //                lcd.println(tiempo--);
    //                ini = fin;
    //            }
    //        }
    //
    //    }
    //}
    teclaPulsada = teclado.getKey(); // obtiene tecla presionada y asigna a variable
    if (teclaPulsada) { // comprueba que se haya presionado una tecla
        input[indice] = teclaPulsada; // almacena en array la tecla presionada
        indice++; // incrementa indice en uno
        lcd.print(teclaPulsada); // envia a lcd la tecla presionada
    }

    if (indice >= 4) { // si ya se almacenaron los 6 digitos
        switch (comprobarClave(input)) {
            case 0:
                activa = true;
                ini = millis();
                break;
            case 1:
                activa = false;
                reset();
                break;
            default:
                break;
        }
        indice = 0;
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

void reset(){
  tiempo = TIEMPO_MAX;
}

int comprobarClave(char input) {
    if (strcmp(input, claveDesactivacion) == 0) {
        return 1;
    }
    if (strcmp(input, claveActivacion) == 0) {
        return 0;
    }
    return -1;
}
