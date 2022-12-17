#pragma once

#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
#include <sstream>
#include <unordered_set>
#include <limits>
#include <algorithm>

#define TIMELIMIT 30

struct Valve {
  int rate;
  std::vector<std::string> next;
};

class Network {
  public:
    Network(): m_root("AA"), m_n(0) {}

    void parseLine(const std::string &s) {
      std::stringstream sstm(s);
      std::vector<std::string> stok;
      std::string tmp;
      while (std::getline(sstm, tmp, ' ')) stok.push_back(tmp);

      //Valve DR has flow rate=22; tunnels lead to valves DC, YA
      std::string name = stok[1];
      // if (m_root == "") m_root = name;
      m_data[name] = std::make_unique<Valve>();
      m_n++;
      Valve* v = m_data[name].get();

      v->rate = std::stoi(stok[4].substr(5, stok[4].length() - 6));
      int len = stok.size(), count = len - 9;
      std::string x;
      for (int i = 0; i < count; i++) {
        x = stok[len - i - 1];
        if (i) x = x.substr(0, x.length() - 1);
        v->next.push_back(x);
      }
    }

    int dynProgMaxCost() {
      std::unordered_set<std::string> unopened;
      for (auto &v: m_data) if (v.second.get()->rate) unopened.insert(v.first);

      std::unordered_map<std::string, std::unordered_map<std::string, int>> dist;
      for (auto v: unopened) for (auto u: unopened) dist[u][v] = dijkstra(u, v);
      for (auto v: unopened) dist[m_root][v] = dijkstra(m_root, v);

      std::unordered_map<std::string, int> cost;
      return computeChain(0, TIMELIMIT, "", m_root, unopened, cost, dist);
    }

    int computeChain(int maxcost, int timeleft, std::string chain, std::string here,
      std::unordered_set<std::string> unopened, std::unordered_map<std::string, int> &cost,
      std::unordered_map<std::string, std::unordered_map<std::string, int>> &dist)
    {
      std::string newchain;
      int tmpcost, traveltime;
      int basecost = (cost.count(chain))? cost[chain] : 0;
      for (auto v: unopened) {
        newchain = chain + v;
        if (cost.count(newchain)) tmpcost = cost[newchain];
        else {
          traveltime = dist[here][v];
          if ((traveltime + 1) >= timeleft) continue;
          tmpcost = basecost + getCost(v, timeleft - traveltime);
          cost[newchain] = tmpcost;

          std::unordered_set<std::string> newunopened(unopened);
          newunopened.erase(v);
          if (newunopened.size())
            tmpcost = computeChain(tmpcost, timeleft - traveltime - 1, newchain, v, newunopened, cost, dist);
        }
        maxcost = std::max(maxcost, tmpcost);
      }

      return maxcost;
    }

    int randyMaxCost() {
      std::unordered_set<std::string> unopened;
      for (auto &v: m_data) if (v.second.get()->rate) unopened.insert(v.first);
      int timeleft = TIMELIMIT;
      std::string here = m_root;
      int cost = 0;

      // iterate through all unopened valves and compute their cost from here
      int tmpcost, traveltime, maxdelta, maxtravel;
      std::string next = m_root;
      while (timeleft && unopened.size()) {
        maxtravel = 0;
        maxdelta = 0;
        for (auto v: unopened) {
          traveltime = dijkstra(here, v);
          tmpcost = getCost(v, timeleft - traveltime);
          if (tmpcost > maxdelta) {
            maxdelta = tmpcost;
            next = v;
            maxtravel = traveltime;
          }
        }

        // pull the trigger on the current max and drop from unopened
        timeleft -= traveltime + 1; //include valve open time
        cost += maxdelta;
        unopened.erase(next);
        here = next;
      }

      return cost;
    }

    int dijkstraMaxCost() {
      std::unordered_map<std::string, int> cost, timeleftafter;
      std::unordered_map<std::string, std::string> prev;
      std::unordered_set<std::string> q;

      for (auto &v: m_data) {
        cost[v.first] = -1;
        prev[v.first] = "";
        q.insert(v.first);
      }
      int tmp = getCost(m_root, TIMELIMIT);
      cost[m_root] = tmp;
      timeleftafter[m_root] = TIMELIMIT - 1 - (tmp>0);

      int max, tmptime;
      std::string u;
      Valve* udata;
      while (q.size()) {
        max = -1;
        for (auto v: q) {
          tmp = cost[v];
          if (tmp >= max) {
            u = v;
            max = tmp;
          }
        }
        q.erase(u);
        udata = m_data[u].get();

        for (auto v: udata->next) {
          if (q.find(v) == q.end()) continue;
          tmp = max + getCost(v, timeleftafter[u]);
          if (tmp > cost[v]) {
            cost[v] = tmp;
            tmptime = timeleftafter[u] - 1 - (tmp>0);
            if (tmptime < 0) tmptime = 0;
            timeleftafter[v] = tmptime;
            prev[v] = u;
          }
        }
      }

      // find max cost and return
      max = -1;
      for (auto &v: m_data) if (cost[v.first] > max) max = cost[v.first];
      return max;
    }

  private:
    std::unordered_map<std::string, std::unique_ptr<Valve>> m_data;
    std::string m_root;
    int m_n;

    int getCost(const std::string &v, int mins_left) {
      Valve* vdata = m_data[v].get();
      int rate = vdata->rate;
      int tmp = rate*(mins_left - 1);
      return tmp;
    }

    int dijkstra(const std::string &from, const std::string &to) {
      // degenerate case bailout:
      if (from == to) return 0;

      const int inf = std::numeric_limits<int>::max();

      std::unordered_map<std::string, int> dist;
      std::unordered_map<std::string, std::string> prev;
      std::unordered_set<std::string> q;

      for (auto &v: m_data) {
        dist[v.first] = inf;
        prev[v.first] = "";
        q.insert(v.first);
      }
      dist[from] = 0;

      int min, tmp;
      std::string u;
      Valve* udata;
      while (q.size()) {
        min = inf;
        for (auto v: q) {
          tmp = dist[v];
          if (tmp < min) {
            u = v;
            min = tmp;
          }
        }
        if (u == to) break;
        q.erase(u);
        udata = m_data[u].get();

        for (auto v: udata->next) {
          if (q.find(v) == q.end()) continue;
          tmp = min + 1; // we only consider adjacent, and all travel times/dist are 1
          if (tmp < dist[v]) {
            dist[v] = tmp;
            prev[v] = u;
          }
        }
      }

      return dist[to];
    }
};
