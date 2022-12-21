#pragma once

#include <string>
#include <vector>
#include <deque>
#include <iostream>

#define DECRYPTKEY 811589153

class Ring {
  public:
    Ring() {}

    void parseLine(const std::string &s) {
      int idx = m_data.size();
      m_ring.push_back(idx);
      m_data.push_back(std::stoi(s));
    }

    int coordSum() {
      mix(false);

      int idx0 = findRingIdxOfFirstData(0);
      int first = get(idx0, 1000);
      int second = get(idx0, 2000);
      int third = get(idx0, 3000);

      std::cout << "first: " << first << "\n";
      std::cout << "second: " << second << "\n";
      std::cout << "third: " << third << "\n";

      return first + second + third;
    }

    long long coordSum2() {
      applyKeyAndResetRing();

      bool print = false;
      if (print) std::cout << "Mix 0: " << to_string() << "\n";
      for (int i = 1; i <= 10; i++) {
        std::cout << "Mix " << i;
        if (!print) std::cout << "\n";
        mix(false);
        if (print) std::cout << ": " << to_string() << "\n";
      }

      int idx0 = findRingIdxOfFirstData(0);
      long long first = get(idx0, 1000);
      long long second = get(idx0, 2000);
      long long third = get(idx0, 3000);

      std::cout << "first: " << first << "\n";
      std::cout << "second: " << second << "\n";
      std::cout << "third: " << third << "\n";

      return first + second + third;
    }

    std::string to_string() {
      std::string s = "";
      bool first = true;
      for (auto i: m_ring) {
        if (first) first = false;
        else s += ", ";
        s += std::to_string(m_data[i]);
      }
      return s;
    }

  private:
    std::vector<long long> m_data;
    std::deque<int> m_ring;

    long long get(int baseidx, long long i) {
      return m_data[m_ring[(baseidx + i) % m_data.size()]];
    }

    int findRingIdxOfFirstData(long long value) {
      int len = m_data.size();
      for (int i = 0; i < len; i++) if (m_data[i] == value) return data2ring(i);
      return -1;
    }

    int data2ring(int dataidx) {
      int len = m_ring.size();
      for (int i = 0; i < len; i++) if (m_ring[i] == dataidx) return i;
      return -1;
    }

    int moveLeft(int ringidx) {
      if (!ringidx) {
        m_ring.push_back(m_ring.back());
        *(m_ring.rbegin() + 1) = m_ring.front();
        m_ring.pop_front();
        return m_ring.size() - 2;
      } else {
        int tmp = m_ring[ringidx];
        m_ring[ringidx] = m_ring[ringidx - 1];
        m_ring[ringidx - 1] = tmp;
        return ringidx - 1;
      }
    }

    int moveRight(int ringidx) {
      if (ringidx == m_ring.size() - 1) {
        m_ring.push_front(m_ring.front());
        m_ring[1] = m_ring.back();
        m_ring.pop_back();
        return 1;
      } else {
        int tmp = m_ring[ringidx];
        m_ring[ringidx] = m_ring[ringidx + 1];
        m_ring[ringidx + 1] = tmp;
        return ringidx + 1;
      }
    }

    int moveRightBy(int ridx, long long offset) {
      int bound = offset % (m_ring.size() - 1);
      for (int j = 0; j < bound; j++) ridx = moveRight(ridx);
      return ridx;
    }

    int moveLeftBy(int ridx, long long offset) {
      int bound = abs(offset) % (m_ring.size() - 1);
      for (int j = 0; j < bound; j++) ridx = moveLeft(ridx);
      return ridx;
    }

    void mix(bool print = false) {
      int ridx;
      if (print) std::cout << "-1: " << to_string() << "\n";
      int len = m_data.size();
      for (int i = 0; i < len; i++) {
        if (!(i % 500)) std::cout << 100.0*i/len << "%\n";
        ridx = data2ring(i);
        if (m_data[i] > 0) ridx = moveRightBy(ridx, m_data[i]);
        else ridx = moveLeftBy(ridx, m_data[i]);
        if (print) std::cout << i << ": " << to_string() << "\n";
      }
    }

    void applyKeyAndResetRing() {
      int len = m_data.size();
      for (int i = 0; i < len; i++) {
        m_data[i] *= DECRYPTKEY;
        m_ring[i] = i;
      }
    }
};
