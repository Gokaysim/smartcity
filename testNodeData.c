#include <stdio.h> 
#include "queueSerializer.h"
#include "log.h"

int main() 
{
    struct SelfNodeData * d = initSelfNodeData(10);

    // toggleEmpty(d);
    toggleEmpty(d);
    addTemperature(d,21.2);
    // addTemperature(d,26.2);

    char ** fas = getOutputString(d,5,1);
    LOG("%s\n",fas[0]);
    LOG("%s\n",fas[1]);

    if(strcmp(fas[0],"||")== 0){
        printf("asdsad\n");
    }

    // processInputString(fas[0]);
    // processInputString(fas[1]);

    // processInputString(fas[0]);
    // printf("********\n");
    processInputString(fas[1],1);


    char ** fas2 = getOutputString(d,5,1);
    LOG("s %s\n",fas2[0]);
    LOG("s %s\n",fas2[1]);
    LOG("Success!!! \n"); 
    
    return 0; 
} 
