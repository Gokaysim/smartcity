#ifndef NODE_DATA_H
#define NODE_DATA_H


#define NullEmpty -1
#define NullTemperature -275 

struct NodeData{
    struct NodeData * next;
    struct NodeData * previous;

    // it is a boolean type
    int isEmpty;

    float lastTemperature;
    float avgTemperature;

    int nodeDiff;
    unsigned nodeId;
};
struct SelfNodeData{

    // it is a boolean type
    int isEmpty;
    int emptyChanged;
    float temperature;
    float totalTemperature;
    int temperatureChanged;
    int totalTemperatureCount;
    int nodeId;
};

struct NodeData * addNode(struct NodeData * head,
    unsigned nodeId, int nodeDiff, int isEmpty, float lastTemperature,float avgTemperature );


struct NodeData* seperateNElementsFromLinkedList(struct NodeData* head,int count);

void printLinkedList(struct NodeData* head);

struct SelfNodeData * initSelfNodeData(int nodeId);
void toggleEmpty(struct SelfNodeData * self);
void addTemperature(struct SelfNodeData * self,float temperature);
void resetSelf(struct SelfNodeData * self);
int nodeCount(struct NodeData* head);

int isThereData(struct SelfNodeData * self);
#endif
