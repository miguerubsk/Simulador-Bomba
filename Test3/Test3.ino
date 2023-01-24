//Version 2.0 Temporizador Arduino + LCD + ENCODER + EEPROM
//Suscribete a mi canal de Youtube y de esta manera apoyas mi trabajo http://www.youtube.com/subscription_center?add_user=cmarinv2005
#include <EEPROM.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0X27, 16, 2);

//#include <LiquidCrystal.h>             // Libreria del display LCD 
//LiquidCrystal lcd(12, 11, 5, 4, 3, 2); // Inicializamos la libreria con todos los pines a utilizar

//Parametros Encoder
const int salida_A = 2; //CLK 
const int salida_B = 3; //DT
const int swPin = 4; //SW

//Variables
int contador = 0; //Esta variable en 0 se posicionara en el primer elemento del Menu principal
int A_estado_actual;
int A_ultimo_estado;
int SW_estado_actual;
int SW_ultimo_estado;
int velocidad_scroll = 300;

int relevo = 10; //Carga

int varstar = 0; //Variable para iniciar el temporizador manualmente
int varautomatico = 0; //Variable para iniciar el temporizador automaticamente 0=off 1=on
int vareeprom = 0; //Variable para usar la Eeprom 0=off 1=on

int ahoras = 0;
int aminutos = 0;
int asegundos = 0;
int temp_horas = 0;
int temp_minutos = 0;
int temp_segundos = 0;

int dirh = 0; //Variable para almacenar la direccion de horas de la memoria EEPROM
int dirm = 1; //Variable para almacenar la direccion de minutos de la memoria EEPROM
int dirs = 2; //Variable para almacenar la direccion de segundos de la memoria EEPROM
int dirflag = 3; //Variable para usar como bandera para saber si debo leer datos de EEPROM
int dirautomatico = 4; //Variable para almacenar la direccion de configuracion de inicio automatico en la memoria EEPROM 
int direeprom = 5; //Variable para almacenar la direccion de configuracion de guardar en la EEPROM
int valida = 0;

int segundos = 0;
unsigned long milisegundos = 0;
unsigned long tiempo1 = 0;
unsigned long tiempoSegundos = 0;
int fin = 0;

//::::::::::MENU::::::::::::::::::::::

String menu1[] = {"Horas", "Minutos", "Segundos", "Comenzar", "Automatico", "Eeprom", "Reset", "Informacion"}; //Inicializamos nuestro Array con los elementos del menu
int sizemenu1 = sizeof (menu1) / sizeof (menu1[0]); //Obtenemos el número de elementos ocupados en la matriz. en este caso 8     

String menu2[] = {"Configurar", "Atras"};
int sizemenu2 = sizeof (menu2) / sizeof (menu2[0]);

String menu3[] = {"Si", "Atras"};
int sizemenu3 = sizeof (menu3) / sizeof (menu3[0]);

String menu4[] = {"Si", "No", "Atras"};
int sizemenu4 = sizeof (menu4) / sizeof (menu4[0]);

String menu5[] = {"Automatico", "Eeprom", "Tiempo", "Atras"};
int sizemenu5 = sizeof (menu5) / sizeof (menu5[0]);

String linea1, linea2; //Lineas del LCD
int seleccion = 0;
int pos = 0;
int level_menu = 0; //Iniciamos la variable en el menu principal 0 --> 7
int opcion = 0;
bool btnpress = false; //Esta variable de retorno en false asegurando que el boton del Encoder aun no se ha oprimido
byte flecha[] = {B10000, B11000, B11100, B11110, B11100, B11000, B10000, B00000}; //Creamos un array de 8 posiciones para la flecha del menu

void setup() {

    Serial.begin(9600); //Iniciamos conexion Serial Usb 

    pinMode(salida_A, INPUT);
    pinMode(salida_B, INPUT);
    pinMode(swPin, INPUT_PULLUP); //Esta entrada siempre estara conectada a positivo a traves de resistencia interna
    pinMode(relevo, OUTPUT);
    digitalWrite(relevo, LOW); //Aseguramos carga desconectada antes de iniciar el temporizador
    lcd.init();
    lcd.backlight(); //Encender la luz de fondo.
    lcd.createChar(0, flecha); //Caracter personalizado       

    vareeprom = EEPROM.read(direeprom); // Obtiene el dato int de EEPROM y lo envía a la variable vareeprom

    valida = EEPROM.read(dirflag); //Obtiene dato de dirflag para comprobar si debo leer la EEPROM
    if (valida == 1) {
        if (vareeprom == 1) { //Leemos la EEPROM si tenemos habilitada esa configuracion
            ahoras = EEPROM.read(dirh); // Obtiene el dato int de EEPROM y lo envía a la variable ahoras
            aminutos = EEPROM.read(dirm); // Obtiene el dato int de EEPROM y lo envía a la variable aminutos
            asegundos = EEPROM.read(dirs); // Obtiene el dato int de EEPROM y lo envía a la variable asegundos   
            varautomatico = EEPROM.read(dirautomatico); // Obtiene el dato int de EEPROM y lo envía a la variable varautomatico       
        }
    }

    fn_creditos(); //Presentamos mensaje de bienvenida
    fn_menu(contador, menu1, sizemenu1); //Iniciamos presentando el menu principal como contador=0 mostrara el primer elemento del Menu
    A_ultimo_estado = digitalRead(salida_A); //Leemos el estado de la salida del Encoder usando el pin CLK

}

void loop() {
    //Ahora, pasaremos a leer los datos que mandan los pulsadores, que seran los que programen el tiempo
    //Esto lo haremos con la orden "do", para que entre en bucle y este leyendo continuamente, hasta que 
    //terminemos de ajustar el tiempo y le demos al pulsador de arranque del Encoder
    do {

        selectOption(); //Funcion para detectar cuando se oprime el encoder.  btnpress == true


        //Menu 1 {"Horas","Minutos","Segundos","Comenzar","Automatico","Eeprom","Reset","Informacion"}

        if (level_menu == 0) { //Esta variable corresponde al nivel principal del menu. level_menu = 0.

            if (fn_encoder(sizemenu1)) { //Esta funcion muestra en el LCD el menu en el que estamos
                fn_menu(contador, menu1, sizemenu1); //Esta funcion muestra la posicion dentro de ese menu segun el valor de la variable contador
            }

            if (btnpress) { //Verificamos si el boton del encoder fue oprimido. btnpress == true
                //Horas     
                if (contador == 0) {
                    contador = 0; //Seleccionamos la posicion donde quedara la flecha de seleccion. On, Off, Atras
                    fn_menu(contador, menu2, sizemenu2); //Vamos a la funcion que nos muestra las opciones para el Led 1
                    level_menu = 1; //Esta variable corresponde al nivel secundario del menu. level_menu = 1.                  
                }

                //Minutos
                if (contador == 1) {
                    contador = 0;
                    fn_menu(contador, menu2, sizemenu2);
                    level_menu = 2;
                }

                //Segundos
                if (contador == 2) {
                    contador = 0;
                    fn_menu(contador, menu2, sizemenu2);
                    level_menu = 3;
                }

                //Comenzar
                if (contador == 3) {
                    contador = 0;
                    fn_menu(contador, menu3, sizemenu3);
                    level_menu = 4;
                }

                //Automatico
                if (contador == 4) {
                    contador = 0;
                    fn_menu(contador, menu4, sizemenu4);
                    level_menu = 5;
                }

                //Eeprom
                if (contador == 5) {
                    contador = 0;
                    fn_menu(contador, menu4, sizemenu4);
                    level_menu = 6;
                }

                //Reset
                if (contador == 6) {
                    contador = 0;
                    fn_menu(contador, menu3, sizemenu3);
                    level_menu = 7;
                }

                //Informacion
                if (contador == 7) {
                    contador = 0;
                    fn_menu(contador, menu5, sizemenu5);
                    level_menu = 8;
                }

                btnpress = false; //  Nos aseguramos que esta variable de retorno de la funcion selectOption() vuelva a su estado inicial
            }

        }

        //SubMenu Horas {"Configurar","Atras"}

        if (level_menu == 1) {

            if (fn_encoder(sizemenu2)) { //Nos desplazamos con el encoder seleccionando las diferentes opciones
                fn_menu(contador, menu2, sizemenu2);
            }

            if (btnpress) { //Verificamos si el boton del encoder fue oprimido. btnpress == true

                if (contador == 0) {

                    contador = ahoras;
                    lcd.clear(); //Borramos la pantalla
                    lcd.setCursor(0, 0);
                    lcd.print("Ajuste el Tiempo ");

                    do {

                        fn_programar_tiempo();

                    } while (digitalRead(swPin) == HIGH); //Instrucciones despues del do se repetirán mientras esta condición sea cierta
                    delay(500);
                    temp_horas = ahoras; //Guardo la configuracion de horas en una variable temporal

                    if (vareeprom == 1) {
                        EEPROM.update(dirh, ahoras); // Se agrega a EEPROM con la dirección primero y valor de la variable después si el dato ha cambiado
                    } //de esta forma ahorramos ciclos de esritura en la EEPROM

                    contador = 1;
                }

                if (contador == 1) {
                    contador = 0; //Posicion del menu donde fue llamado "horas"
                    fn_menu(contador, menu1, sizemenu1);
                    level_menu = 0;
                }

                btnpress = false;

            }

        }

        //SubMenu Minutos {"Configurar","Atras"}

        if (level_menu == 2) {

            if (fn_encoder(sizemenu2)) { //Nos desplazamos con el encoder seleccionando las diferentes opciones
                fn_menu(contador, menu2, sizemenu2);
            }

            if (btnpress) { //Verificamos si el boton del encoder fue oprimido. btnpress == true

                if (contador == 0) {

                    contador = aminutos;
                    lcd.clear(); //Borramos la pantalla
                    lcd.setCursor(0, 0);
                    lcd.print("Ajuste el Tiempo ");

                    do {

                        fn_programar_tiempo();

                    } while (digitalRead(swPin) == HIGH); //Instrucciones despues del do se repetirán mientras esta condición sea cierta
                    delay(500);
                    temp_minutos = aminutos; //Guardo la configuracion de horas en una variable temporal

                    if (vareeprom == 1) {
                        EEPROM.update(dirm, aminutos); // Se agrega a EEPROM con la dirección primero y valor de la variable después si el dato ha cambiado
                    }

                    contador = 1;

                }

                if (contador == 1) {
                    contador = 1; //Posicion del menu donde fue llamado "minutos"
                    fn_menu(contador, menu1, sizemenu1);
                    level_menu = 0;
                }

                btnpress = false;

            }

        }

        //SubMenu Segundos {"Configurar","Atras"}

        if (level_menu == 3) {

            if (fn_encoder(sizemenu2)) { //Nos desplazamos con el encoder seleccionando las diferentes opciones
                fn_menu(contador, menu2, sizemenu2);
            }

            if (btnpress) { //Verificamos si el boton del encoder fue oprimido. btnpress == true

                if (contador == 0) {

                    contador = asegundos;
                    lcd.clear(); //Borramos la pantalla
                    lcd.setCursor(0, 0);
                    lcd.print("Ajuste el Tiempo ");

                    do {

                        fn_programar_tiempo();

                    } while (digitalRead(swPin) == HIGH); //Instrucciones despues del do se repetirán mientras esta condición sea cierta
                    delay(500);
                    temp_segundos = asegundos; //Guardo la configuracion de horas en una variable temporal

                    if (vareeprom == 1) {
                        EEPROM.update(dirs, asegundos); // Se agrega a EEPROM con la dirección primero y valor de la variable después si el dato ha cambiado
                    }

                    contador = 1;

                }

                if (contador == 1) {
                    contador = 2; //Posicion del menu donde fue llamado "segundos"
                    fn_menu(contador, menu1, sizemenu1);
                    level_menu = 0;
                }

                btnpress = false;

            }

        }

        //SubMenu Comenzar {"Si","Atras"}

        if (level_menu == 4) {

            if (fn_encoder(sizemenu3)) { //Nos desplazamos con el encoder seleccionando las diferentes opciones
                fn_menu(contador, menu3, sizemenu3);
            }

            fn_mostrar_tiempo();

            if (btnpress) { //Verificamos si el boton del encoder fue oprimido. btnpress == true

                if (contador == 0) {


                    varstar = 1;
                    contador = 1; //Iniciamos el temporizador manualmente 
                }

                if (contador == 1) {
                    contador = 3;
                    fn_menu(contador, menu1, sizemenu1);
                    level_menu = 0;
                }

                btnpress = false;

            }

        }

        //SubMenu Automatico {"Si","No","Atras"}

        if (level_menu == 5) {

            if (fn_encoder(sizemenu4)) { //Nos desplazamos con el encoder seleccionando las diferentes opciones
                fn_menu(contador, menu4, sizemenu4);
            }

            if (btnpress) { //Verificamos si el boton del encoder fue oprimido. btnpress == true

                if (contador == 0) {
                    lcd.setCursor(8, 0);
                    lcd.print("Guardado");
                    delay(1000);
                    lcd.setCursor(8, 0);
                    lcd.print("        ");
                    varautomatico = 1; //Activamos la bandera de temporizador automatico
                    if (vareeprom == 1) {
                        EEPROM.update(dirautomatico, varautomatico); // Se agrega a EEPROM con la dirección primero y valor de la variable después si el dato ha cambiado
                    }
                }

                if (contador == 1) {
                    lcd.setCursor(8, 1);
                    lcd.print("Guardado");
                    delay(1000);
                    lcd.setCursor(8, 1);
                    lcd.print("        ");
                    varautomatico = 0; //Desactivamos la bandera de temporizador automatico
                    if (vareeprom == 1) {
                        EEPROM.update(dirautomatico, varautomatico); // Se agrega a EEPROM con la dirección primero y valor de la variable después si el dato ha cambiado
                    }
                }

                if (contador == 2) {
                    contador = 4;
                    fn_menu(contador, menu1, sizemenu1);
                    level_menu = 0;
                }

                btnpress = false;

            }

        }

        //SubMenu Eeprom {"Si","No","Atras"}

        if (level_menu == 6) {

            if (fn_encoder(sizemenu4)) { //Nos desplazamos con el encoder seleccionando las diferentes opciones
                fn_menu(contador, menu4, sizemenu4);
            }

            if (btnpress) { //Verificamos si el boton del encoder fue oprimido. btnpress == true

                if (contador == 0) {
                    vareeprom = 1; //Activamos la bandera de guardar en la Eeprom
                    EEPROM.update(direeprom, vareeprom); // Se agrega a EEPROM con la dirección primero y valor de la variable después si el dato ha cambiado   
                }

                if (contador == 1) {
                    vareeprom = 0; //Desactivamos la bandera de guardar en la Eeprom
                    EEPROM.update(direeprom, vareeprom); // Se agrega a EEPROM con la dirección primero y valor de la variable después si el dato ha cambiado
                }

                if (contador == 2) {
                    contador = 5;
                    fn_menu(contador, menu1, sizemenu1);
                    level_menu = 0;
                }

                btnpress = false;

            }

        }


        //SubMenu Reset {"Si","Atras"}

        if (level_menu == 7) {

            if (fn_encoder(sizemenu3)) { //Nos desplazamos con el encoder seleccionando las diferentes opciones
                fn_menu(contador, menu3, sizemenu3);
            }

            if (btnpress) { //Verificamos si el boton del encoder fue oprimido. btnpress == true

                if (contador == 0) {
                    asm("jmp 0x0000"); //Linea de codigo en Assembler para reiniciar el Arduino              
                }

                if (contador == 1) {
                    contador = 6;
                    fn_menu(contador, menu1, sizemenu1);
                    level_menu = 0;
                }
                btnpress = false;
            }

        }

        //SubMenu Informacion {"Automatico","Eeprom","Tiempo","Atras"}

        if (level_menu == 8) {

            if (fn_encoder(sizemenu5)) { //Nos desplazamos con el encoder seleccionando las diferentes opciones
                fn_menu(contador, menu5, sizemenu5);
            }

            if (btnpress) { //Verificamos si el boton del encoder fue oprimido. btnpress == true

                if (contador == 0) {
                    if (varautomatico == 1) {
                        lcd.setCursor(13, 0);
                        lcd.print("ON");
                    } else {
                        lcd.setCursor(13, 0);
                        lcd.print("OFF");
                    }
                }

                if (contador == 1) {
                    if (vareeprom == 1) {
                        lcd.setCursor(13, 1);
                        lcd.print("ON");
                    } else {
                        lcd.setCursor(13, 1);
                        lcd.print("OFF");
                    }
                }

                if (contador == 2) {
                    lcd.setCursor(8, 0);
                    if (ahoras < 10) { //Esto es para poner un 0 delante si las horas son menor de 10
                        lcd.print("0");
                    }
                    lcd.print(ahoras);
                    lcd.print(":");

                    if (aminutos < 10) { //Esto es para poner un 0 delante si los minutos son menor de 10
                        lcd.print("0");
                    }
                    lcd.print(aminutos);
                    lcd.print(":");

                    if (asegundos < 10) { //Esto es para poner un 0 delante si los segundos son menor de 10
                        lcd.print("0");
                    }
                    lcd.print(asegundos);
                }

                if (contador == 3) {
                    contador = 7;
                    fn_menu(contador, menu1, sizemenu1);
                    level_menu = 0;
                }

                btnpress = false;

            }

        }

    } while (varstar == 0);

    if (varstar == 1) { //Temporizador automatico o iniciado manualmente

        lcd.clear();

        lcd.setCursor(4, 1);
        if (ahoras < 10) { //Esto es para poner un 0 delante si las horas son menor de 10
            lcd.print("0");
        }
        lcd.print(ahoras);
        lcd.print(":");

        if (aminutos < 10) { //Esto es para poner un 0 delante si los minutos son menor de 10
            lcd.print("0");
        }
        lcd.print(aminutos);
        lcd.print(":");

        if (asegundos < 10) { //Esto es para poner un 0 delante si los segundos son menor de 10
            lcd.print("0");
        }
        lcd.print(asegundos);

        temp_horas = ahoras; //Guardo la configuracion de horas en una variable temporal
        temp_minutos = aminutos; //Guardo la configuracion de minutos en una variable temporal
        temp_segundos = asegundos; //Guardo la configuracion de segundos en una variable temporal

        valida = 1;
        EEPROM.update(dirflag, valida); // Se escribe la direccion 3 de la EEPROM con el valor 1 si el valor es diferente de 1

        lcd.setCursor(0, 0);
        lcd.print("                "); // Limpio la primer fila del LCD con 16 espacios en blanco        

        //..........................................................................................................................    
        //contador de segundos
        //..........................................................................................................................     

        do {

            digitalWrite(relevo, HIGH); //Activamos la carga mientras dure el contador

            selectOption(); //Con esto aseguramos estar leyendo el pulsador del Encoder mientras esta en funcionamiento el temporizador

            if (btnpress) {
                delay(500);
                btnpress = false;
                selectOption(); //Vuelvo a leer el Encoder
                if (btnpress) { //Si mantengo oprimido el pulsador del Encoder reinicio el Arduino
                    btnpress = false;
                    digitalWrite(relevo, LOW); //Aseguramos carga desconectada antes de reiniciar el Arduino
                    asm("jmp 0x0000"); //Codigo en Assembler para reiniciar el Arduino 
                } else { //Si oprimo el pulsador del Encoder por menos de 1 segundo pauso el temporizador 
                    lcd.setCursor(0, 0);
                    lcd.print(" Tiempo Pausado ");
                    btnpress = false;
                    delay(250);
                    fn_pausar(); //Temporizador pausado   
                }
            }

            milisegundos = millis();
            if (milisegundos > (tiempo1 + 1000)) { //Si ha pasado 1 segundo ejecuta el IF
                tiempo1 = millis(); //Actualiza el tiempo actual
                segundos++; //Incrementa segundos
                tiempoSegundos = tiempo1 / 1000; //Tiempo transcurrido desde que esta encendido el Arduino
            }

            if (segundos == 1) {
                lcd.setCursor(0, 0);
                lcd.print("Tiempo Restante");
                segundos = 0;
                if (asegundos > 0) {
                    asegundos--;
                    lcd.setCursor(10, 1);
                    if (asegundos < 10) { //Esto es para poner un 0 delante si los segundos son menor de 10
                        lcd.print("0");
                    }
                    lcd.print(asegundos);
                }
                if (asegundos == 0) {
                    if (aminutos > 0) {
                        aminutos--;
                        asegundos = 60;
                        lcd.setCursor(7, 1);
                        if (aminutos < 10) { //Esto es para poner un 0 delante si los minutos son menor de 10
                            lcd.print("0");
                        }
                        lcd.print(aminutos);
                    }
                }
                if (asegundos == 0) {
                    if (aminutos == 0) {
                        if (ahoras > 0) {
                            ahoras--;
                            aminutos = 60;
                            lcd.setCursor(4, 1);
                            if (ahoras < 10) { //Esto es para poner un 0 delante si las horas son menor de 10
                                lcd.print("0");
                            }
                            lcd.print(ahoras);
                        }
                    }
                }
            }

            if (ahoras == 0) {
                if (aminutos == 0) {
                    if (asegundos == 0) {
                        digitalWrite(relevo, LOW); //Carga desconectada al finalizar el tiempo
                        fin = 1;
                    }
                }
            }

        } while (fin == 0);
        fin = 0; //Aseguramos en 0 la bandera que indica que finalizo el temporizador
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("T. Finalizado");
        delay(2000);
        ahoras = temp_horas; // recupero tiempo programado de la variable temporal horas
        aminutos = temp_minutos; // recupero tiempo programado de la variable temporal minutos
        asegundos = temp_segundos; // recupero tiempo programado de la variable temporal segundos

        if (varautomatico == 1) { //Activamos inicio automatico si tenemos habilitada la configuracion
            varstar = 1;
        } else {
            varstar = 0; //Aseguramos que inicio automatico se encuentre deshabilitado
            contador = 0; //Dejamos listo el Submenu para iniciar otra vez el temporizador
            fn_menu(contador, menu3, sizemenu3); //Nos situamos en el SubMenu Comenzar/Si
            level_menu = 4;
        }

        fn_mostrar_tiempo(); //Escribimos en el LCD la configuracion de tiempo que tiene el temporizador       
    }
}

//..........................................................................................................................    
//Funciones adicionales
//..........................................................................................................................

void selectOption() {
    if (digitalRead(swPin) == LOW) {
        delay(500);
        btnpress = true;
    }
}

void fn_programar_tiempo() {

    A_estado_actual = digitalRead(salida_A);

    if (A_estado_actual != A_ultimo_estado) {
        if (digitalRead(salida_B) != A_estado_actual) { //DT != CLK     
            contador++;
            delay(250); ///Retardo para evitar rebotes
        } else {
            contador--;
            delay(250);
        }
    }

    if (level_menu == 1) { //Estamos configurando horas
        ahoras = contador;
        if (ahoras > 99) {
            contador = 0;
            ahoras = 0;
        }
        if (ahoras < 0) {
            contador = 99;
            ahoras = 99;
        }
    }

    if (level_menu == 2) { //Estamos configurando minutos
        aminutos = contador;
        if (aminutos > 59) {
            contador = 0;
            aminutos = 0;
        }
        if (aminutos < 0) {
            contador = 59;
            aminutos = 59;
        }
    }

    if (level_menu == 3) { //Estamos configurando segundos
        asegundos = contador;
        if (asegundos > 59) {
            contador = 0;
            asegundos = 0;
        }
        if (asegundos < 0) {
            contador = 59;
            asegundos = 59;
        }
    }

    lcd.setCursor(4, 1);
    if (ahoras < 10) { //Esto es para poner un 0 delante si las horas son menor de 10
        lcd.print("0");
    }
    lcd.print(ahoras);
    lcd.print(":");

    if (aminutos < 10) { //Esto es para poner un 0 delante si los minutos son menor de 10
        lcd.print("0");
    }
    lcd.print(aminutos);
    lcd.print(":");

    if (asegundos < 10) { //Esto es para poner un 0 delante si los segundos son menor de 10
        lcd.print("0");
    }
    lcd.print(asegundos);
}

void fn_menu(int pos, String menus[], byte sizemenu) {
    lcd.clear();
    linea1 = "";
    linea2 = "";

    if ((pos % 2) == 0) {

        lcd.setCursor(0, 0);
        lcd.write(byte(0));
        linea1 = menus[pos];

        if (pos + 1 != sizemenu) {
            linea2 = menus[pos + 1];
        }

    } else {
        linea1 = menus[pos - 1];
        lcd.setCursor(0, 1);
        lcd.write(byte(0));
        linea2 = menus[pos];
    }

    lcd.setCursor(1, 0);
    lcd.print(linea1);

    lcd.setCursor(1, 1);
    lcd.print(linea2);

}

bool fn_encoder(byte sizemenu) {
    bool retorno = false;

    A_estado_actual = digitalRead(salida_A);

    if (A_estado_actual != A_ultimo_estado) {

        if (digitalRead(salida_B) != A_estado_actual) { //DT != CLK     
            contador++;
            delay(250);
        } else {
            contador--;
            delay(250);
        }

        if (contador <= 0) {
            contador = 0;
        }

        if (contador >= sizemenu - 1) {
            contador = sizemenu - 1;
        }
        retorno = true;
    }
    return retorno;
}

void fn_pausar() {

    lcd.setCursor(0, 0);
    lcd.print(" Tiempo Pausado ");
    btnpress = false;
    delay(250);

    while (digitalRead(swPin) == HIGH);
    if (digitalRead(swPin) == LOW) {
        delay(250); //Evito rebotes del pulsador del Encoder
        lcd.setCursor(0, 0);
        lcd.print("Tiempo Restante");
    }
}

void fn_mostrar_tiempo() {
    lcd.setCursor(8, 0);
    if (ahoras < 10) { //Esto es para poner un 0 delante si las horas son menor de 10
        lcd.print("0");
    }
    lcd.print(ahoras);
    lcd.print(":");

    if (aminutos < 10) { //Esto es para poner un 0 delante si los minutos son menor de 10
        lcd.print("0");
    }
    lcd.print(aminutos);
    lcd.print(":");

    if (asegundos < 10) { //Esto es para poner un 0 delante si los segundos son menor de 10
        lcd.print("0");
    }
    lcd.print(asegundos);
}

void fn_creditos() {

    lcd.setCursor(0, 0);
    lcd.print("Robotica, Impresion, 3D y algo mas...");
    delay(2000);

    for (int positionCounter = 0; positionCounter < 21; positionCounter++) {
        lcd.scrollDisplayLeft(); //Desplaza el texto del LCD hacia la Izquierda
        delay(velocidad_scroll);
    }

    delay(1000);

    lcd.clear(); //Borramos la pantalla
    lcd.setCursor(0, 0); //Ubicamos el cursor en LCD
    lcd.print("Temporizador por"); //Mensaje que se muestra a partir de la posición anterior del cursor
    lcd.setCursor(0, 1); //Volvemos a ubicar el cursor, ahora en la linea de abajo
    lcd.print("Carlos A. Marin "); //Mensaje que se muestra a partir de la posición anterior del cursor
    delay(3000); //tiempo que aparece el mensaje en pantalla
}
