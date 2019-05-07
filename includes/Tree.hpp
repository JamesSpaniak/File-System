#ifndef _Tree_HPP_
#define _Tree_HPP_

#include "GNode.hpp"
#include "LDisk.hpp"

class Tree {
    public:
        Tree(int bs,int b);
        Tree(int bs, int b, LDisk *d);
        //FOR YOUR DIRECTORIES AND FILES
        void addNode(int size,std::string name);
        //FOR YOUR DEBUGGING
        void prePrint(bool isDFS);
        // METHODS FOR THE SHELL
        void cd(std::string name);
        void cdOut();
        void ls();
        void mkdir(std::string name);
        void create(std::string name);
        void append(std::string name, int bytes);
        void remove(std::string name,int bytes);
        void deleteNode(std::string name);
        void dir();
        void printDisk();
        void printFiles();
        void setHandleAsRoot();
        std::string getCurrentWorkingDirectory();
        // CLEANUP
        ~Tree();
    private:
        GNode * root;
        LDisk * disk;
        // USE currentDir as a handle
        GNode * currentDir;
        int numBlocks;
        int blockSize;
        void pFile(GNode * dir);
        GNode* lookUp(std::string name);
        void printStage(GNode* curr);
        void printDFS(GNode* curr, int level);
        int cdHelper(std::string name);
        int cdFromRoot(std::string name);
        int time;
};

#endif
