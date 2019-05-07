#ifndef _FNode_HPP_
#define _FNode_HPP_
#include <cstdio>

struct FNode {
    int blockAddress;
    int capacity;
    FNode* next;
    FNode* prev;  

    FNode() {
        blockAddress = -1;
        next = NULL;
        prev = NULL;
    }

    FNode(int m, int capacity_, FNode* p, FNode* n) {
        blockAddress = m;
        capacity = capacity_;
        next = n;
        prev = p;
    }
};


#endif
