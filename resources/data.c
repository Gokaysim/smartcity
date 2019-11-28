#include <stdio.h>
#include <string.h>
#include "coap-engine.h"
#include "coap.h"
#include "sys/node-id.h"
#include "log.h"

/* Log configuration */
#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_APP


static void res_any_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);

/* This resource mirrors the incoming request. It shows how to access the options and how to set them for the response. */
RESOURCE(res_data,
         "title=\"Returns your decoded message\";rt=\"Debug\"",
         res_any_handler,
         res_any_handler,
         res_any_handler,
         res_any_handler);

static void
res_any_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  /* The ETag and Token is copied to the header. */
  // uint8_t opaque[] = { 0x0A, 0xBC, 0xDE };
  const uint8_t *chunk;
  const char *str = NULL;
  int lenss = coap_get_payload(request, &chunk);
  
  LOG_RECEIVED((char *)chunk);
  processInputString((char *)chunk,node_id);

  coap_set_payload(response, chunk, lenss);
}