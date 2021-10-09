#include "contiki.h"
#include "net/routing/routing.h" //se va a hacer uso de RPL
#include "net/netstack.h" 
#include "net/ipv6/simple-udp.h" 

#include "sys/log.h" 
#define LOG_MODULE "Sensor Node App" 
#define LOG_LEVEL LOG_LEVEL_INFO 

#define SEND_INTERVAL (10 * CLOCK_SECOND) //definir cada cuanto se va a enviar info

static struct simple_udp_connection connection //variable a través de la cual se va a definir la conexión UDP y por donde se van a enviar/recibir datos. El nombre es connection

PROCESS(sensor_node, "Sensor Node"); //creación de proceso, el nombre es lo que va entre comillas
AUTOSTART_PROCESSES(&sensor_node);

//ya que este nodo sensor no va a recibir datos, no se va a implementar ninguna función de callback
//registrar udp para enviar información a través de ese protocolo de transporte

PROCESS_THREAD(sensor_node, ev, data) 
{
    static struct etimer periodic_timer; //declaración de temporizador
    static unsigned counter; //implementación de counter (cada paquete que se envíe va a representar un counter+=1)
    static char buffer [32]: //declaración de buffer de 32 bits
    uip_ipaddr_t server_address; //guardar dirección del nodo sinc o servidor
    
    PROCESS_BEGIN(); 

    simple_udp_register(&connection, CLIENT_PORT, NULL, SERVER_PORT, NULL) //registro de udp en la que se establece la conexión, el puerto del server, se indica que no habrá un callback para server mediante la asignación del valor NULL y se indica que va a mandar a llamar la función cada que llegue un paquete
    etimer_set(&periodic_timer, SEND_INTERVAL); //inicializar timer y asignar periodo de tiempo, cada 10s reinicia

    while (1) {
        PROCESS_WATI_EVENT_UNTIL(etimer_expired(&periodic_timer));

        uint8_t is_node_reachable = NETSTACK_ROUTING.is_node_reachable();//se va a fijar si el nodo ya pertenece al nodo raíz
        uint8_t got_server_address = NETSTACK_ROUTING.get_root_ipaddr(&server_address)://va a indicar si ya se pudo obtener la IP del nodo raíz y la va a guardar en la variable server addres
        if (is_node_reachable && got_server_address){
            LOG_INFO("Sending request %u to ", counter) //enviar info al nodo # 
            LOG_INFO_6ADDR(&server_address); //depuración para saber IP del servidor
            LOG_INFO_("\n");

            snprintf(buffer, sizeof(buffer), "Packet No. %d", counter ++); //dar formato a datos a enviar. Guardar en buffer, indicar el tamaño del mismo y el mensaje
            simple_upd_sendto(&connection, buffer, strlen(buffer), &server_address)//enviar info a través de variable connection
            //se asegura que el nodo ya sea parte del DODAG o grafo aciclico dirigido
        } else {
            LOG_INFO_("Root not reachable yet.\n");
        }

        etimer_reset(&periodic_timer);
    }
    PROCESS_END();
}