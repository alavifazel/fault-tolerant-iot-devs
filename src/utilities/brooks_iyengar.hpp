#ifndef BROOKS_IYENGAR_HPP
#define BROOKS_IYENGAR_HPP

#include <iostream>
#include <utility>
#include <string>
#include <vector>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <cassert>
#include <map>

using namespace std;

vector<pair<double,int>> buildTuples(vector<pair<double,double>> values)
{
    vector<pair<double,int>> res;
    for(size_t i = 0; i < values.size(); ++i)
    {
        if(values[i].second < values[i].first)
        {
            throw std::runtime_error("Second value should be less than the first.");
        }
        res.push_back(make_pair(values[i].first,1));
        res.push_back(make_pair(values[i].second,-1));
    }
    return res;
}

vector<pair<double,int>> sortTuples(vector<pair<double,int>> tuples)
{
    std::sort(tuples.begin(), tuples.end(), [](pair<double,int> first, pair<double,int> second)
    {
            return first.first < second.first;
    });

    return tuples;
}

inline double brooksIyengar(vector<pair<double,double>> values, int numberOfFaultyNodes)
{

    assert(values.size() > numberOfFaultyNodes);
    auto tuples = sortTuples(buildTuples(values));
    int N = values.size();
    
    vector<pair<int,pair<double,double>>> weights;

    // for(auto v : tuples)
    // {
    //     cout << v.first << " " << v.second << endl;
    // }

    int sum = 0;
    for(size_t i = 0; i < tuples.size(); ++i)
    {
        sum += tuples[i].second;

        if(i == 0)
        {
            weights.push_back(make_pair(sum, make_pair(tuples[i].first, NULL)));
        }
        else if (i == tuples.size() - 1)
        {
            weights.back().second.second = tuples[i].first;
        }
        else
        {
            weights.back().second.second = tuples[i].first;
            weights.push_back(make_pair(sum, make_pair(tuples[i].first, NULL)));
        }
    }

    double result = 0;
    double weightCounter = 0;
    for(int i = 0; i < weights.size(); ++i)
    {
        if(weights[i].first > numberOfFaultyNodes)
        {
            result += weights[i].first * (weights[i].second.second + weights[i].second.first) / 2;
            weightCounter += weights[i].first;
        }
    }
    return result / weightCounter;
}


inline double brooksIyengar(vector<double> values, int numberOfFaultyNodes)
{
    vector<pair<double,double>> tmp;
    for (size_t i = 0; i < values.size(); i++)
    {
        tmp.push_back(make_pair(values[i] - 1, values[i] + 1));
    }
    return brooksIyengar(tmp, 1);
}


#endif