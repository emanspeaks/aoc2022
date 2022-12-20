#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <queue>
#include <algorithm>
#include <iostream>

enum MonkeyOp {plus, mult, sq};

typedef unsigned long long Item;

struct Monkey {
  std::queue<Item> items;
  MonkeyOp op;
  int opnum, testnum, true_id, false_id, inspectcount = 0;
};

bool byInspectCount(const Monkey* i, const Monkey* j) {
  // true if i before j
  return i->inspectcount < j->inspectcount;
}

class MonkeySim {
  public:
    MonkeySim(): m_linenum(0), m_base(1) {}

    void parseLine(const std::string &s) {
      if (m_linenum) {
        std::stringstream sstm(s);
        std::vector<std::string> stok;
        std::string tmp;
        while (std::getline(sstm, tmp, ' ')) stok.push_back(tmp);

        auto me = m_mnky.back().get();
        switch (m_linenum)
        {
          case 1: {
            // . . Starting items: 79, 98
            for (int i = 4; i < stok.size(); i++) me->items.push(std::stoi(stok[i]));
            break;
          }

          case 2: {
            // . . Operation: new = old * 19
            switch (stok[6][0])
            {
              case '*':
                me->op = mult;
                break;

              case '+':
                me->op = plus;
                break;

              default:
                throw;
            }
            if (stok[7] == "old") me->op = sq;
            else me->opnum = std::stoi(stok[7]);
            break;
          }

          case 3: {
            // . . Test: divisible by 23
            me->testnum = std::stoi(stok[5]);
            m_base *= me->testnum;
            break;
          }

          case 4: {
            // . . . . If true: throw to monkey 2
            me->true_id = std::stoi(stok[9]);
            break;
          }

          case 5: {
            // . . . . If false: throw to monkey 3
            me->false_id = std::stoi(stok[9]);
          }
        }
      } else m_mnky.push_back(std::make_unique<Monkey>());
      m_linenum++;
      m_linenum %= 6;
    }

    long long runSim(int runcount, bool part2 = false) {
      for (int i = 0; i < runcount; i++) {
        // if (!(i%100)) std::cout << "running: " << i << "\n";
        runRound(part2);
      }

      if (part2) for (auto &m: m_mnky) std::cout << "inspect: " << m.get()->inspectcount << "\n";

      std::vector<Monkey*> sorted;
      for (auto &m: m_mnky) sorted.push_back(m.get());
      std::sort(sorted.begin(), sorted.end(), byInspectCount);
      auto second = *(sorted.rbegin() + 1);
      long long one = sorted.back()->inspectcount, two = second->inspectcount;
      return one*two;
    }

  private:
    int m_linenum;
    std::vector<std::unique_ptr<Monkey>> m_mnky;
    unsigned long long m_base;

    void runRound(bool part2 = false) {
      Item i;
      unsigned long long tmp, test;
      Monkey* me;
      for (auto &m: m_mnky) {
        me = m.get();
        while (me->items.size()) {
          // inspect
          i = me->items.front();
          me->items.pop();
          me->inspectcount++;

          // worry
          switch (me->op)
          {
            case plus:
              i += me->opnum;
              break;

            case mult: {
              i *= me->opnum;
              break;
            }

            case sq: {
              i *= i;
            }
          }

          // relief
          if (part2) {
            // rescale the problem
            i %= m_base;
          } else i /= 3; // yeah, it's lazy, but idc about part 1 now

          // test
          // we know the upper bits are divisible because they are factors of
          // all these test numbers, so if the last bit is, the whole thing is.
          test = i % me->testnum;
          if (test) m_mnky[me->false_id].get()->items.push(std::move(i));
          else m_mnky[me->true_id].get()->items.push(std::move(i));
        }
      }
    }
};
