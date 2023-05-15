#pragma once

#include <iostream>
#include <thread>
#include "../include/StompProtocol.h"

class KeyBoardThread{

private:

    
    bool stop;
    StompProtocol &prot;

public:

    bool waiting;
    KeyBoardThread(StompProtocol &pr);

    void stopp();
    void run();


};

