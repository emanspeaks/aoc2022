#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <limits>
#include <iostream>
#include <queue>
#include <array>
#include <unordered_set>
#include <chrono>

#define PCTCOUNT 10

const int inf = std::numeric_limits<int>::max();

typedef std::array<int, 3> MapState;

enum Heading {
  nohead = 0,
          //-yx
  east  = 0b001,
  south = 0b010,
  west  = 0b101,
  north = 0b110,

  px    = 0b001,
  py    = 0b010,
  neg   = 0b100,
};

class Blizzard {
  public:
    Blizzard(int x0, int y0, Heading head): m_x0(x0), m_y0(y0),
      m_headx(head & px), m_headsign(1 - 2*(!!(head & neg))) {}

    void forecast(int t, int width, int height, int &x, int &y) {
      // cache the forecast and only recompute if it's a miss
      if (m_headx) {
        if (m_cache.count(t)) x = m_cache[t];
        else m_cache[t] = x = (width + m_x0 + (t % width)*m_headsign)%width;
        y = m_y0;
      } else {
        if (m_cache.count(t)) y = m_cache[t];
        else m_cache[t] = y = (height + m_y0 + (t % height)*m_headsign)%height;
        x = m_x0;
      }
    }

    void clearCache() {m_cache.clear();}

  private:
    int m_x0, m_y0, m_headsign;
    bool m_headx;
    std::unordered_map<int, int> m_cache;
};

class BlizzardValley {
  public:
    BlizzardValley(): m_height(0), m_global_min_t(inf) {}

    void parseLine(const std::string &s) {
      m_width = s.length() - 2;
      if (s[2] != '#') {
        for (int i = 1; i <= m_width; i++) {
          switch (s[i]) {
            case '>': m_blzs.push_back(Blizzard(i - 1, m_height, east )); break;
            case 'v': m_blzs.push_back(Blizzard(i - 1, m_height, south)); break;
            case '<': m_blzs.push_back(Blizzard(i - 1, m_height, west )); break;
            case '^': m_blzs.push_back(Blizzard(i - 1, m_height, north)); break;
            default: break;
          }
        }
        m_height++;
      }
    }

    int shortestDuration() {
      // find the first opening to enter the map
      int t = 1;
      while (isBadForecast(0, 0, t)) t++;
      std::cout << "waited to start until t = " << t << "\n";
      return computeMinStepsAStar({0, 0, t}, {m_width - 1, m_height - 1, 0});

      // int recurlimit = 10*(m_width + m_height);
      // return computeMinStepsFromHere(0, 0, t, recurlimit, "ZZ", false);
    }

    int shortestDurDoubleBack() {
      // find the first opening to enter the map
      int t = 1;
      while (isBadForecast(0, 0, t)) t++;
      std::cout << "waited to start until t = " << t << "\n";
      t = computeMinStepsAStar({0, 0, t}, {m_width - 1, m_height - 1, 0});
      std::cout << "trip 1: " << t << "\n";

      // for (int i = 0; i < 56; i++) std::cout << "t = " << i << "\n" << printWx(i) << "\n";

      int lastt = t;
      do {
        t = ++lastt;
        while (isBadForecast(m_width - 1, m_height - 1, t)) t++;
        std::cout << "waited to start until t = " << t << "\n";
        t = computeMinStepsAStar({m_width - 1, m_height - 1, t}, {0, 0, 0});
        if (t < 0) std::cout << "failed, try again\n";
      } while (t < 0);
      std::cout << "trip 2: " << (t - lastt) << "\n";

      lastt = t;
      do {
        t = ++lastt;
        while (isBadForecast(0, 0, t)) t++;
        std::cout << "waited to start until t = " << t << "\n";
        t = computeMinStepsAStar({0, 0, t}, {m_width - 1, m_height - 1, 0});
        if (t < 0) std::cout << "failed, try again\n";
      } while (t < 0);
      std::cout << "trip 3: " << (t - lastt) << "\n";

      return t;
    }

  private:
    std::vector<Blizzard> m_blzs;
    int m_width, m_height, m_global_min_t;

    bool isBadForecast(int x, int y, int t) {
      int bx, by;
      if (y >= 0) {
        for (auto &b: m_blzs) {
          b.forecast(t, m_width, m_height, bx, by);
          if (bx == x && by == y) return true;
        }
      }
      return false;
    }

    std::string printWx(int t) {
      std::string tmp((m_width+1)*m_height, '.');
      int bx, by;
      for (auto &b: m_blzs) {
        b.forecast(t, m_width, m_height, bx, by);
        tmp[by*(m_width+1)+bx] = '#';
      }
      for (int i = 0; i < m_height; i++) tmp[(i+1)*(m_width+1)-1] = '\n';
      return tmp;
    }

    void clearBlzsCache() {for (auto &b: m_blzs) b.clearCache();}

    bool isBadForecast(MapState state) {
      return isBadForecast(state[0], state[1], state[2]);
    }

    int computeMinStepsFromHere(int x0, int y0, int t0, int reclimit,
      std::string chain, bool print = false)
    {
      int min_t = inf, t = t0 + 1, x, y;
      char step;
      if (print) std::cout << "after " << t0 << ": " << chain << "\n";
      // break out of infinite loops, set recursion limit
      if (!(t % 100)) std::cout << "time check: " << t << ": " << chain << "\n";
      for (auto head: {nohead, east, south, west, north}) {
        if (t < std::min(reclimit, m_global_min_t)) {
          x = x0;
          y = y0;
          // take action, if possible
          switch (head) {
            case east: {
              step = 'E';
              if (y < 0 || x == m_width - 1 || isBadForecast(x + 1, y, t)) continue;
              x++;
              break;
            }
            case west: {
              step = 'W';
              if (y < 0 || !x || isBadForecast(x - 1, y, t)) continue;
              x--;
              break;
            }
            case south: {
              step = 'S';
              if (x != m_width - 1 && y == m_height - 1 || isBadForecast(x, y + 1, t))
                continue;
              y++;
              break;
            }
            case north: {
              step = 'N';
              if (y <= 0 || isBadForecast(x, y - 1, t)) continue;
              y--;
              break;
            }
            case nohead: {
              step = 'Z';
              if (isBadForecast(x, y, t)) continue;
              break;
            }
          }

          chain += step;
          if (x == m_width - 1 && y == m_height) {
            if (print) std::cout << "SOLUTION after " << t << ": " << chain << "\n";
            min_t = std::min(min_t, t);
            if (min_t < m_global_min_t) {
              m_global_min_t = min_t;
              std::cout << "new global min t " << m_global_min_t << "\n";
              std::cout << "solution to beat: " << chain << "\n";
            }
          } else {
            int tmp = computeMinStepsFromHere(x, y, t, reclimit, chain, print);
            min_t = std::min(min_t, tmp);
          }
        } // else std::cout << "hit rec limit of " << std::min(reclimit, m_global_min_t) << "\n";
      }
      return min_t;
    }

    int manhattanDistToTgt(MapState state, MapState tgt) {
      return abs(tgt[0] - state[0]) + abs(tgt[1] - state[1]);
    }

    std::string state2key(MapState state) {
      return std::to_string(state[0]) + "," + std::to_string(state[1]) + "," + std::to_string(state[2]);
    }

    int computeMinStepsAStar(MapState state_0, MapState state_f)
    {
      int min_t = inf, gtmp;
      MapState state = state_0, newstate;

      std::unordered_map<std::string, MapState> steps;
      std::unordered_map<std::string, int> f, g;
      auto fcompare = [&](const std::string &a, const std::string &b) {
        return f[a] > f[b];
      };
      std::priority_queue<std::string, std::vector<std::string>, decltype(fcompare)> openq(fcompare);
      std::unordered_set<std::string> openset;

      std::string current = state2key(state);
      steps[current] = state;
      g[current] = 0;
      f[current] = manhattanDistToTgt(state, state_f);
      openq.push(current);
      openset.insert(current);

      std::string newkey;
      int iters = 0, dist2go, pct[PCTCOUNT], maxpct, lastpct = 0, t;
      std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
      while (!openq.empty()) {
        if (!(++iters % 1000)) std::cout << "iter check: " << iters << "\n";

        current = openq.top();
        openset.erase(current);
        openq.pop();
        state = steps[current];
        dist2go = manhattanDistToTgt(state, state_f);
        // std::cout << "time check: " << state[2] << "\n";

        // calculate running average of last 10 percents, output if it changed
        t = state[2] - state_0[2];
        pct[(iters - 1)%PCTCOUNT] = 100*t/(t+dist2go);
        maxpct = *std::max_element(pct, pct + std::min(iters, PCTCOUNT));
        if (!(maxpct % 5) && maxpct != lastpct) {
          std::cout << "progress check: " << maxpct << "%\n";
          lastpct = maxpct;
        }
        // avgpct = 0;
        // for (int i = 0; i < std::min(iters, PCTCOUNT); i++) avgpct += pct[i];
        // avgpct /= std::min(iters, PCTCOUNT);
        // if (!(avgpct % 5) && avgpct != lastpct) {
        //   std::cout << "progress check: " << avgpct << "%\n";
        //   lastpct = avgpct;
        // }

        if (!dist2go) {
          std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
          std::cout << "run time: " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << " [s]" << "\n";
          std::cout << "final iters: " << iters << "\n";
          return state[2] + 1; // + 1 for last step out
        }

        for (auto head: {nohead, east, south, west, north}) {
          newstate = state;
          newstate[2]++;
          switch (head) {
            case east: {
              newstate[0]++;
              if (newstate[0] == m_width || isBadForecast(newstate)) continue;
              break;
            }
            case west: {
              newstate[0]--;
              if (newstate[0] < 0 || isBadForecast(newstate)) continue;
              break;
            }
            case south: {
              newstate[1]++;
              if (newstate[1] == m_height || isBadForecast(newstate)) continue;
              break;
            }
            case north: {
              newstate[1]--;
              if (newstate[1] < 0 || isBadForecast(newstate)) continue;
              break;
            }
            case nohead: {
              if (isBadForecast(newstate)) continue;
              break;
            }
          }

          gtmp = g[current] + 1; // "distance" is fixed as 1 min
          newkey = state2key(newstate);
          if (!g.count(newkey) || gtmp < g[newkey]) {
            steps[newkey] = newstate;
            g[newkey] = gtmp;
            f[newkey] = gtmp + manhattanDistToTgt(newstate, state_f);
            if (openset.find(newkey) == openset.end()) {
              openq.push(newkey);
              openset.insert(newkey);
            }
          }
        }

      }

      return -1;
    }
};
