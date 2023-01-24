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

#include <Keypad.h>
#include <LiquidCrystal_I2C.h>


//Constantes 
const uint8_t ROWS = 4; //4 filas
const uint8_t COLS = 3; //3 columnas
const int ledPin = 13;
const int Buzzer = 10;
const int tonos[] = {261, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494};
const int countTonos = 10;
const int passLength = 4;

//Variables temporizador
int Scount = 15; // Conteo regresivo de Segundos
int Mcount = 00; // Conteo regresivo de Minutos
int Hcount = 00; // Conteo regresivo de Horas
long secMillis = 0; // Guarda ultimo time para sumar segundos, referencia para llegando a fin de conteo
long interval = 1000; // Intervalo de tiempo para los segundos
char HHkey[1]; //Almacena temporalmente la hora en vector de dos posiciones
char MMkey[1]; //Almacena temporalmente los minutos en vector de dos posiciones
char SSkey[1]; //Almacena temporalmente los segundos en vector de dos posiciones

//Variables teclado y clave
char password[passLength]; // Longitud del PW: 4 caracteres
int currentLength = 0; //Lleva la longitud del numero que se está escribiendo actualmente 
char entered[passLength]; //número ingresado
uint8_t colPins[COLS] = {5, 4, 3};
uint8_t rowPins[ROWS] = {9, 8, 7, 6};
char keys[ROWS][COLS] = {
    {'6', '5', '4'},
    {'3', '2', '1'},
    {'9', '8', '7'},
    {'*', '0', '#'},
};

//Otras variables
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
unsigned long duracion = 100; //Controla el tiempo que dura el tono de avance de los segundos
LiquidCrystal_I2C lcd(0x3F, 16, 2);

void sonido() {
    for (int iTono = 0; iTono < countTonos; iTono++) {
        tone(Buzzer, tonos[iTono]);
        delay(50);
    }
    noTone(Buzzer);
}

void setup() {
    pinMode(Buzzer, OUTPUT);
    pinMode(ledPin, OUTPUT); // LED que pulsa con cada segundo 
    digitalWrite(LED_BUILTIN, HIGH); // turn the LED on (HIGH is the voltage level)
    tone(Buzzer, 100);
    delay(500); // wait for a second
    tone(Buzzer, 600);
    delay(500); // wait for a second
    noTone(Buzzer);
    digitalWrite(LED_BUILTIN, LOW); // turn the LED off by making the voltage LOW
    Serial.begin(9600);
    lcd.init();
    lcd.backlight();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Batallón 32");
    lcd.setCursor(0, 1);
    lcd.print("Strike Bomb");
    delay(3000);
    sonido();

    //**********************hh
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Enter Countdown");
    lcd.setCursor(0, 1);
    lcd.print("Hours: 00");
    lcd.setCursor(9, 1);

    while (currentLength < 2) {
        lcd.setCursor(currentLength + 7, 1); //mueva el cursos a medida que se digita
        lcd.cursor();
        char key = keypad.getKey(); //guarde el valor de la tecla digitada en la variable key
        key == NO_KEY;
        if (key != NO_KEY) {
            if ((key != '*') && (key != '#')) {
                lcd.print(key);
                Serial.print("Opción: * ó #");
                Serial.println(key);
                HHkey[currentLength] = key;
                currentLength++;
                digitalWrite(LED_BUILTIN, HIGH); // turn the LED on (HIGH is the voltage level)
                tone(Buzzer, 250, 100);
                delay(200);
                digitalWrite(LED_BUILTIN, LOW); // turn the LED off by making the voltage LOW
            }
        }
    }
    if (currentLength == 2) {
        delay(500);
        lcd.noCursor();
        lcd.clear();
        lcd.home();
        lcd.print("Hours: ");
        lcd.setCursor(6, 1);
        lcd.print(HHkey[0]);
        Serial.print(HHkey[0]);
        lcd.print(HHkey[1]);
        Serial.print(HHkey[1]);
        Serial.println();
        Serial.println(HHkey);
        Hcount = 10 * (int(HHkey[0]) - 48) + (int(HHkey[1] - 48));
        Serial.println(Hcount);
        delay(2000);
        lcd.clear();
        currentLength = 0;
    }
    //**********************
    //**********************
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Enter Countdown");
    lcd.setCursor(0, 1);
    lcd.print("Minutes: 00");
    lcd.setCursor(11, 1);
    while (currentLength < 2) {
        lcd.setCursor(currentLength + 9, 1);
        lcd.cursor();
        char key = keypad.getKey();
        key == NO_KEY;
        if (key != NO_KEY) {
            if ((key != '*') && (key != '#')) {
                lcd.print(key);
                MMkey[currentLength] = key;
                currentLength++;
                digitalWrite(LED_BUILTIN, HIGH); // turn the LED on (HIGH is the voltage level)
                tone(Buzzer, 250, 100);
                delay(200);
                digitalWrite(LED_BUILTIN, LOW); // turn the LED off by making the voltage LOW
            }
        }
    }
    if (currentLength == 2) {
        delay(500);
        lcd.noCursor();
        lcd.clear();
        lcd.home();
        lcd.print("Minutes: ");
        lcd.setCursor(6, 1);
        lcd.print(MMkey[0]);
        lcd.print(MMkey[1]);
        Mcount = 10 * (int(MMkey[0]) - 48) + (int(MMkey[1] - 48));
        delay(2000);
        lcd.clear();
        currentLength = 0;
    }
    //******************
    //**********************ss
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Enter Countdown");
    lcd.setCursor(0, 1);
    lcd.print("Seconds: 00");
    lcd.setCursor(11, 1);
    while (currentLength < 2) {
        lcd.setCursor(currentLength + 9, 1);
        lcd.cursor();
        char key = keypad.getKey();
        key == NO_KEY;
        if (key != NO_KEY) {
            if ((key != '*') && (key != '#')) {
                lcd.print(key);
                SSkey[currentLength] = key;
                currentLength++;
                digitalWrite(LED_BUILTIN, HIGH); // turn the LED on (HIGH is the voltage level)
                tone(Buzzer, 250, 100);
                delay(200);
                digitalWrite(LED_BUILTIN, LOW); // turn the LED off by making the voltage LOW
            }
        }
    }
    if (currentLength == 2) {
        delay(500);
        lcd.noCursor();
        lcd.clear();
        lcd.home();
        lcd.print("Seconds: ");
        lcd.setCursor(6, 1);
        lcd.print(SSkey[0]);
        lcd.print(SSkey[1]);
        Scount = 10 * (int(SSkey[0]) - 48) + (int(SSkey[1] - 48));
        delay(2000);
        lcd.clear();
        currentLength = 0;
    }
    //******************
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Enter defusing ");
    lcd.setCursor(0, 1);
    lcd.print("Code: ");
    while (currentLength < 4) {
        lcd.setCursor(currentLength + 6, 1);
        lcd.cursor();
        char key = keypad.getKey();
        key == NO_KEY;
        if (key != NO_KEY) {
            if ((key != '*') && (key != '#')) {
                lcd.print(key);
                password[currentLength] = key;
                currentLength++;
                digitalWrite(LED_BUILTIN, HIGH); // turn the LED on (HIGH is the voltage level)
                tone(Buzzer, 250, 100);
                delay(200);
                digitalWrite(LED_BUILTIN, LOW); // turn the LED off by making the voltage LOW
            }
        }
    }
    if (currentLength == passLength) {
        delay(500);
        lcd.noCursor();
        lcd.clear();
        lcd.home();
        lcd.print("You've Entered: ");
        lcd.setCursor(6, 1);
        lcd.print(password[0]);
        lcd.print(password[1]);
        lcd.print(password[2]);
        lcd.print(password[3]);
        delay(2000);
        lcd.clear();
        currentLength = 0;
    }
}

void loop() {
    timer(); //conteo regresivo
    char key2 = keypad.getKey(); // get the key

    if (key2 == '*') // en el caso de querer desarmar la bomba
    {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Code: ");
        while (currentLength < passLength) {
            timer();
            char key2 = keypad.getKey();
            if (key2 == '#') {
                currentLength = 0;
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print(F("Code: "));
            } else if (key2 != NO_KEY) {
                lcd.setCursor(currentLength + 7, 0);
                lcd.cursor();
                lcd.print(key2);
                entered[currentLength] = key2;
                currentLength++;
                digitalWrite(LED_BUILTIN, HIGH); // turn the LED on (HIGH is the voltage level)
                tone(Buzzer, 250, 100);
                delay(200);
                digitalWrite(LED_BUILTIN, LOW); // turn the LED off by making the voltage LOW
                lcd.noCursor();
                lcd.setCursor(currentLength + 6, 0);
                lcd.print("*");
                lcd.setCursor(currentLength + 7, 0);
                lcd.cursor();
            }
        }
        if (currentLength == passLength) {
            if (entered[0] == password[0] && entered[1] == password[1] && entered[2] == password[2] && entered[3] == password[3]) {
                lcd.noCursor();
                lcd.clear();
                lcd.home();
                lcd.print("Bomb Defused"); // Bomba desarmada
                currentLength = 0;
                delay(2000);
                lcd.setCursor(0, 1);
                lcd.print("Reset the Bomb");
                delay(1000000);
            } else {
                lcd.noCursor();
                lcd.clear();
                lcd.home();
                lcd.print("Wrong Password!");
                if (Hcount > 0) {
                    Hcount = 0; //Hcount = Hcount - 1;
                }

                if (Mcount > 0) {
                    Mcount = 0; //Mcount = Mcount - 59;
                }
                if (Scount > 0) {
                    Scount = 5; //Scount = Scount - 59;
                }
                delay(1500);
                currentLength = 0;
            }
        }
    }
}

void timer() {
    if (Hcount <= 0) {
        if (Mcount < 0) {
            lcd.noCursor();
            lcd.clear();
            lcd.home();
            lcd.print("The Bomb Has ");
            lcd.setCursor(0, 1);
            lcd.print("Exploded!");
            while (Mcount < 0) {
                tone(Buzzer, 650);
                digitalWrite(13, HIGH); //LED ON
                delay(1000);
                delay(2000);
            }
        }
    }
    lcd.setCursor(0, 1);
    lcd.print("Timer:");
    Serial.println("Timer: ");
    if (Hcount >= 10) {
        lcd.setCursor(7, 1);
        lcd.print(Hcount);
        Serial.println(Hcount);
    }
    if (Hcount < 10) {
        lcd.setCursor(7, 1);
        lcd.write('0');
        lcd.setCursor(8, 1);
        lcd.print(Hcount);
    }
    lcd.print(":");
    if (Mcount >= 10) {
        lcd.setCursor(10, 1);
        lcd.print(Mcount);
    }
    if (Mcount < 10) {
        lcd.setCursor(10, 1);
        lcd.write('0');
        lcd.setCursor(11, 1);
        lcd.print(Mcount);
    }
    lcd.print(":");
    if (Scount >= 10) {
        lcd.setCursor(13, 1);
        lcd.print(Scount);
    }
    if (Scount < 10) {
        lcd.setCursor(13, 1);
        lcd.write('0');
        lcd.setCursor(14, 1);
        lcd.print(Scount);
    }
    if (Hcount < 0) {
        Hcount = 0;
    }
    if (Mcount < 0) {
        Hcount--;
        Mcount = 59;
    }
    if (Scount < 1) { // Si es 60 ejecutar
        Mcount--; // sume 1 a Mcount
        Scount = 59; // reiniciar Scount
    }
    if (Scount > 0) { // Hacer esto 59 veces
        unsigned long currentMillis = millis();
        if (currentMillis - secMillis > interval) {
            secMillis = currentMillis;
            Scount--;
            if ((Scount < 40) && (Mcount == 0) && (Hcount == 0)) {
                duracion = (((900 / -39) * Scount + 1023));
                tone(Buzzer, 650, duracion);
                digitalWrite(13, HIGH); //LED ON
                delay(duracion);
                if (Scount == 5) {
                    delay(1000);
                }
            } else if ((Scount >= 40) || (Mcount > 0) || (Hcount > 0)) {
                tone(Buzzer, 650, 100); // Milis: 100 -
                digitalWrite(13, HIGH); //LED ON
                delay(30);
            }
            delay(50);
            noTone(Buzzer);
            digitalWrite(13, LOW); //LED OFF
        }
    }
}
