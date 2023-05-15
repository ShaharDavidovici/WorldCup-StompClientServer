#pragma once
//#include <unordered_map>
#include <string>
#include "../include/ConnectionHandler.h"
#include "../include/GameEvents.h"
#include "../include/event.h"
#include <map>
#include <vector>


// TODO: implement the STOMP protocol
class StompProtocol
{
private:

//std::unordered_map<std::string, std::string> StompProtocol::msgTranslater(std::string msg);

void processLogin(std::string msg);

void processJoin(std::string msg);

void processExit(std::string msg);

void processReport(std::string msg);

void processConnected(std::string msg);

void processMessage(std::string msg);

bool processRecipt(std::string msg);

bool processError(std::string msg);

void processSummery(std::string msg);

void processLogout(std::string msg);

int idCounter;
int receiptCounter;
std::map<std::string, std::string>& topicToId;
std::vector<std::vector<Event>> eventsVector;
std::string user;
ConnectionHandler &conHandler;//&

// std::string outRec;
// bool waitingRecDisconnect;

std::map<std::string, std::map<std::string, GameEvents>>& userToGames; // user -> games -> gameEvents

std::string receiptDone; // the rec of the disconnect



std::string gameGroup;
bool join = false;
bool exit = false;


public:

StompProtocol(std::map<std::string, std::string> &topicToId, ConnectionHandler &con, std::map<std::string, std::map<std::string, GameEvents>> &userToGames, std::string use);

//void StompProtocol::process(std::string msg);

//bool disonnect;

void processKeyBoard(std::string msg);

bool processSocket(std::string msg);

//bool isWaitingForDisconnectReceipt();
//bool setDisconnectFlag(bool val);

};
