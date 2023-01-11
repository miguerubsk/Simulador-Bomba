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
#include <LiquidCrystal.h>    // importa libreria

int rs = 7;
int e = 6;
int d5 = 4;
int d4 = 5;
int d6 = 3;
int d7 = 2;

LiquidCrystal lcd(rs, e, d4, d5, d6, d7);  // pines RS, E, D4, D5, D6, D7 de modulo 1602A

void setup() {
  lcd.begin(16, 2);     // inicializa a display de 16 columnas y 2 lineas
}

void loop() {
  lcd.setCursor(0, 0);      // ubica cursor en columna 0, linea 0
  lcd.print("FRAN MARICA");  // escribe el texto en pantalla
  lcd.setCursor(0, 1);      // ubica cursor en columna 0, linea 1
  lcd.print(millis() / 1000);   // escribe valor en segundos devuelto por funcion millis()
  lcd.print(" Segundos.");     // imprime a continuacion segundos
}
