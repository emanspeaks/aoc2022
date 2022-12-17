#pragma once

#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
#include <sstream>
#include <unordered_set>
#include <limits>
#include <algorithm>
#include <iostream>

#define TIMELIMIT 30
#define TIMELIMITPT2 26

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

    int dynProgDualMaxCost() {
      int maxcost = 0;

      std::cout << "setting up...";
      std::unordered_set<std::string> unopened;
      for (auto &v: m_data) if (v.second.get()->rate) unopened.insert(v.first);

      std::unordered_map<std::string, std::unordered_map<std::string, int>> dist;
      std::unordered_map<std::string, int> decoder;
      int i = 0;
      for (auto v: unopened) {
        for (auto u: unopened) dist[u][v] = dijkstra(u, v);
        dist[m_root][v] = dijkstra(m_root, v);
        decoder[v] = i++;
      }
      std::cout << "complete\n";

      // find all time-feasible chains first
      std::cout << "building chains...";
      std::unordered_map<std::string, uint64_t> visited;
      buildChain(TIMELIMITPT2, "", m_root, decoder, unopened, visited, dist);
      std::cout << "complete\n";

      // compute cost for remaining chains
      std::cout << "compute costs...";
      std::unordered_map<std::string, int> cost;
      computeChainCost(TIMELIMITPT2, visited, dist, cost);
      std::cout << "complete\n";

      // compute sums of non-overlapping, find max
      std::cout << "max search...\n";
      int tmpcost;
      int j = 0, total = cost.size();
      long searchsize = total*total, current = 0;
      i = 0;
      for (auto a: cost) {
        i++;
        for (auto b: cost) {
          j++;
          current++;
          if (!(current%1000000)) std::cout << 100.0*current/searchsize << "%\n";
          if (a.first == b.first) continue;
          if (visited[a.first] & visited[b.first]) continue;
          tmpcost = a.second + b.second;
          if (tmpcost > maxcost)
            maxcost = tmpcost;
        }
      }
      std::cout << "complete\n";

      return maxcost;
    }

    // NOTE: THIS TOOK TOO LONG AND TOO INEFFICIENT
    // int dynProgDualMaxCost() {
    //   std::cout << "setting up\n";
    //   std::unordered_set<std::string> unopened;
    //   std::unordered_map<std::string, std::unordered_map<std::string, int>> dist;
    //   for (auto &v: m_data) {
    //     if (v.second.get()->rate) unopened.insert(v.first);
    //     for (auto &u: m_data) dist[u.first][v.first] = dijkstra(u.first, v.first);
    //   }

    //   std::unordered_map<std::string, int> cost;

    //   // compute all the dual permutations first
    //   std::unordered_set<std::string> startcombos;
    //   for (auto v: unopened) {
    //     for (auto u: unopened) {
    //       if (u == v) continue;
    //       startcombos.insert((u < v)? u + v : v + u);
    //     }
    //   }
    //   int combocount = startcombos.size();
    //   std::cout << "combocount: " << combocount << "\n";

    //   std::string a, b, chain;
    //   int dista, distb, tmpcost, maxcost = 0, i = 0;
    //   for (auto pair: startcombos) {
    //     a = pair.substr(0, 2);
    //     b = pair.substr(2, 2);
    //     dista = dist[m_root][a];
    //     distb = dist[m_root][b];

    //     if ((dista + 1) >= TIMELIMITPT2 || (distb + 1) >= TIMELIMITPT2) continue;
    //     chain = (dista < distb)? 'a' + a + 'b' + b : 'b' + b + 'a' + a;
    //     tmpcost = getCost(a, TIMELIMITPT2 - dista) + getCost(b, TIMELIMITPT2 - distb);
    //     cost[chain] = tmpcost;
    //     std::unordered_set<std::string> newunopened(unopened);
    //     newunopened.erase(a);
    //     newunopened.erase(b);
    //     std::cout << "test: " << chain << " (" << 100.0*++i/combocount << "%)\n";
    //     tmpcost = computeDualChain(maxcost, a, TIMELIMITPT2 - dista - 1, b, TIMELIMITPT2 - distb - 1,
    //       chain, newunopened, cost, dist);
    //     if (tmpcost > maxcost) maxcost = tmpcost;
    //   }

    //   return maxcost;
    // }

    // int randyMaxCost() {
    //   std::unordered_set<std::string> unopened;
    //   for (auto &v: m_data) if (v.second.get()->rate) unopened.insert(v.first);
    //   int timeleft = TIMELIMIT;
    //   std::string here = m_root;
    //   int cost = 0;

    //   // iterate through all unopened valves and compute their cost from here
    //   int tmpcost, traveltime, maxdelta, maxtravel;
    //   std::string next = m_root;
    //   while (timeleft && unopened.size()) {
    //     maxtravel = 0;
    //     maxdelta = 0;
    //     for (auto v: unopened) {
    //       traveltime = dijkstra(here, v);
    //       tmpcost = getCost(v, timeleft - traveltime);
    //       if (tmpcost > maxdelta) {
    //         maxdelta = tmpcost;
    //         next = v;
    //         maxtravel = traveltime;
    //       }
    //     }

    //     // pull the trigger on the current max and drop from unopened
    //     timeleft -= traveltime + 1; //include valve open time
    //     cost += maxdelta;
    //     unopened.erase(next);
    //     here = next;
    //   }

    //   return cost;
    // }

    // int dijkstraMaxCost() {
    //   std::unordered_map<std::string, int> cost, timeleftafter;
    //   std::unordered_map<std::string, std::string> prev;
    //   std::unordered_set<std::string> q;

    //   for (auto &v: m_data) {
    //     cost[v.first] = -1;
    //     prev[v.first] = "";
    //     q.insert(v.first);
    //   }
    //   int tmp = getCost(m_root, TIMELIMIT);
    //   cost[m_root] = tmp;
    //   timeleftafter[m_root] = TIMELIMIT - 1 - (tmp>0);

    //   int max, tmptime;
    //   std::string u;
    //   Valve* udata;
    //   while (q.size()) {
    //     max = -1;
    //     for (auto v: q) {
    //       tmp = cost[v];
    //       if (tmp >= max) {
    //         u = v;
    //         max = tmp;
    //       }
    //     }
    //     q.erase(u);
    //     udata = m_data[u].get();

    //     for (auto v: udata->next) {
    //       if (q.find(v) == q.end()) continue;
    //       tmp = max + getCost(v, timeleftafter[u]);
    //       if (tmp > cost[v]) {
    //         cost[v] = tmp;
    //         tmptime = timeleftafter[u] - 1 - (tmp>0);
    //         if (tmptime < 0) tmptime = 0;
    //         timeleftafter[v] = tmptime;
    //         prev[v] = u;
    //       }
    //     }
    //   }

    //   // find max cost and return
    //   max = -1;
    //   for (auto &v: m_data) if (cost[v.first] > max) max = cost[v.first];
    //   return max;
    // }

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

    int computeDualChain(int maxcost, std::string a, int atimeleft, std::string b, int btimeleft,
      std::string chain, std::unordered_set<std::string> unopened, std::unordered_map<std::string, int> &cost,
      std::unordered_map<std::string, std::unordered_map<std::string, int>> &dist)
    {
      std::string newchain;
      int tmpcost, traveltime, deltatime = abs(atimeleft - btimeleft);
      int basecost = (cost.count(chain))? cost[chain] : 0;
      for (auto v: unopened) {
        for (char c = 'a'; c < 'c'; c++) {
          newchain = chain + c + v;
          if (cost.count(newchain)) tmpcost = cost[newchain];
          else {
            // get this out of the way now, even if we end up not using it
            std::unordered_set<std::string> newunopened(unopened);
            newunopened.erase(v);

            switch (c) {
              case 'a': {
                traveltime = dist[a][v];
                if (atimeleft > btimeleft && traveltime < deltatime) continue;
                if ((traveltime + 1) >= atimeleft) continue;
                tmpcost = basecost + getCost(v, atimeleft - traveltime);
                cost[newchain] = tmpcost;
                if (newunopened.size())
                  tmpcost = computeDualChain(tmpcost, v, atimeleft - traveltime - 1, b, btimeleft,
                    newchain, newunopened, cost, dist);

                break;
              }

              case 'b': {
                traveltime = dist[b][v];
                if (btimeleft > atimeleft && traveltime < deltatime) continue;
                if ((traveltime + 1) >= btimeleft) continue;
                tmpcost = basecost + getCost(v, btimeleft - traveltime);
                cost[newchain] = tmpcost;
                if (newunopened.size())
                  tmpcost = computeDualChain(tmpcost, a, atimeleft, v, btimeleft - traveltime - 1,
                    newchain, newunopened, cost, dist);
                break;
              }
            }
          }
          maxcost = std::max(maxcost, tmpcost);
        }
      }
      return maxcost;
    }

    void buildChain(int timeleft, std::string chain, std::string here,
      std::unordered_map<std::string, int> &decoder,
      std::unordered_set<std::string> unopened, std::unordered_map<std::string, uint64_t> &visited,
      std::unordered_map<std::string, std::unordered_map<std::string, int>> &dist)
    {
      std::string newchain;
      int traveltime, newtime;
      uint64_t visit = (visited.count(chain))? visited[chain] : 0;
      for (auto v: unopened) {
        traveltime = dist[here][v];
        newtime = timeleft - traveltime - 1;
        if (newtime < 1) continue;
        newchain = chain + v;
        visited[newchain] = visit | (1 << decoder[v]);
        std::unordered_set<std::string> newunopened(unopened);
        newunopened.erase(v);
        buildChain(newtime, newchain, v, decoder, newunopened, visited, dist);
      }
    }

    void computeChainCost(int maxtime, std::unordered_map<std::string, uint64_t> &visited,
      std::unordered_map<std::string, std::unordered_map<std::string, int>> &dist,
      std::unordered_map<std::string, int> &cost)
    {
      int timeleft, traveltime, len, tmpcost;
      std::string here, next;
      for (auto visit: visited) {
        tmpcost = 0;
        timeleft = maxtime;
        here = m_root;
        len = visit.first.length();
        for (int i = 0; i < len; i += 2) {
          next = visit.first.substr(i, 2);
          traveltime = dist[here][next];
          timeleft -= traveltime;
          tmpcost += getCost(next, timeleft);
          timeleft--;
          here = next;
        }
        cost[visit.first] = tmpcost;
      }
    }
};
