#include "nodeData.h"
#include <stdio.h>
#include <stdlib.h>
#include "log.h"



struct NodeData * addNode(struct NodeData * head, unsigned nodeId, int nodeDiff, int isEmpty, float lastTemperature,float avgTemperature ){
    if(head == NULL){
        struct NodeData * val = (struct NodeData *) malloc(sizeof(struct NodeData));
        val->nodeId = nodeId;
        val->nodeDiff = nodeDiff;
        val->isEmpty = isEmpty;
        val->lastTemperature = lastTemperature;
        val->avgTemperature = avgTemperature;
        val->next = NULL;
        val->previous = NULL;

        return val;
    }
    
    struct NodeData * iterator = head;

    while(1){
        if(iterator->nodeId == nodeId)
        {
            iterator->nodeId = nodeId;
            iterator->nodeDiff = nodeDiff;
            if(isEmpty != NullEmpty)
            {             
                iterator->isEmpty = isEmpty;   
            }
            if(lastTemperature != NullTemperature)
            {
                iterator->lastTemperature = lastTemperature;
            }
            if(avgTemperature != NullTemperature)
            {            
                iterator->avgTemperature = avgTemperature;
            }

            return head;
        }
        else if(iterator->next == NULL)
        {            
            
            struct NodeData * temp = (struct NodeData *) malloc(sizeof(struct NodeData));
            temp->nodeId = nodeId;
            temp->nodeDiff = nodeDiff;
            temp->next = NULL;
            // if(isEmpty != NullEmpty)
            // {             
                temp->isEmpty = isEmpty;   
            // }
            // if(lastTemperature != NullTemperature)
            // {
                temp->lastTemperature = lastTemperature;
            // }
            // if(avgTemperature != NullTemperature)
            // {            
                temp->avgTemperature = avgTemperature;
            // }
        
            iterator->next = temp;    
            iterator->next->previous = iterator;

            return head;
        }
        iterator = iterator->next;   
    }
}


int isThereData(struct SelfNodeData * self)
{
    if( self->emptyChanged == 0 && self->temperatureChanged == 0)
    {
        return 0;
    }
    return 1;
}

struct NodeData* seperateNElementsFromLinkedList(struct NodeData* head,int count){
    struct NodeData* newHead = head;

    int index = 0;
    for(index= 0;index<count;index++){
        if(newHead == NULL){
            return newHead;
        }
        newHead = newHead->next;
    }

    newHead->previous->next = NULL;
    newHead->previous = NULL;
    
    return newHead;
}


void printLinkedList(struct NodeData* head){
    LOG("\n\nLinked List\n\n");
    struct NodeData* iterator = head;
    while(iterator !=NULL){
        LOG("Node Id %d \t Node Diff %d \t Is empty %d \t Last temperature %f \t Avg temperature %f \n",iterator->nodeId,iterator->nodeDiff,
        iterator->isEmpty,iterator->lastTemperature,iterator->avgTemperature);
        iterator = iterator->next;
    }
}


struct SelfNodeData * initSelfNodeData(int nodeId)
{
    struct SelfNodeData * self = (struct SelfNodeData *)malloc(sizeof(struct SelfNodeData));
    self->nodeId = nodeId;
    self->isEmpty = 0;
    self->temperature = -275;
    self->totalTemperature = 0;
    self->totalTemperatureCount = 0;
    self->emptyChanged = 0;
    self->temperatureChanged =0;

    return self;
}
void toggleEmpty(struct SelfNodeData * self)
{
    self->isEmpty = !self->isEmpty;
    self->emptyChanged = 1;
}
void addTemperature(struct SelfNodeData * self,float temperature)
{
    self->temperatureChanged = 1;
    self->temperature = temperature;
    self->totalTemperature = self->totalTemperature + temperature;
    self->totalTemperatureCount = self->totalTemperatureCount + 1;
}
void resetSelf(struct SelfNodeData * self)
{
    self->emptyChanged = 0;
    self->temperatureChanged = 0;

    self->totalTemperatureCount = 0;
    self->totalTemperature = 0;
}

int nodeCount(struct NodeData* head){
    int count = 0;

    struct NodeData* iterator = head;

    while(iterator!= NULL){
        count = count+1;
        iterator = iterator->next;
    }

    return count;
}
