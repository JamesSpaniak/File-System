#ifndef _LDisk_HPP_
#define _LDisk_HPP_

#include <iostream>
#include "DNode.hpp"

class LDisk {
    public:
        LDisk(int blocks, int blockSize);
        int insert(int size, LFile* fhead);
        void remove(int blockAddress, LFile* fhead);
        void merge();
        void print(bool debug);
        int totalBlocks;
        ~LDisk();
    private:
        DNode *head;
        int blockSize;
        void split(DNode *node, int toSplit, int toStatus);
        void printNode(DNode *node);
};


#endif
