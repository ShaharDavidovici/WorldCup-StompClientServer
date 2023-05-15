#include <stdlib.h>
#include "../include/ConnectionHandler.h"

/**
* This code assumes that the server replies the exact text the client sent it (as opposed to the practical session example)
*/
int main (int argc, char *argv[]) { //1-ip,2-port
    std::cout << "starting main acoooo" << std::endl;
    if (argc < 3) {
        std::cerr << "Usage : " << argv[0] << " host port" << std::endl << std::endl;
        return -1;
    }
    std::string host = argv[1];
    short port = atoi(argv[2]);
    
    ConnectionHandler connectionHandler(host, port); // take it to keyboard
    if (!connectionHandler.connect()) {
        std::cerr << "Cannot connect to " << host << ":" << port << std::endl;
        return 1;
    }
	
    // LOGIN IN HERE

	//From here we will see the rest of the ehco client implementation:
    while (1) {  //threadim in here
        const short bufsize = 1024;
        char buf[bufsize];
        std::cin.getline(buf, bufsize); // reads from the keyboard
		std::string line(buf); // this too to keyboard

        //threadim

		int len=line.length();
        if (!connectionHandler.sendLine(line)) { // sends the line
            std::cout << "1Disconnected. Exiting...\n" << std::endl;
            break;
        } // HERE

        // keyboard calls the protocol

		// connectionHandler.sendLine(line) appends '\n' to the message. Therefor we send len+1 bytes.
        std::cout << "Sent " << len+1 << " bytes to server" << std::endl;

 
        // We can use one of three options to read data from the server:
        // 1. Read a fixed number of characters
        // 2. Read a line (up to the newline character using the getline() buffered reader
        // 3. Read up to the null character
        std::string answer; // from the server
        // Get back an answer: by using the expected number of bytes (len bytes + newline delimiter)
        // We could also use: connectionHandler.getline(answer) and then get the answer without the newline char at the end
        if (!connectionHandler.getLine(answer)) {
            std::cout << "2Disconnected. Exiting...\n" << std::endl;
            break;
        }
        
		len=answer.length();
		// A C string must end with a 0 char delimiter.  When we filled the answer buffer from the socket
		// we filled up to the \n char - we must make sure now that a 0 char is also present. So we truncate last character.
        answer.resize(len-1);
        std::cout << "Reply: " << answer << " " << len << " bytes " << std::endl << std::endl;
        if (answer == "bye") {
            std::cout << "Exiting...\n" << std::endl;
            break;
        }
    }
    return 0;
}
