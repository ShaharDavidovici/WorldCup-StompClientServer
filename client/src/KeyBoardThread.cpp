#include "../include/KeyBoardThread.h"


    KeyBoardThread::KeyBoardThread(StompProtocol& pr):waiting(true), stop(false), prot(pr){}

    void KeyBoardThread::stopp(){

        stop = true;
    }

    void KeyBoardThread::run(){
        while(!stop){ 

            if(!waiting ){ 
                
                const short bufsize = 1024;
                char buf[bufsize];
                std::cin.getline(buf, bufsize); // reads from the keyboard
		        std::string line(buf); 

                int typeIndex = line.find(' ');
                std::string command = line.substr(0, typeIndex);

                    prot.processKeyBoard(line);

                    if(line.substr(0, line.find(' ')).compare("summary") != 0){
                        waiting = true;
                    }
                        
            }
                
        }

    }