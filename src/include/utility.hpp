#ifndef UTILITY_H
#define UTILITY_H

#include <string>
#include <iostream>
#include <iostream>
#include <algorithm>
#include <vector>
#include <map>
#include <cmath>
#include <string>
using namespace std;

namespace cadmium::iot
{
    void inline log(std::string s, int priority = 0)
    {
#ifndef LOG_OFF
        if (priority > 0)
            std::cout << s << std::endl;
#endif
    }

    inline map<int, vector<int>> getNeighbors(vector<pair<double, double>> v, double r)
    {
        map<int, vector<int>> res; // map a node to other nodes that are in its proximity
        for (size_t i = 0; i < v.size(); i++)
        {
            for (size_t j = 0; j < v.size(); j++)
            {
                if (i == j)
                    continue;
                if (sqrt(pow(v[i].first - v[j].first, 2) + pow(v[i].second - v[j].second, 2)) < r)
                    res[i].push_back(j);
            }
        }

        return res;
    }

    inline map<int, vector<string>> createPubMap(map<int, vector<int>> m)
    {
        string baseTopic = "/zone/";
        map<int, vector<string>> res;
        for (auto e : m)
        {
            for (size_t i = 0; i < e.second.size(); i++)
            {
                res[e.second[i]].push_back(baseTopic + to_string(e.first));
            }
        }
        return res;
    }
}

#endif