/*
* Envío de datos de T/H a OpenHab
* por: Hugo Escalpelo
* Fecha: 3 de junio de 2022
*
* Estre programa envio a los valores de temperatura y humedad del sensor DHT11
* por MQTT para que sean recuperados por OpenHab. Posteriormente se realizara
* una automatización simple con OpenHab
*
* Componente PinESP32CAM Estados lógicos
* ledStatus------GPIO 33---------On=>BAJO, Off=>ALTO
* ledFlash-------GPIO 4----------Encendido=>ALTO, Apagado=>BAJO
*
* DHT11 ESP32CAM
* VCC------------Vcc/5V
* TIERRA------------TIERRA
* S--------------14
*
 */

// Bibliotecas
# include  < WiFi.h >   // Biblioteca para el control de WiFi
# include  < PubSubClient.h >  // Biblioteca para conexión MQTT
# include  " DHT.h "  // Biblioteca del DHT11

// consta
# define  DHTPIN  14  // Pin el que voy a conectar el sensor DHT11
# define  DHTTYPE DHT11 // Variable para seleccion de modelo

// Datos de WiFi
const  char * ssid = " INFINITUMB09C_2.4 " ;  // Aquí debes poner el nombre de tu red
const  char * contraseña = "mz7srSbma6 " ;  // Aquí debes poner la contraseña de tu red

// Datos del bróker MQTT
const  char * mqtt_server = " 192.168.1.93 " ; // Si estas en una red local, coloca la IP indicada, en caso contrario, coloca la IP publica
servidor de dirección IP ( 192 , 168 , 1 , 93 );

// Objetos
WiFiCliente espCliente; // Este objeto maneja los datos de conexion WiFi
Cliente PubSubClient (espClient); // Este objeto maneja los datos de conexion al broker
DHT dht (DHTPIN, DHTTYPE); // Objeto para manejo del DHT11

// Variables
int flashLedPin = 4 ;  // Para indicar el estado de conexion
int estadoLedPin = 33 ; // Para ser controlado por MQTT
mucho tiempoAhora, tiempoÚltimo; // Variables de control de tiempo no bloqueante
datos int = 0 ; // contador
espera int = 5000 ;  // Indica la espera cada 5 segundos para envio de mensajes MQTT

// Inicializacion del programa
 configuración vacía () {
  // Iniciar comunicacion serial
  De serie. comenzar ( 115200 );
  pinMode (flashLedPin, SALIDA);
  pinMode (estadoLedPin, SALIDA);
  escritura digital ( flashLedPin , BAJO);
  digitalWrite (estadoLedPin, ALTO);

  De serie. imprimirln ();
  De serie. imprimirln ();
  De serie. print ( " Conectar a " );
  De serie. imprimir ( ssid );
 
  Wifi. comenzar (ssid, contraseña); // Esta es la función que realiza la conexión a WiFi
 
  while (WiFi. status () != WL_CONNECTED) { // Este bucle espera a que se realice la conexión
    digitalWrite (estadoLedPin, ALTO);
    retraso ( 500 ); // dado que es de suma importancia esperar a la conexion, debe usar espera bloqueante
    escritura digital ( estadoLedPin , BAJO);
    De serie. imprimir ( " . " );  // Indicador de progreso
    retraso ( 5 );
  }
  
  // Cuando se haya logrado la conexión, el programa avanzará, por lo tanto, puede informarse lo siguiente
  De serie. imprimirln ();
  De serie. println ( " WiFi conectado " );
  De serie. println ( " Dirección IP: " );
  De serie. println (WiFi. localIP ());

  // Si se logro la conexión, encienda el led
  si (WiFi. estado () > 0 ){
  escritura digital ( estadoLedPin , BAJO);
  }
  
  retraso ( 1000 ); // Esta espera es solo una formalidad antes de iniciar la comunicación con el corredor

  // Conexión con el broker MQTT
  cliente. setServer (servidor, 1883 ); // Conectarse a la IP del broker en el puerto indicado
  cliente. setCallback (devolución de llamada); // Activar función de CallBack, permite recibir mensajes MQTT y ejecutar funciones a partir de ellos
  retraso ( 1500 );  // Esta espera es preventiva, espera a la conexion para no perder informacion

  // Inicio del objeto que maneja el sensor
  dht. comenzar ();
  
  tiempoÚltimo = milisegundos ( ); // Inicia el control de tiempo
} // configuración final del vacío ()

// Cuerpo del programa, bucle principal
 bucle vacío () {
  // Verificar siempre que haya conexion al broker
  if (!cliente. conectado ()) {
    reconectar ();  // En caso de que no haya conexión, ejecute la función de reconexión, definida después del void setup ()
  } // fin del if (!cliente.conectado())
  cliente. bucle (); // Esta función es muy importante, ejecuta de manera no bloqueante las funciones necesarias para la comunicación con el broker
  
  tiempoAhora = milisegundos ( ); // Control de tiempo para esperas no bloqueantes
  if (timeNow - timeLast > wait ) { // Manda un mensaje por MQTT cada cinco segundos
    tiempoÚltimo = tiempoAhora; // Actualización de seguimiento de tiempo

    // Obtener las lecturas de temperatura y humedad del sensor
    flotante h = dht. leerHumedad ();
    flotante t = dht. leerTemperatura ();

    // Comprobar que la conexion con el sensor sea exitosa
    si ( isnan (h) || isnan (t)) {
      De serie. println ( F ( " ¡ Error al leer del sensor DHT! " ));
      volver ;
    }
    
    char cadena de datos[ 8 ]; // Define un arreglo de caracteres para enviarlos por MQTT, especifica la longitud del mensaje en 8 caracteres
    dtostrf (t, 1 , 2 , cadena de datos);  // Esta es una función nativa de leguaje AVR que convierte un arreglo de caracteres en una variable String
    De serie. imprimir ( " Temperatura: " ); // Se imprime en monitor solo para poder visualizar que el evento sucede
    De serie. println (cadena de datos);
    cliente. publicar ( " codigoIoT/G6/temp " , dataString); // Esta es la función que envía los datos por MQTT, especifica el tema y el valor

    retraso ( 100 );
    dtostrf (h, 1 , 2 , cadena de datos);
    De serie. print ( " Humedad: " ); // Se imprime en monitor solo para poder visualizar que el evento sucede
    De serie. println (cadena de datos);
    cliente. publicar ( " codigoIoT/G6/hum " , dataString);
  } // fin del if (timeNow - timeLast > esperar)
} // Fin del bucle vacío ()

// Funciones de usuario

// Esta función permite tomar acciones en caso de que se reciba un mensaje correspondiente a un tema al cual se hará una suscripción
 devolución de llamada vacía ( char * tema, byte * mensaje, longitud int sin firmar ) { 

  // Indicar por serial que llego un mensaje
  De serie. print ( " Llego un mensaje en el tema: " );
  De serie. imprimir (tema);

  // Concatenar los mensajes recibidos para conformarlos como una varialbe String
  Cadena mensajeTemp; // Se declara la variable en la cual se generará el mensaje completo  
  for ( int i = 0 ; i < length; i++) {   // Se imprime y concatena el mensaje
    De serie. imprimir (( char )mensaje[i]);
    mensajeTemp += ( char )mensaje[i];
  }

  // Se comprueba que el mensaje se haya concatenado correctamente
  De serie. imprimirln ();
  De serie. print ( " Mensaje concatenado en una sola variable: " );
  De serie. println (mensajeTemp);

  // En esta parte puedes agregar las funciones que requieres para actuar segun lo necesitas al recibir un mensaje MQTT

  // Ejemplo, en caso de recibir el mensaje true - false, se cambiará el estado del led soldado en la placa.
  // El ESP323CAM está suscrito al tema esp/output
  if ( String (topic) == " codigoIoT/G6/led " ) {   // En caso de recibir un mensaje en el tema esp32/output
    si (mensajeTemp == " ON" ){
      De serie. println ( " Led encendido " );
      escritura digital ( flashLedPin , ALTO);
    } // fin del if (String(tema) == "esp32/salida")
    más  si (messageTemp == " OFF " ){
      De serie. println ( " Led apagado " );
      escritura digital ( flashLedPin , BAJO);
    } // fin del else if(messageTemp == "false")
  } // fin del if (String(tema) == "esp32/salida")
} // devolución de llamada fin del void

// Funcion para reconectarse
 reconexión vacía () {
  // Bucle hasta lograr la conexion
  while (!cliente. conectado ()) { // Pregunta si hay conexión
    De serie. print ( " Tratando de contactarse... " );
    // Intentar reconectar
    if (client. connect ( " ESP32CAMClient " )) { // Pregunta por el resultado del intento de conexión
      De serie. println ( " Conectado " );
      cliente. suscribirse ( " codigoIoT/G6/led " ); // Esta función realiza la suscripción al tema
    } // fin del if (cliente.conectar("ESP32CAMClient"))
    else {   // en caso de que la conexión no se logre
      De serie. print ( " Conexion fallida, Error rc= " );
      De serie. imprimir (cliente. estado ()); // Muestra el codigo de error
      De serie. println ( " Volviendo a intentar en 5 segundos " );
      // Espera de 5 segundos bloqueante
      retraso ( 5000 );
      De serie. println (cliente. conectado ()); // Muestra el estado de conexion
    } // fin del otro
  } // fin del bucle while (!cliente.conectado())
} // Fin de void reconectar (
