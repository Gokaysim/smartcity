#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "contiki.h"
#include "contiki-net.h"
#include "coap-engine.h"
#include "coap-blocking-api.h"
#include "sys/node-id.h"
#include <string.h>
#include "queueSerializer.h"

/* Log configuration */
#include "log.h"
#include "settings.h"
#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_APP
#define SERVER_EP "coap://[fe80::201:1:1:1]"
#define BASE_SERVER_EP "coap://[fe80::"

char* intToString(int num, char* str, int base) 
{ 
    int i = 0; 
    bool isNegative = false; 
  
    /* Handle 0 explicitely, otherwise empty string is printed for 0 */
    if (num == 0) 
    { 
        str[i++] = '0'; 
        str[i] = '\0'; 
        return str; 
    } 
  
    // In standard itoa(), negative numbers are handled only with  
    // base 10. Otherwise numbers are considered unsigned. 
    if (num < 0 && base == 10) 
    { 
        isNegative = true; 
        num = -num; 
    } 
  
    // Process individual digits 
    while (num != 0) 
    { 
        int rem = num % base; 
        str[i++] = (rem > 9)? (rem-10) + 'a' : rem + '0'; 
        num = num/base; 
    } 
  
    // If number is negative, append '-' 
    if (isNegative) 
        str[i++] = '-'; 
  
    str[i] = '\0'; // Append string terminator 
  
    // // Reverse the string 
    // reverse(str, i); 

    int index = 0;
    int len = strlen(str);

    while ( index < len/2 )
    {
      char temp = str[index];
      str[index] = str[len-1-index];
      str[len-1-index] = temp;

      index = index +1;
    }
  
    return str; 
} 
  
/*
 * Resources to be activated need to be imported through the extern keyword.
 * The build system automatically compiles the resources in the corresponding sub-directory.
 */
extern coap_resource_t
  res_data;
static struct etimer et;
  static char ** output;
static int requestCount =0;
void client_chunk_handler(coap_message_t *response)
{
  requestCount = requestCount-1;
  if(requestCount == 0)
  {
    free(output[0]);
    free(output[1]);
    free(output);
  }    
}

PROCESS(er_example_server, "Node Server");
AUTOSTART_PROCESSES(&er_example_server);


char * GET_SERVER_EP_FOR_NODE(int nodeId){
  char *buf = malloc(sizeof(char)*5);
  intToString(nodeId+200, buf, 10);
  char *val = malloc(sizeof(char) * 50 );
  val[0] = '\0';

  strcat(val,BASE_SERVER_EP);
  strcat(val,buf);
  intToString(nodeId, buf, 10);
  strcat(val,":");
  strcat(val,buf);
  strcat(val,":");
  strcat(val,buf);
  strcat(val,":");
  strcat(val,buf);
  strcat(val,"]");
  free(buf);

  return val;
}

int containsData(char * input)
{
    int length = strlen(input);

    if(input[length-1]!='|')
    {
      return 1;
    }
    if(input[length-2]!='|')
    {
      return 1;
    }
    if(input[length-3]!='|')
    {
      return 1;
    }
    return 0;

}

PROCESS_THREAD(er_example_server, ev, data)
{
  static coap_message_t request[1];
  static coap_endpoint_t server_ep;
  static struct SelfNodeData * self;
  static int dataWaitCount = 0;
  
  PROCESS_BEGIN();
  PROCESS_PAUSE();

  srand(time(NULL));
  self = initSelfNodeData(node_id);

  /*
   * Bind the resources to their Uri-Path.
   * WARNING: Activating twice only means alternate path, not two instances!
   * All static variables are the same for each URI path.
   */
  coap_activate_resource(&res_data, "data/send");

  etimer_set(&et, TOGGLE_INTERVAL * CLOCK_SECOND);
  
  /* Define application-specific events here. */
  int continueLoop = 1;


  while(continueLoop) {
    PROCESS_WAIT_EVENT();

    if(etimer_expired(&et)) {
      etimer_reset(&et);
      int temperatureRandomNumber = rand()%100;
      int availabilityRandomNumber = rand()%100;

      //new request      
      if(temperatureRandomNumber>TEMPERATURE_GENERATE_THRESHOLD)
      {
        float randomTemperature = (float)(rand()%350)/10;
        addTemperature(self,randomTemperature);
        LOG_TEMPARATURE(randomTemperature);
      }
      if(availabilityRandomNumber>AVAILABILITY_GENERATE_THRESHOLD)
      {
        toggleEmpty(self);
        LOG_AVAILABILITY(self->isEmpty);
      }
      
      

      if (dataWaitCount == MAXIMUM_WAIT_TO_SEND || availabilityRandomNumber>AVAILABILITY_GENERATE_THRESHOLD)
      {

          dataWaitCount=0;
          int nodeCount = getNodeCount();
          
          if( nodeCount < MINIMUM_DATA_SEND_COUNT)
          {
              if( nodeCount!=0 || isThereData(self) != 0)
              {              
                  output = getOutputString(self,5,node_id);

                  resetSelf(self);
                  

                  
                  if(containsData(output[0])==1)
                  {
                      requestCount = requestCount+1;
                  }
                  if(containsData(output[1])==1){
                      requestCount = requestCount+1;
                  }

                  // Left
                  if(containsData(output[0])==1)
                  {
                      LOG_SENT(node_id-1,output[0]);
                      char * leftEndPoint = GET_SERVER_EP_FOR_NODE(node_id-1);
                      coap_init_message(request, COAP_TYPE_CON, COAP_POST, 0);
                      coap_set_header_uri_path(request,"/data/send"); 

                      coap_endpoint_parse(leftEndPoint, strlen(leftEndPoint), &server_ep);      
                      coap_set_payload(request, (uint8_t *)output[0], strlen(output[0]));                 
                      COAP_BLOCKING_REQUEST(&server_ep, request, client_chunk_handler);
                  }

                  // //Right        
                  if(containsData(output[1])==1){

                    LOG_SENT(node_id+1,output[1]);
                    char * rightEndPoint = GET_SERVER_EP_FOR_NODE(node_id+1);
                    coap_init_message(request, COAP_TYPE_CON, COAP_POST, 0);
                    coap_set_header_uri_path(request,"/data/send"); 
                    coap_endpoint_parse(rightEndPoint, strlen(rightEndPoint), &server_ep);                        

                    coap_set_payload(request, (uint8_t *)output[1], strlen(output[1]));
                    COAP_BLOCKING_REQUEST(&server_ep, request, client_chunk_handler);
                  }
                  
              }              
          }
      }
      else{
        dataWaitCount = dataWaitCount+1;
      }
    }

  }

  PROCESS_END();
}
