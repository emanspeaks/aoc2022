#pragma once

#include <string>
#include <unordered_set>
#include <limits>
#include <unordered_map>
#include <iostream>

class TopoMap {
  public:
    TopoMap(): m_data(""), m_width(0), m_height(0), m_sx(-1), m_sy(-1), m_ex(-1), m_ey(-1) {}

    void parseLine(std::string s) {
      m_width = s.length();
      m_height++;
      for (int i = 0; i < m_width; i++) {
        if (s[i] == 'S') {m_sx = i; m_sy = m_height - 1; s[i] = 'a';}
        if (s[i] == 'E') {m_ex = i; m_ey = m_height - 1; s[i] = 'z';}
      }
      m_data += s;
    }

    int minSteps() {return dijkstra(m_sx, m_sy, m_ex, m_ey);}

    int minStepsFromA() {
      int min = std::numeric_limits<int>::max(), len = m_data.length(), x, y, tmp;
      std::cout << "i: ";
      for (int i = 0; i < len; i++) {
        if (m_data[i] == 'a') {
          std::cout << i << ", ";
          i2xy(i, x, y);
          tmp = dijkstra(x, y, m_ex, m_ey);
          if (tmp < min) min = tmp;
        }
      }
      std::cout << "\n";
      return min;
    }

  private:
    std::string m_data;
    int m_width, m_height, m_sx, m_sy, m_ex, m_ey;

    int xy2i(int x, int y) {return y*m_width + x;}
    void i2xy(int i, int &x, int &y) {x = i % m_width; y = i/m_width;}
    char get(int x, int y) {return m_data[xy2i(x, y)];}

    std::vector<int> next(int u) {
      std::vector<int> out;
      int ux, uy, x, y;
      char uv = m_data[u], tmp;
      i2xy(u, ux, uy);
      if (ux > 0) {
        x = ux - 1;
        y = uy;
        tmp = get(x, y);
        if (tmp - uv <= 1) out.push_back(xy2i(x, y));
      }
      if (ux < m_width - 1) {
        x = ux + 1;
        y = uy;
        tmp = get(x, y);
        if (tmp - uv <= 1) out.push_back(xy2i(x, y));
      }
      if (uy > 0) {
        x = ux;
        y = uy - 1;
        tmp = get(x, y);
        if (tmp - uv <= 1) out.push_back(xy2i(x, y));
      }
      if (uy < m_height - 1) {
        x = ux;
        y = uy + 1;
        tmp = get(x, y);
        if (tmp - uv <= 1) out.push_back(xy2i(x, y));
      }
      return out;
    }

    int dijkstra(int fromx, int fromy, int tox, int toy) {
      int from = xy2i(fromx, fromy), to = xy2i(tox, toy);

      // degenerate case bailout:
      if (from == to) return 0;

      const int inf = std::numeric_limits<int>::max();

      std::unordered_map<int, int> dist;
      std::unordered_map<int, int> prev;
      std::unordered_set<int> q;
      std::vector<int> candidates;

      int len = m_data.length();
      for (int i = 0; i < len; i++) {
        dist[i] = inf;
        prev[i] = -1;
        q.insert(i);
      }
      dist[from] = 0;

      int min, tmp;
      int u, ulast = -1;
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
        if (u == ulast) break;
        q.erase(u);

        candidates = next(u);
        // if (!candidates.size()) break;
        for (auto v: candidates) {
          if (q.find(v) == q.end()) continue;
          tmp = min + 1; // we only consider adjacent, and all travel times/dist are 1
          if (tmp < dist[v]) {
            dist[v] = tmp;
            prev[v] = u;
          }
        }
        ulast = u;
      }

      return dist[to];
    }
};
