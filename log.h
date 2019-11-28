#include <stdlib.h>
#define LOG(...) printf(__VA_ARGS__)
#define LOG_SENT(sentTo,sentData) printf("LOG SENT %d %s \n",sentTo,sentData)
#define LOG_RECEIVED(sentData) printf("LOG RECEIVED %s \n",sentData)
#define LOG_TEMPARATURE(temprature) printf("LOG TEMPARATURE %3.1f \n",temprature)
#define LOG_AVAILABILITY(availability) printf("LOG AVAILABILITY %d\n",availability)


