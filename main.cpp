#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "Tree.hpp"

/*
    Author(s): James Spaniak
*/

void replace(std::string& str, const std::string& from, const std::string& to);

void useStatement() {
    std::cout<<"./driver -f input_file -d input_dir -s disk_size -b block_size" << std::endl;
}

int main(int argc, char* argv[]) {
    std::string fileList = "";
    std::string dirList = "";
    int diskSize = -1;
    int blockSize = -1;
    char flag;
    int bytes;

    while((flag = getopt(argc, argv, "f:d:s:b:")) != -1)
    {
        switch(flag)
        {
            case 'f':
                fileList = optarg;
                break;
            case 'd':
                dirList = optarg;
                break;
            case 's':
                diskSize = atoi(optarg);
                break;
            case 'b':
                blockSize = atoi(optarg);
                break;
            case '?':
                useStatement();
                exit(1);
            default:
                useStatement();
                exit(1);
        }
    }

    //Error handling
    if(diskSize < 0 || blockSize < 0 || fileList.length() < 1 || dirList.length() < 1)
    {
        useStatement();
        exit(1);
    }

    //reading streams
    std::ifstream fileStream(fileList);
    std::ifstream directoryStream(dirList);
    std::string line;
    size_t index=0;

    if (!directoryStream){
        std::cerr<< "Cannot open file " <<std::endl;
        exit(1);
    }

    int numBlocks= diskSize/blockSize;
    
    LDisk disk_ll = LDisk(numBlocks, blockSize);
    Tree directory= Tree(blockSize, numBlocks, &disk_ll);

    //Read initial directories from input file
    while(std::getline(directoryStream, line)){
        //each directory becomes a node on the tree
        replace(line, "//", "/");
        directory.addNode(-1, line);
    }
    directoryStream.close();

    if (!fileStream){
        std::cerr<< "Cannot open file" <<std::endl;
    }

    //Set up initial files from input file
    while (std::getline(fileStream,line)){
        std::string name;
        int size=0;

        std::string buffer;
        std::stringstream stream(line);
        int ctr=0;

        while (stream >> buffer){
            if (ctr==6){
                size= atoi(buffer.c_str());
            }
            if (ctr == 10){
                name= buffer;
            }
            ctr++;
        }
        
        replace(name, "//", "/");
        directory.addNode(size, name);
    }
    fileStream.close();
    directory.setHandleAsRoot();

    //User input
    while(1) {
        std::cout << "FileSystem/Shell: " << directory.getCurrentWorkingDirectory() << ": ";
        std::string command;
        std::string arg1;
        std::string arg2;
        getline(std::cin,command);

        std::stringstream stream(command);

        stream >> command;
        stream >> arg1;
        stream >> arg2;

        if (!command.compare("cd")) {
            std::string search = "./";
            if(!strncmp(arg1.c_str(), search.c_str(), search.size())) {
                arg1 = arg1.substr(2);
            }
            directory.cd(arg1);
        }

        else if(!command.compare("cd..")) {
            directory.cdOut();
        }

        else if(!command.compare("ls")) {
            directory.ls();
        }

        else if(!command.compare("mkdir")) {
            if (arg1.compare("")){
                directory.mkdir(arg1);
            }
            else{
                std::cout << "Need more arguments" <<std::endl;
            }   
        }

        else if(!command.compare("create")) {
            if (arg1.compare("")){
                directory.create(arg1);
            }
            else{
                std::cout << "Need more arguments" <<std::endl;
            }   
        }

        else if(!command.compare("append")) {
            try {
                bytes = stoi(arg2);
            } catch(const std::exception& e) {
                std::cout << "Enter an integer for number of bytes to be appended" << std::endl;
            } 
            if (arg1.compare("") && arg2.compare("")) {
                directory.append(arg1, bytes);
            }
            else{
                std::cout << "Need more arguments" <<std::endl;
            }   
        }

        else if(!command.compare("remove")) {
            try {
                bytes = stoi(arg2);
            } catch(const std::exception& e) {
                std::cout << "Enter an integer for number of bytes to be removed" << std::endl;
            } 
            if (arg1.compare("") && arg2.compare("")) {
                directory.remove(arg1, bytes);
            }
            else{
                std::cout << "Need more arguments" <<std::endl;
            }   
        }

        else if(!command.compare("delete")) {
            directory.deleteNode(arg1); 
        }

        else if(!command.compare("exit")) {
            std::cout << "Bye" << std::endl;
            break;
        }

        else if(!command.compare("dir")) {
            directory.dir();
        }

        else if(!command.compare("prfiles")) {
            directory.printFiles();
        }

        else if(!command.compare("prdisk")) {
            directory.printDisk();
        }

        else {
            std::cout << "Command does not exist" <<std::endl;
        }
    }
}


void replace(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = str.find(from);
    if(start_pos != std::string::npos)
        str.replace(start_pos, from.length(), to);
}