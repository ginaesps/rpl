#include "contiki.h"
#include "net/routing/routing.h" //protocolos de enrutamiento
#include "net/netstack.h" //stack de red
#include "net/ipv6/simple-udp.h" //librería para crear paquetes udp de manera simple

#include "sys/log.h" //bitacora
#define LOG_MODULE "Sink Node App" //módulo de bitácora
#define LOG_LEVEL LOG_LEVEL_INFO //tipo de mensajes que se van a recibir

static struct simple_udp_connection connection //variable a través de la cual se va a definir la conexión UDP y por donde se van a enviar/recibir datos. El nombre es connection

PROCESS(sink_node, "Sink Node"); //creación de proceso, el nombre es lo que va entre comillas
AUTOSTART_PROCESSES(&sink_node);

static void
rx_callback(struct simple_upd_connection *client_connection, //indicar función de callback que se va a mandar a llamar cada que llegue un mensaje de los nodos sensores para recibirlos y mostrarlos
 const uip_ipaddr_t *sender_address, //client connection es un puntero por lo que trae el puntero. uip se refiere a una micro dirección IP que es una estructura diseñada para contener distintos tipos de variables
 uint16_t sender_port, //entero sin signo de 16 bits por donde se va a indicar el puerto por el que se envia la info
 const uip_ipaddr_t *receiver_address, //micro ip en la cual se indica la dirección de receptor 
 uint16_t receiver_port, //indicación del puerto del receptor
 const uint8_t *data, //buffer para guardar datos. Todos estos son bytes. Un arreglo puede trabajar como un puntero sin longitud definida, por lo que se puede considerar que se está declarando un buffer infinito dentro de los datods
 uint16_t data_length)  //para evitar el desborde del buffer por parte de los datos, se va a indicar una longitud máxima y esta variable va a servir para poder recorrer el buffer
{
    LOG_INFO("Received packet: '%.*s' from node", data_length, (char *) data);//cada que se mande a llamar la función, se va a mostrar en consola el paquete que se recibió evaluando la posición y contenido
    LOG_INFO_6ADDR(sender_address); //se va a indicar cual es el nodo que está enviando esta info con 6addr para que pueda formatear una ipv6
    LOG_INFO_("\n");
} 

PROCESS_THREAD(sink_node, ev, data) //nodo raiz, eventos y datos de entrada 
{
    PROCESS_BEGIN(); //inicialización

    NETSTACK_ROUTING.root_start(); //asignación del nodo raíz para la red, los demás se deben unir a el
    //el root se va a estar ejecutando de manera indefinida ya que no hay algo que lo impida

    simple_udp_register(&connection, SERVER_PORT, NULL, CLIENT_PORT, rx_callback) //registro de udp en la que se establece la conexión, el puerto del server, se indica que no habrá un callback para server mediante la asignación del valor NULL y se indica que va a mandar a llamar la función cada que llegue un paquete

    PROCESS_END();
}