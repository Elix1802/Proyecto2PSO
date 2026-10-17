#ifndef NODEPAGE_H
#define NODEPAGE_H

struct NodePage {
    int id;
    int parentId;
    int loaded; //1 if process is in RAM otherwise 0
    int L_Address;
    int M_Address;
    int D_Address; //-1 cuando no este en memoria virtual
    int loaded_T; //El tiempo añadido
    int Mark; //Second chance : 1 fue usado 0 no usado | Predicts: 0 fui cargado por new 1 fui cargado por use
    struct NodePage * nextPtr;
    struct NodePage * pastPtr;
    int freq;
};

typedef struct NodePage NodePage;

NodePage * createNodePage(int pageId, int parentId, int loaded, int L_Addr, int M_Addr, int D_addr, int loaded_T, NodePage * nextPtr, NodePage * pastPtr);

void destroyNodePage(NodePage * self);

//get
NodePage * getNextPtr(NodePage * self);
NodePage * getPastPtr(NodePage * self);
void bringFromDisk(NodePage * self);


//Puts
void putNextPtr(NodePage * self, NodePage * nextPtr);
void putPastPtr(NodePage * self, NodePage * pastPtr);
void putOnVirtualMemory(NodePage * self, int D_Addr);
void addOneMoreFreq(NodePage * self);

//print
void printNodePage(NodePage * self);
#endif





