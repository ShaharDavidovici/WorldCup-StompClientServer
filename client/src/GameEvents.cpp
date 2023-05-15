#include "../include/GameEvents.h"
#include <iterator>
#include <iostream>

GameEvents::GameEvents(){}

GameEvents::GameEvents(const std::string &namee, const std::string &nameA, const std::string &nameB) : name(namee), aName(nameA), bName(nameB){}

void GameEvents::add(std::string report){

    std::string temp;

    std::string gameEventReport;

    int reportLastIndex = report.length() - 1;
    temp = report.substr(report.find("event name: ")+12, reportLastIndex - report.find("event name: ")-12);
    std::string eventName = temp.substr(0, temp.find('\n'));

    temp = report.substr(report.find("time: ")+6, reportLastIndex - report.find("time: ")-6);
    std::string eventTime = temp.substr(0, temp.find('\n'));

    temp = report.substr(report.find("description:\n")+13, reportLastIndex - report.find("description:\n")-13);
    std::string eventDescription = temp; // maybe until the ^@

    gameEventReport = eventTime + " - " + eventName + ";\n\n" + eventDescription + "\n\n";

    gameReportim.push_back(gameEventReport); // ALL THE DOWN STUFF


    std::string statName;
    std::string statVal;


    temp = report.substr(report.find("general game updates:\n")+22, report.find("team a updates:") - report.find("general game updates:\n")-22);

    temp = temp + "\n\0";

    while ((!temp.empty() || temp.length() > 1) && temp != "\0" && temp != "\n")
    {
        statName = temp.substr(0, temp.find(':'));
        statVal = temp.substr(temp.find(':')+2, temp.find('\n')-temp.find(':')-2);

        generalStats[statName] = statVal;

        temp = temp.substr(temp.find('\n')+1, temp.length()-temp.find('\n')-1);

    }

    temp = report.substr(report.find("team a updates:\n")+16, report.find("team b updates:") - report.find("team a updates:\n")-16);
    temp = temp + "\n\0";


    while (!temp.empty() && temp != "\0" && temp != "\n")
    {
        statName = temp.substr(0, temp.find(':'));
        statVal = temp.substr(temp.find(':')+2, temp.find('\n')-temp.find(':')-2);

        //CHECK IN THE HASH MAP
        aStats[statName] = statVal;

        temp = temp.substr(temp.find('\n')+1, temp.length()-temp.find('\n'));
    }

    temp = report.substr(report.find("team b updates:\n")+16, report.find("description:") - report.find("team b updates:\n")-16);
    temp = temp + "\n\0";

    while (!temp.empty() && temp != "\0" && temp != "\n")
    {
        statName = temp.substr(0, temp.find(':'));
        statVal = temp.substr(temp.find(':')+2, temp.find('\n')-temp.find(':')-2);

        //CHECK IN THE HASH MAP
        bStats[statName] = statVal;

        temp = temp.substr(temp.find('\n')+1, temp.length() - temp.find('\n') - 1);
    }

}

std::string GameEvents::summary(){
    
    std::string sum;


    sum = aName + " VS " + bName +"\nGame stats:\nGeneral stats:\n";

    std::map<std::string, std::string>::iterator itr1 = generalStats.begin();

    while(itr1 != generalStats.end()){

        std::string key = itr1->first;
        std::string value = itr1->second;

        sum += key + ": " + value + "\n";

        itr1++;
    }

    std::map<std::string, std::string>::iterator itr2 = aStats.begin();

    sum += aName + " stats:\n";


    while(itr2 != aStats.end()){

        std::string key = itr2->first;
        std::string value = itr2->second;

        sum += key + ": " + value + "\n";

        itr2++;
    }

    std::map<std::string, std::string>::iterator itr3 = bStats.begin();

    sum += bName + " stats:\n";

    while(itr3 != bStats.end()){

        std::string key = itr3->first;
        std::string value = itr3->second;

        sum += key + ": " + value + "\n";

        itr3++;
    }

    sum += "Game event reports:\n";

    for(std::string rep: gameReportim){
        sum += rep;
    }

    return sum;


}
