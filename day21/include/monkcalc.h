#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <stack>

enum MonkOp {PLUS, MINUS, MULT, DIV};

class MonkCalc {
  public:
    MonkCalc() {}

    void parseLine(const std::string &s) {
      std::stringstream sstm(s);
      std::vector<std::string> stok;
      std::string tmp;
      while (std::getline(sstm, tmp, ' ')) stok.push_back(tmp);
      tmp = stok[0].substr(0, 4);
      if (stok[1][0] > '9') {
        m_op1[tmp] = stok[1];
        m_op2[tmp] = stok[3];
        switch (stok[2][0])
        {
          case '+': m_oper[tmp] = PLUS; break;
          case '-': m_oper[tmp] = MINUS; break;
          case '*': m_oper[tmp] = MULT; break;
          case '/': m_oper[tmp] = DIV; break;
        }
      } else m_monkval[tmp] = std::stoi(stok[1]);
    }

    long long root() {
      return get("root");
    }

    long long humn() {
      // determine which dependency chain includes humn
      // create an inverse tree
      std::string name, tmp;
      std::unordered_map<std::string, std::string> dep;
      // std::vector<std::string> q;
      std::stack<std::string> q;
      q.push("root");
      while (!q.empty()) {
        name = q.top();
        q.pop();
        if (m_op1.count(name)) {
          tmp = m_op1[name];
          q.push(tmp);
          dep[tmp] = name;
        }
        if (m_op2.count(name)) {
          tmp = m_op2[name];
          q.push(tmp);
          dep[tmp] = name;
        }
      }

      // if we got here, the stack is empty, so we can start reloading it.
      name = "humn";
      while (name != "root") {
        name = dep[name];
        q.push(name);
      }
      q.pop(); // get rid of root;

      long long val;
      if (q.top() == m_op1["root"]) {
        val = get(m_op2["root"]);
      } else val = get(m_op1["root"]);

      // follow stack to get value before me
      long long val2;
      std::string tmp2;
      bool qisop1;
      while (!q.empty()) {
        name = q.top();
        q.pop();
        tmp = m_op1[name];
        tmp2 = m_op2[name];

        if (tmp == ((tmp == "humn" || tmp2 == "humn")? "humn" : q.top())) {
          // q is op1, calc op2
          val2 = get(tmp2);
          qisop1 = true;
        } else {
          // q is op2, calc op1
          val2 = get(tmp);
          qisop1 = false;
        }

        switch (m_oper[name]) {
          case PLUS: val -= val2; break;
          case MINUS: if (qisop1) val += val2; else val = val2 - val; break;
          case MULT: val /= val2; break;
          case DIV: if (qisop1) val *= val2; else val = val2 / val; break;
        }
      }

      return val;
    }

  private:
    std::unordered_map<std::string, long long> m_monkval;
    std::unordered_map<std::string, std::string> m_op1, m_op2;
    std::unordered_map<std::string, MonkOp> m_oper;

    long long get(std::string name) {
      if (m_monkval.count(name)) return m_monkval[name];
      switch (m_oper[name]) {
        case PLUS: return get(m_op1[name]) + get(m_op2[name]);
        case MINUS: return get(m_op1[name]) - get(m_op2[name]);
        case MULT: return get(m_op1[name]) * get(m_op2[name]);
        case DIV: return get(m_op1[name]) / get(m_op2[name]);
      }
      throw;
    }
};
