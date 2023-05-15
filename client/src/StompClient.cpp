#include <stdlib.h>
// #include "../include/ConnectionHandler.h"
#include "../include/StompProtocol.h"
#include "../include/KeyBoardThread.h"
#include <map>
#include <thread>
#include <mutex>
#include <iostream>


int main() {
    std::string host;

    bool error = false;

    while(!error){ //so more than one client can connect

        bool legal = false;
        short port;

        std::string userName;
        std::string passcode;

        while(!legal){

            const short bufsize = 1024;
            char buf[bufsize];
            std::cin.getline(buf, bufsize); // reads from the keyboard
	        std::string line(buf);

            if(line.substr(0, 5).compare("login") == 0){

                line = line.substr(6, line.length() - 6); // cutting the "login "

                host = line.substr(0, line.find(' ')); // host = "host:port"
                std::string tempPort;
                tempPort = host.substr(host.find(':') + 1, host.length() - host.find(':') - 1);
                host = host.substr(0, host.find(':'));
                std::istringstream strm(tempPort); strm >> port; // from internet

                line = line.substr(line.find(' ') + 1, line.length() - line.find(' ') - 1); // cutting the "host:port "

                userName = line.substr(0, line.find(' '));

                line = line.substr(line.find(' ') + 1, line.length() - line.find(' ') - 1); // cutting the "userName "

                passcode = line;

                legal = true;

            }

        }
        
	    ConnectionHandler connectionHandler(host, port);
        if (!connectionHandler.connect()) {
            std::cerr << "Cannot connect to " << host << ":" << port << std::endl;
            return 1;
        }

        std::map<std::string, std::string> topicToId1;

        std::map<std::string, std::map<std::string, GameEvents>> userToGames;

        StompProtocol stompProt(topicToId1, connectionHandler, userToGames, userName); // creating the protocol

        // login func

        std::string frame = "CONNECT\naccept-version:1.2\nhost:stomp.cs.bgu.ac.il\nlogin:"+userName+"\npasscode:"+passcode+"\n\0";
        connectionHandler.sendFrameAscii(frame, '\0');

        // login func


        KeyBoardThread KeyTask(stompProt); // creating the thread MAYBE NEED REFRENCE
        std::thread keyThread(&KeyBoardThread::run, &KeyTask);
        bool diconnect = false; 

        while(!diconnect){

            std::string answer;
            if(!connectionHandler.getFrameAscii(answer, '\0')){

                KeyTask.stopp();
                diconnect = true;
            }
            else{

                diconnect = stompProt.processSocket(answer);
                if(diconnect){
                    KeyTask.stopp();
                }
                else KeyTask.waiting = false;
            }
        }

        keyThread.join();
    }
	return 0;
}
