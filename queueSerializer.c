#include "queueSerializer.h"
#include <stdio.h> 
#include <stdlib.h>
#include "log.h"
#include <string.h>
#include "settings.h"

static struct NodeData* head;
int countChars( char* s, char c );
int * parseStrToNodes(char * str,int * count);
struct TemperatureHash * createTemperatureHash2(float temperature,int* nodeIds,int count);

void FreeTemperatureHash(struct TemperatureHash * f){
    struct TemperatureHash *s, *tmp;
    HASH_ITER(hh, f, s, tmp) {
        free(s->nodeIds);
        free(s);
    }
}

void processInputString(char * input,int nodeId){   
    int nodeDiff =1; 
    char *nodeIdStr=NULL,*availabilityStr=NULL,*temperatureStr=NULL,*avgTemperatureStr=NULL;    

    if(input!= NULL){
        nodeIdStr = strtok(input,"|");
        int requestNodeId = atoi(nodeIdStr);
        nodeDiff = nodeId - requestNodeId;

        availabilityStr = strtok(NULL,"|");
        temperatureStr = strtok(NULL,"|");
        avgTemperatureStr = strtok(NULL,"|");
    } 
    // availability
    if(availabilityStr!=NULL)
    {

        int availabilityCount = countChars(availabilityStr,';');
        char ** temp = malloc(sizeof(char * )*availabilityCount);
        if(availabilityCount==1)
        {
            temp[0]= strtok(availabilityStr,";");
        }
        if(availabilityCount==2)
        {
            temp[0]= strtok(availabilityStr,";");
            temp[1]= strtok(NULL,";");
        }
        int i = 0;

        for(;i<availabilityCount;i++){
            char * stateVal = strtok(temp[i],":");
            char * nodeIdsStr =strtok(NULL,":");

            int nodeCount = 0;
            unsigned int * nodeIds = parseStrToNodes(nodeIdsStr,&nodeCount);
             
            int j = 0;              
            for(j;j<nodeCount;j++){
                unsigned int nodeId = nodeIds[j];
                int state = atoi(stateVal);
                if(state==1){
                    head = addNode(head,nodeId,nodeDiff,1,NullTemperature,NullTemperature);                    
                }
                else if(state == 0){
                    head = addNode(head,nodeId,nodeDiff,0,NullTemperature,NullTemperature);
                }
            }
            
        }

        free(temp);
    }
    // temperature
    if(temperatureStr!=NULL)
    {
        int temperatureCount = countChars(temperatureStr,';'); 
        char ** temp = malloc(sizeof(char * )*temperatureCount);
        if(temperatureCount>0)
        {
            int index = 1;
            temp[0]= strtok(temperatureStr,";");
            for(;index<temperatureCount;index++)
            {
                temp[index]= strtok(NULL,";");
            }
            index =0;
            for(;index<temperatureCount;index++)
            {
                float temperature = atof(strtok(temp[index],":"));
                int count=0;
                int * nodeIds = parseStrToNodes(strtok(NULL,":"),&count);


                int innerIndex = 0;
                for(;innerIndex<count;innerIndex++)
                {
                    head = addNode(head,nodeIds[innerIndex],nodeDiff,NullEmpty,temperature,NullTemperature);                      
                }    
            }
        }
    
        free(temp);   
    }
    // avg temperature
    if(avgTemperatureStr!=NULL)
    {
        int temperatureCount = countChars(avgTemperatureStr,';');
        
        char ** temp = malloc(sizeof(char * )*temperatureCount);
       
        if(temperatureCount>0)
        {
            int index = 1;
            
            temp[0]= strtok(avgTemperatureStr,";");
            
            for(;index<temperatureCount;index++)
            {
                temp[index]= strtok(NULL,";");
            }
            index =0;
            
            for(;index<temperatureCount;index++)
            {
                float temperature = atof(strtok(temp[index],":"));
                int count=0;
                int * nodeIds = parseStrToNodes(strtok(NULL,":"),&count);

                int innerIndex = 0;
                for(;innerIndex<count;innerIndex++)
                {                                        
                    head = addNode(head,nodeIds[innerIndex],nodeDiff,NullTemperature,NullTemperature,temperature);
                }
                free(nodeIds);                                             
            }
        }
        free(temp);
    }    
}

void generateOutputString(int nodeId,char * str,
    int* emptyNodeList,int emptyNodeListCount,
    int * notEmptyNodeList,int notEmptyNodeListCount,
    struct TemperatureHash* lastTempList,
    struct TemperatureHash* avgTempList);


struct TemperatureHash * createTemperatureHash(float temperature,int nodeId,int maxNodeIdCount){
    struct TemperatureHash * hash =(struct TemperatureHash *) malloc(sizeof(struct TemperatureHash));
    hash->count = 1;
    hash->id = temperature;
    hash->nodeIds = (int *)malloc(sizeof(int)*maxNodeIdCount);
    hash->nodeIds[0] = nodeId;
    return hash;
}

struct TemperatureHash * createTemperatureHash2(float temperature,int* nodeIds,int count){
    struct TemperatureHash * hash =(struct TemperatureHash *) malloc(sizeof(struct TemperatureHash));
    hash->count = count;
    hash->id = temperature;
    hash->nodeIds = nodeIds;
    return hash;
}

void addTemperatureToHash(struct TemperatureHash * hash,int nodeId){
    hash->nodeIds[hash->count]=nodeId;
    hash->count = hash->count +1;
}


char** getOutputString(struct SelfNodeData * self,int userMaxCount,int nodeId){

    char ** returnString = malloc(sizeof(char*)*2);
    int maxCount = userMaxCount +1;
    
    returnString[0]=(char *)malloc(sizeof(char *) * MAX_STRING_LENGTH);
    returnString[1]=(char *)malloc(sizeof(char *) * MAX_STRING_LENGTH);
    returnString[0][0]='\0';
    returnString[1][0]='\0';

    
    char * leftOutput = returnString[0];
    char * rightOutput = returnString[1];
    struct NodeData* iterator = head;

    // isEmpty Storeage
    int ** isEmptyStorageLeft = (int **) malloc(sizeof(int * )*2);
    // indicates empty
    isEmptyStorageLeft[0] = (int *) malloc(sizeof(int)*maxCount);
    int emptyStorageLeftCount = 0;
    // indicates not empty
    isEmptyStorageLeft[1] = (int *) malloc(sizeof(int)*maxCount);
    int notEmptyStorageLeftCount = 0;

    int ** isEmptyStorageRight = (int **) malloc(sizeof(int * )*2);
    // indicates empty
    isEmptyStorageRight[0] = (int *) malloc(sizeof(int)*maxCount);
    int emptyStorageRightCount = 0;
    // indicates not empty
    isEmptyStorageRight[1] = (int *) malloc(sizeof(int)*maxCount);
    int notEmptyStorageRightCount = 0;

    // last temperature storeage
    struct TemperatureHash* leftLastTemperatureKeeper= NULL;
    struct TemperatureHash* rightLastTemperatureKeeper= NULL;

    // avg temperature storeage
    struct TemperatureHash* leftAvgTemperatureKeeper= NULL;
    struct TemperatureHash* rightAvgTemperatureKeeper= NULL;

    while(iterator!=NULL){
        // data goes to both direction
        if(iterator->nodeDiff == 0)
        {
            if(iterator->isEmpty==0)
            {
                isEmptyStorageLeft[0][emptyStorageLeftCount] = iterator->nodeId;
                emptyStorageLeftCount = emptyStorageLeftCount+1;

                isEmptyStorageRight[0][emptyStorageRightCount] = iterator->nodeId;
                emptyStorageRightCount = emptyStorageRightCount+1;
            }
            else if(iterator->isEmpty==1){
                isEmptyStorageLeft[1][notEmptyStorageLeftCount] = iterator->nodeId;
                notEmptyStorageLeftCount = notEmptyStorageLeftCount+1;

                isEmptyStorageRight[1][notEmptyStorageRightCount] = iterator->nodeId;
                notEmptyStorageRightCount = notEmptyStorageRightCount+1;
            }
            
            // last temperature
            {
                struct TemperatureHash *leftLastTemp;
                HASH_FIND_INT(leftLastTemperatureKeeper, &iterator->lastTemperature,leftLastTemp );
                if (leftLastTemp==NULL) {
                    leftLastTemp = createTemperatureHash(iterator->lastTemperature,iterator->nodeId,maxCount);                    
                    HASH_ADD_INT(leftLastTemperatureKeeper,id,leftLastTemp);
                }
                else{
                    addTemperatureToHash(leftLastTemp,iterator->nodeId);                    
                }

                struct TemperatureHash *rightLastTemp;
                HASH_FIND_INT(rightLastTemperatureKeeper, &iterator->lastTemperature,rightLastTemp );
                if (rightLastTemp==NULL) {
                    rightLastTemp = createTemperatureHash(iterator->lastTemperature,iterator->nodeId,maxCount);
                    HASH_ADD_INT(rightLastTemperatureKeeper,id,rightLastTemp);
                }
                else{
                    addTemperatureToHash(rightLastTemp,iterator->nodeId);
                }
            }

            // avg temperature
            {
                struct TemperatureHash *leftAvgTemp;
                HASH_FIND_INT(leftAvgTemperatureKeeper, &iterator->avgTemperature,leftAvgTemp );
                if (leftAvgTemp==NULL) {
                    leftAvgTemp = createTemperatureHash(iterator->avgTemperature,iterator->nodeId,maxCount);
                    HASH_ADD_INT(leftAvgTemperatureKeeper,id,leftAvgTemp);
                }
                else{
                    addTemperatureToHash(leftAvgTemp,iterator->nodeId);
                }

                struct TemperatureHash *rightAvgTemp;
                HASH_FIND_INT(rightAvgTemperatureKeeper, &iterator->avgTemperature,rightAvgTemp );
                if (rightAvgTemp==NULL) {
                    rightAvgTemp = createTemperatureHash(iterator->avgTemperature,iterator->nodeId,maxCount);
                    HASH_ADD_INT(rightAvgTemperatureKeeper,id,rightAvgTemp);
                }
                else{
                    addTemperatureToHash(rightAvgTemp,iterator->nodeId);
                }
            }
            


        }
        // data goes to left
        else if(iterator->nodeDiff == -1)
        {
            if(iterator->isEmpty==0)
            {
                isEmptyStorageLeft[0][emptyStorageLeftCount] = iterator->nodeId;
                emptyStorageLeftCount = emptyStorageLeftCount+1;
            }
            else if(iterator->isEmpty==1){
                isEmptyStorageLeft[1][notEmptyStorageLeftCount] = iterator->nodeId;
                notEmptyStorageLeftCount = notEmptyStorageLeftCount+1;
            }

            // last temperature
            {
                struct TemperatureHash *leftLastTemp;
                HASH_FIND_INT(leftLastTemperatureKeeper, &iterator->lastTemperature,leftLastTemp );
                if (leftLastTemp==NULL) {
                    leftLastTemp = createTemperatureHash(iterator->lastTemperature,iterator->nodeId,maxCount);                    
                    HASH_ADD_INT(leftLastTemperatureKeeper,id,leftLastTemp);
                }
                else{
                    addTemperatureToHash(leftLastTemp,iterator->nodeId);                    
                }
            }

            // avg temperature
            {
                struct TemperatureHash *leftAvgTemp;
                HASH_FIND_INT(leftAvgTemperatureKeeper, &iterator->avgTemperature,leftAvgTemp );
                if (leftAvgTemp==NULL) {
                    leftAvgTemp = createTemperatureHash(iterator->avgTemperature,iterator->nodeId,maxCount);
                    HASH_ADD_INT(leftAvgTemperatureKeeper,id,leftAvgTemp);
                }
                else{
                    addTemperatureToHash(leftAvgTemp,iterator->nodeId);
                }
            }
        }
        // data goes to right
        else{
            if(iterator->isEmpty==0)
            {
                isEmptyStorageRight[0][emptyStorageRightCount] = iterator->nodeId;
                emptyStorageRightCount = emptyStorageRightCount+1;
            }
            else if(iterator->isEmpty==1){
                isEmptyStorageRight[1][notEmptyStorageRightCount] = iterator->nodeId;
                notEmptyStorageRightCount = notEmptyStorageRightCount+1;
            }

            // last temperature
            {

                struct TemperatureHash *rightLastTemp;
                HASH_FIND_INT(rightLastTemperatureKeeper, &iterator->lastTemperature,rightLastTemp );

                if (rightLastTemp==NULL) {
                    rightLastTemp = createTemperatureHash(iterator->lastTemperature,iterator->nodeId,maxCount);
                    HASH_ADD_INT(rightLastTemperatureKeeper,id,rightLastTemp);

                }
                else{
                    addTemperatureToHash(rightLastTemp,iterator->nodeId);
                }
            }

            // avg temperature
            {

                struct TemperatureHash *rightAvgTemp;
                HASH_FIND_INT(rightAvgTemperatureKeeper, &iterator->avgTemperature,rightAvgTemp );
                if (rightAvgTemp==NULL) {
                    rightAvgTemp = createTemperatureHash(iterator->avgTemperature,iterator->nodeId,maxCount);
                    HASH_ADD_INT(rightAvgTemperatureKeeper,id,rightAvgTemp);
                }
                else{
                    addTemperatureToHash(rightAvgTemp,iterator->nodeId);
                }
            }
        }
        iterator = iterator->next;        
    }

    if(self->emptyChanged)
    {
        if(self->isEmpty==0)
        {
            isEmptyStorageLeft[0][emptyStorageLeftCount] = self->nodeId;
            emptyStorageLeftCount = emptyStorageLeftCount+1;
        }
        else{
            isEmptyStorageLeft[1][notEmptyStorageLeftCount] = self->nodeId;
            notEmptyStorageLeftCount = notEmptyStorageLeftCount+1;
        }
        if(self->isEmpty==0)
        {
            isEmptyStorageRight[0][emptyStorageRightCount] = self->nodeId;
            emptyStorageRightCount = emptyStorageRightCount+1;
        }
        else{
            isEmptyStorageRight[1][notEmptyStorageRightCount] = self->nodeId;
            notEmptyStorageRightCount = notEmptyStorageRightCount+1;
        }
    }
    if(self->temperatureChanged)
    {
        float avgTemperature = self->totalTemperature/self->totalTemperatureCount;
        
        // last temperature
        {
            struct TemperatureHash *leftLastTemp;
            HASH_FIND_INT(leftLastTemperatureKeeper, &self->temperature,leftLastTemp );
            if (leftLastTemp==NULL) {
                leftLastTemp = createTemperatureHash(self->temperature,self->nodeId,maxCount);                    
                HASH_ADD_INT(leftLastTemperatureKeeper,id,leftLastTemp);
            }
            else{
                addTemperatureToHash(leftLastTemp,self->nodeId);                    
            }
            struct TemperatureHash *rightLastTemp;
            HASH_FIND_INT(rightLastTemperatureKeeper, &self->temperature,rightLastTemp );
            if (rightLastTemp==NULL) {
                rightLastTemp = createTemperatureHash(self->temperature,self->nodeId,maxCount);                    
                HASH_ADD_INT(rightLastTemperatureKeeper,id,rightLastTemp);
            }
            else{
                addTemperatureToHash(rightLastTemp,self->nodeId);                    
            }
        }

        // avg temperature
        {
            struct TemperatureHash *leftAvgTemp;
            HASH_FIND_INT(leftAvgTemperatureKeeper, &avgTemperature,leftAvgTemp );
            if (leftAvgTemp==NULL) {
                leftAvgTemp = createTemperatureHash(avgTemperature,self->nodeId,maxCount);
                HASH_ADD_INT(leftAvgTemperatureKeeper,id,leftAvgTemp);
            }
            else{
                addTemperatureToHash(leftAvgTemp,self->nodeId);
            }

            struct TemperatureHash *rightAvgTemp;
            HASH_FIND_INT(rightAvgTemperatureKeeper, &avgTemperature,rightAvgTemp );
            if (rightAvgTemp==NULL) {
                rightAvgTemp = createTemperatureHash(avgTemperature,self->nodeId,maxCount);
                HASH_ADD_INT(rightAvgTemperatureKeeper,id,rightAvgTemp);
            }
            else{
                addTemperatureToHash(rightAvgTemp,self->nodeId);
            }
        }
    }

    generateOutputString(nodeId,leftOutput,isEmptyStorageLeft[0],emptyStorageLeftCount,isEmptyStorageLeft[1],notEmptyStorageLeftCount,leftLastTemperatureKeeper,leftAvgTemperatureKeeper);
    generateOutputString(nodeId,rightOutput,isEmptyStorageRight[0],emptyStorageRightCount,isEmptyStorageRight[1],notEmptyStorageRightCount,rightLastTemperatureKeeper,rightAvgTemperatureKeeper);
    
    free(isEmptyStorageLeft[0]);
    free(isEmptyStorageLeft[1]);
    free(isEmptyStorageLeft);


    free(isEmptyStorageRight[0]);
    free(isEmptyStorageRight[1]);
    free(isEmptyStorageRight);
    
    
    FreeTemperatureHash(leftLastTemperatureKeeper);
    FreeTemperatureHash(leftAvgTemperatureKeeper);
    FreeTemperatureHash(rightLastTemperatureKeeper);
    FreeTemperatureHash(rightAvgTemperatureKeeper);

    resetSelf(self);

    return returnString;
}

void generateOutputString(int nodeId,char * str,
    int* emptyNodeList,int emptyNodeListCount,
    int * notEmptyNodeList,int notEmptyNodeListCount,
    struct TemperatureHash* lastTempList,
    struct TemperatureHash* avgTempList)
{
    int strIndex = 0;

    strIndex = strIndex + sprintf(str+strIndex,"%d|",nodeId);

    int loopIndex=0;    
    if(emptyNodeListCount>0)
    {
        str[strIndex]='0';
        strIndex++;
        str[strIndex]=':';
        strIndex++;

        for(loopIndex=0;loopIndex<emptyNodeListCount;loopIndex++)
        {
            if(emptyNodeListCount-1 == loopIndex)
            {
                strIndex = strIndex + sprintf(str+strIndex,"%d;",emptyNodeList[loopIndex]);
            }
            else{
                strIndex = strIndex + sprintf(str+strIndex,"%d,",emptyNodeList[loopIndex]);
            }                      
        }
    }

    if(notEmptyNodeListCount>0)
    {
        str[strIndex]='1';
        strIndex++;
        str[strIndex]=':';
        strIndex++;

        for(loopIndex=0;loopIndex<notEmptyNodeListCount;loopIndex++)
        {
            if(notEmptyNodeListCount-1 == loopIndex)
            {
                strIndex = strIndex + sprintf(str+strIndex,"%d;",notEmptyNodeList[loopIndex]);
            }
            else{
                strIndex = strIndex + sprintf(str+strIndex,"%d,",notEmptyNodeList[loopIndex]);
            }                      
        }
    }

    str[strIndex]='|';
    strIndex++;
    str[strIndex]='\0';

    struct TemperatureHash *s;

    for(s=lastTempList; s != NULL; s=s->hh.next) {
        if(s->id == NullTemperature)
        {
            continue;
        }

        strIndex = strIndex + sprintf(str+strIndex,"%3.1f",s->id);
        str[strIndex]=':';
        strIndex++;

        int innerLoopIndex=0;
        for(;innerLoopIndex<s->count;innerLoopIndex++){
            if(s->count-1 == innerLoopIndex)
            {
                strIndex = strIndex + sprintf(str+strIndex,"%d;",s->nodeIds[innerLoopIndex]);
            }
            else{
                strIndex = strIndex + sprintf(str+strIndex,"%d,",s->nodeIds[innerLoopIndex]);
            }
        }     
    }

    str[strIndex]='|';
    strIndex++;
    str[strIndex]='\0';

    s=NULL;
    for(s=avgTempList; s != NULL; s=s->hh.next) {

        if(s->id == NullTemperature)
        {
            continue;
        }
        strIndex = strIndex + sprintf(str+strIndex,"%3.1f",s->id);
        str[strIndex]=':';
        strIndex++;

        int innerLoopIndex=0;
        for(;innerLoopIndex<s->count;innerLoopIndex++){
            if(s->count-1 == innerLoopIndex)
            {
                strIndex = strIndex + sprintf(str+strIndex,"%d;",s->nodeIds[innerLoopIndex]);
            }
            else{
                strIndex = strIndex + sprintf(str+strIndex,"%d,",s->nodeIds[innerLoopIndex]);
            }
        }        
    }
}

int getNodeCount(){
    return nodeCount(head);
}

int * parseStrToNodes(char * str,int * count)
{
    if(str == NULL)
    {
        *count = 0;
        return NULL;
    }
    *count = countChars(str,',')+1;

    int * nodeIds = malloc(sizeof(int)*(*count));
    int index = 0;
    char * nodeIdStr= strtok(str,",");
    nodeIds[index] = atoi(nodeIdStr);
    index = index +1;
    while(1)
    {
        nodeIdStr = strtok(NULL,",");
        if(nodeIdStr == NULL)
        {
            break;
        }
        nodeIds[index] = atoi(nodeIdStr);
        index = index +1;
    }
    return nodeIds;
}

int countChars( char* s, char c)
{
    return *s == '\0'
              ? 0
              : countChars( s + 1, c ) + (*s == c);
}


void printList(){
    printLinkedList(head);
}