#ifndef _LFILE_HPP_
#define _LFILE_HPP_

#include "FNode.hpp"


class LFile {
    public:
        LFile(int ts, int bs);
        void appendNodeWithSize(int numBytes, int startingAddress);
        void fillLastNode(int numBytes);
        int getLastNodeSpace();
        void remove(int numBytes);
        void print(std::string indent);
        bool isAddressPresent(int startingAddress);
        int getSize();
        FNode* getHead();
        ~LFile();
    private:
        FNode* start;
        int totalSize;
        int blockSize;
        int capacity;
};

#endif
