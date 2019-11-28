#include "nodeData.h"
#include "uthash/uthash.h"

struct TemperatureHash{
    float id;
    int * nodeIds;
    int count;
    UT_hash_handle hh;
};

struct NodeIdTemperature{
    int nodeId;
    float temperature;
    UT_hash_handle hh;
};
struct TemperatureHash * createTemperatureHash(float temperature,int nodeId,int maxNodeIdCount);
void addTemperatureToHash(struct TemperatureHash * hash,int nodeId);

char ** getOutputString(struct SelfNodeData * self,int maxCount,int nodeId);
void processInputString(char * input,int nodeId);

void printList();

int getNodeCount();

char * FloatToTwoByte(float value);
float TwoByteToFloat(char * val);
int TwoByteToInt(char * val);
char * IntToTwoByte(int value);

void FreeTemperatureHash(struct TemperatureHash * f);
