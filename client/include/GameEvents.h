#pragma once
#include <string>
#include <map>
#include <vector>


class  GameEvents
{
private:

const std::string name;

const std::string bName;

std::map<std::string, std::string> aStats;
std::map<std::string, std::string> bStats;
std::map<std::string, std::string> generalStats;

std::vector<std::string> gameReportim;

public:

    const std::string aName; // return to private!!

    GameEvents();
    GameEvents(const std::string &name, const std::string &aName , const std::string &bName);
    void add(std::string report);
    std::string summary();
    
};