#include "../include/StompProtocol.h"
//#include "../include/event.h"
#include <map>
#include <iterator>
#include <iostream>
#include <fstream>

//class event;



// send keyboard

// receive from server

// wait for receipts


// header cpp of string and do it something we can read from

StompProtocol::StompProtocol(std::map<std::string, std::string> &topicToId1, ConnectionHandler &con, std::map<std::string, std::map<std::string, GameEvents>>& userToGames, std::string use) : topicToId(topicToId1), 
conHandler(con), userToGames(userToGames), idCounter(0), receiptCounter(0), receiptDone(""), user(use), gameGroup(""), exit(false), join(false){} 
void StompProtocol :: processKeyBoard(std::string msg){
    int typeIndex = msg.find(' ');
    std::string command = msg.substr(0, typeIndex);
    std::string body = msg.substr(typeIndex+1, msg.length() - typeIndex);

    if(!command.empty() && !body.empty()){

        if(command.compare("login") == 0){

            processLogin(body);
        }

        else if(command.compare("join") == 0){

            processJoin(body);
        }

        else if(command.compare("exit") == 0){
            
            processExit(body);
        }

        else if(command.compare("report") == 0){
            
            processReport(body);
        }

        else if(command.compare("logout") == 0){
            
            processLogout(body);
        }

        else if(command.compare("summary") == 0){
            
            processSummery(body);
        }

        else{ //ERROR

        }

    }

    else{ //ERROR

    }

}

bool StompProtocol :: processSocket(std::string msg){

    int typeIndex = msg.find('\n');
    std::string command = msg.substr(0, typeIndex);
    std::string body = msg.substr(typeIndex+1, msg.length() - typeIndex);

    if(!command.empty() && !body.empty()){

        if(command.compare("CONNECTED") == 0){
            processConnected(body);
        }

        else if(command.compare("MESSAGE") == 0){
            processMessage(body);
        }

        else if(command.compare("RECEIPT") == 0){
            
            bool ans = processRecipt(body);
            return ans;
        }

        else if(command.compare("ERROR") == 0){
            
            bool ans = processError(body);
            return ans;
        }

        else{  // DOESNT SUPOSSED TO HAPPEN YOU PROMISSED LEGAL INPUT

        }

    }
    else{ // DOESNT SUPOSSED TO HAPPEN YOU PROMISSED LEGAL INPUT

    }
    return false;

}

void StompProtocol :: processLogin(std::string msg){

    int in = msg.find(' ');
    std::string host = msg.substr(0, in);

    msg = msg.substr(in + 1, msg.length() - in); // changed

    in = msg.find(' ');
    std::string login = msg.substr(0, in);
    msg = msg.substr(in + 1, msg.length() - in);

    in = msg.find(' ');
    std::string pass = msg.substr(0, in);

    if(pass.empty() || login.empty() || host.empty()){ // DOESNT SUPOSSED TO HAPPEN YOU PROMISSED LEGAL INPUT

    }
    else{

        std::string frame = "CONNECT\naccept-version:1.2\nhost:stomp.cs.bgu.ac.il\nlogin:"+login+"\npasscode:"+pass+"\n\0";
        user = login;
        conHandler.sendFrameAscii(frame, '\0');

    }


}

void StompProtocol :: processJoin(std::string msg){

    if(msg.empty()){ // ERROR SHOULDNT SUPPOSE TO HAPPEN!

    }
    else{

        std::string id = std::to_string(idCounter);
        topicToId[msg] = id;
        idCounter++;
        std::string rec = std::to_string(receiptCounter);
        receiptCounter++;

        std::string frame = "SUBSCRIBE\ndestination:" +msg+ "\nid:"+id+ "\nreceipt:"+rec+"\n\0"; // NOT DONE

        join = true;
        gameGroup = msg;

        conHandler.sendFrameAscii(frame, '\0');

    }

}

void StompProtocol :: processExit(std::string msg){

    if(msg.empty()){ // ERROR

    }
    else{

        std::string id = topicToId[msg];
        std::string rec = std::to_string(receiptCounter);
        receiptCounter++;

        std::string framee = "UNSUBSCRIBE\nid:"+id+ "\nreceipt:"+rec+"\n\0";

        exit = true;
        gameGroup = msg;

        conHandler.sendFrameAscii(framee, '\0');

    }

}

void StompProtocol :: processReport(std::string msg){

    names_and_events nne = parseEventsFile("data/"+msg); 
    eventsVector.push_back(nne.events);
    
    for(Event e: nne.events){

        std::string sendMessage = "SEND\ndestination:" + e.get_team_a_name() + "_" + e.get_team_b_name() + "\n\nuser: "+user+
        "\nteam a: " + e.get_team_a_name() + "\nteam b: " + e.get_team_b_name() + "\nevent name: " + e.get_name() + "\ntime: " + 
        std::to_string(e.get_time()) + "\ngeneral game updates:\n";

        
        
        std::map<std::string, std::string>::iterator itr1 = e.get_game_updates().begin();

        while(itr1 != e.get_game_updates().end()){

            std::string key = itr1->first;
            std::string value = itr1->second;

            sendMessage = sendMessage + key + ": " + value + "\n";

            itr1++;
        }
        sendMessage += "team a updates:\n";

        std::map<std::string, std::string>::iterator itr2 = e.get_team_a_updates().begin();

        while(itr2 != e.get_team_a_updates().end()){

            std::string key = itr2->first;
            std::string value = itr2->second;

            sendMessage = sendMessage + key + ": " + value + "\n";

            itr2++;
        }
        sendMessage += "team b updates:\n";

        std::map<std::string, std::string>::iterator itr3 = e.get_team_b_updates().begin();

        while(itr3 != e.get_team_b_updates().end()){

            std::string key = itr3->first;
            std::string value = itr3->second;

            sendMessage = sendMessage + key + ": " + value + "\n";

            itr3++;
        }

        sendMessage += "description:\n" + e.get_discription() + "\n\0"; // NOT DONE

        conHandler.sendFrameAscii(sendMessage, '\0');
        
        
    }

    


}

void StompProtocol ::processConnected(std::string msg){

    std::cout << "Login successful" << std::endl;

}

void StompProtocol :: processMessage(std::string msg){

    std::string content = msg.substr(msg.find("destination:")+12, msg.length() - msg.find("destination:")-12);

    std::string topic = content.substr(0, content.find('\n')); // getting the topic

    content = content.substr(content.find('\n')+1, content.length() - content.find('\n'));

    std::string temp = content.substr(content.find("user: ")+5, content.length() - content.find("user: ")-5); // getting the user
    std::string user = temp.substr(1, temp.find('\n')-1); // CHANGED THE -1

    if(userToGames.count(user) == 0){ 

        std::map<std::string, GameEvents> userMap;
        userToGames[user] = userMap;
    }
    if(userToGames[user].count(topic) == 0){

        std::string nameA = topic.substr(0, topic.find('_'));
        std::string nameB = topic.substr(topic.find('_')+1, topic.length() - topic.find('_'));
        userToGames[user].emplace(topic, GameEvents(topic, nameA, nameB)); 

    }
    if( userToGames.count(user) != 0 && userToGames[user].count(topic) != 0){
        userToGames.at(user).at(topic).add(content);
    }
    
}

bool StompProtocol :: processRecipt(std::string msg){

    std::string rec = msg.substr(msg.find("receipt-id:") + 11, msg.length() - msg.find("receipt-id:") - 11);
    rec = rec.substr(0, rec.find('\n')); // getting only the rec number

    std::cout<<"got a receipt"<<std::endl;

    if(rec.compare(receiptDone) == 0){
        return true;
    }
    else if(exit){

        std::cout<<"exited channel "+gameGroup<<std::endl;
        exit = false;
        return false;
    }
    else if(join){

        std::cout<<"joined channel "+gameGroup<<std::endl;
        join = false;
        return false;
    }
    else return false;

}

bool StompProtocol :: processError(std::string msg){

    std::cout << msg << std::endl;
    return true;
}

void StompProtocol :: processSummery(std::string msg){

    std::string userName;
    std::string gameName;
    std::string file;

    gameName = msg.substr(0, msg.find(' '));
    msg = msg.substr(msg.find(' ')+1, msg.length() - msg.find(' ') - 1);

    userName = msg.substr(0, msg.find(' '));
    msg = msg.substr(msg.find(' ')+1, msg.length() - msg.find(' ') - 1);

    file = "data/" + msg; 

    GameEvents game = userToGames[userName][gameName];

    std::string sum = game.summary();
    
    std::ofstream myFile;
    myFile.open(file, std::fstream::app);
    myFile << sum; // adding the data to file
    myFile.close();

    


}

void StompProtocol :: processLogout(std::string msg){


    std::string rec = std::to_string(receiptCounter);
    receiptDone = std::to_string(receiptCounter);
    receiptCounter++;



    std::string frame = "DISCONNECT\nreceipt:"+rec+"\n\n"; // NOT DONE
    topicToId.clear();

    // setDisconnectFlag(true);

    conHandler.sendFrameAscii(frame, '\0');

}