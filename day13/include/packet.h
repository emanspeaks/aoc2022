#pragma once

#include <string>
#include <memory>
#include <algorithm>
#include <vector>
#include <queue>

class PacketList;
class PacketInt;

enum PacketDataType {PKTINT, PKTLIST};

class PacketData {
  public:
    virtual int compare(const PacketList *right) const = 0;
    virtual int compare(const PacketInt *right) const = 0;
    virtual std::string to_string() = 0;
};

class PacketInt: public PacketData {
  public:
    PacketInt(const std::string &s): m_data(std::stoi(s)) {}

    int compare(const PacketList *right) const;
    int compare(const PacketInt *right) const;

    const int value() const {return m_data;}
    std::string to_string();
  private:
    int m_data;
};

class PacketList: public PacketData {
  public:
    PacketList(const std::string &&s) {
      // rvalue ref since the Packet constructor passes an rvalue from substr.
      // this should only get called for the top of the parser stack.
      std::queue<char> q;
      for (auto c: s) q.push(c);
      parseLine(q);
    }

    PacketList(std::queue<char> &q) {parseLine(q);}
    int compare(const PacketList *right) const;
    int compare(const PacketInt *right) const;
    // std::vector<PacketList*> getSortedList();
    std::string to_string();
  private:
    std::vector<std::unique_ptr<PacketData>> m_data;
    std::vector<PacketDataType> m_type;

    void addList(std::queue<char> &q);
    void addInt(const std::string &s);
    void parseLine(std::queue<char> &q);
};

class Packet {
  public:
    Packet(std::string s): m_data(std::string(s.substr(1, s.length() - 1))) {}

    int compare(const Packet &right) const {return m_data.compare(&right.m_data);}
    std::string to_string();
  private:
    PacketList m_data;
};

bool byPacketCompare(const Packet &i, const Packet &j);
