#pragma once

#include <string>
#include <vector>
#include <deque>

class SnafuConverter {
  public:
   SnafuConverter() {}

    void parseLine(const std::string &s) {
      std::deque<int> pv;
      for (auto c: s) {
        switch (c) {
          case '-': pv.push_front(-1); break;
          case '=': pv.push_front(-2); break;
          default: pv.push_front(c - '0'); break;
        }
      }
      m_values.push_back(pv);
    }

    void addPlaceValues(std::deque<int> &total, int addv, int addi) {
      int tmp = total[addi];
      tmp += addv;
      tmp += 2;
      if (tmp < 0) tmp -= 5; // carry
      int tmp2 = tmp / 5;
      tmp %= 5;
      if (tmp2 < 0) tmp += 5;
      tmp -= 2;
      if (tmp + tmp2*5 != total[addi] + addv || tmp>2 || tmp2>2 || tmp<-2 || tmp2<-2) throw;
      total[addi] = tmp;
      if (tmp2) {
        if (addi + 1 == total.size()) total.push_back(0);
        addPlaceValues(total, tmp2, addi + 1);
      }
    }

    std::string snafuTotal() {
      std::deque<int> total;
      for (auto v: m_values) {
        for (int i = 0; i < v.size(); i++) {
          if (i == total.size()) {total.push_back(v[i]); continue;}
          addPlaceValues(total, v[i], i);
        }
      }

      std::string out;
      for (int i = total.size() - 1; i >= 0; i--) {
        switch (total[i]) {
          case -2: out += '='; break;
          case -1: out += '-'; break;
          default: out += std::to_string(total[i]); break;
        }
      }
      return out;
    }

  private:
    std::vector<std::deque<int>> m_values;
};
