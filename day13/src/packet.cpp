#include "packet.h"

int PacketInt::compare(const PacketInt *right) const {
  return (m_data < right->m_data)? 1 : (m_data > right->m_data)? -1 : 0;
}

int PacketInt::compare(const PacketList *right) const {
  PacketList mytmp(std::to_string(m_data) + "]");
  return mytmp.compare(right);
}

int PacketList::compare(const PacketInt *right) const {
  PacketList rtmp(std::to_string(right->value()) + "]");
  return compare(&rtmp);
}

int PacketList::compare(const PacketList *right) const {
  // 1 = right, -1 = wrong, 0 = indeterminate
  int res = 0, mylen = m_data.size(), rlen = right->m_data.size();
  int minlen = std::min(mylen, rlen);
  PacketDataType mytype, rtype;
  for (int i = 0; i < minlen; i++) {
    mytype = m_type[i];
    rtype = right->m_type[i];
    if (mytype == PKTINT && rtype == PKTINT) {
      res = m_data[i].get()->compare(dynamic_cast<PacketInt*>(right->m_data[i].get()));
    } else if (mytype == PKTINT && rtype == PKTLIST) {
      res = m_data[i].get()->compare(dynamic_cast<PacketList*>(right->m_data[i].get()));
    } else if (mytype == PKTLIST && rtype == PKTINT) {
      res = m_data[i].get()->compare(dynamic_cast<PacketInt*>(right->m_data[i].get()));
    } else if (mytype == PKTLIST && rtype == PKTLIST) {
      res = m_data[i].get()->compare(dynamic_cast<PacketList*>(right->m_data[i].get()));
    }
    if (res) break;
  }

  if (!res) res = (mylen < rlen)? 1 : (mylen > rlen)? -1 : 0;

  return res;
}

void PacketList::addList(std::queue<char> &q) {
  m_data.push_back(std::make_unique<PacketList>(q));
  m_type.push_back(PKTLIST);
}

void PacketList::addInt(const std::string &s) {
  m_data.push_back(std::make_unique<PacketInt>(s));
  m_type.push_back(PKTINT);
}

void PacketList::parseLine(std::queue<char> &q) {
  std::string buf = "";
  char c;
  while (q.size()) {
    c = q.front();
    q.pop();
    switch (c)
    {
      case ',': {
        if (buf.length()) {
          addInt(buf);
          buf = "";
        }
        break;
      }

      case '[': {
        addList(q);
        break;
      }

      case ']': {
        if (buf.length()) {
          addInt(buf);
          buf = "";
        }
        return;
      }

      default: {
        buf += c;
        break;
      }
    }

  }
}

// std::vector<PacketList*> PacketList::getSortedList() {
//   std::vector<PacketList*> vec;
//   int len = m_data.size();
//   for (int i = 0; i < len; i++) vec.push_back(dynamic_cast<PacketList*>(m_data[i].get()));
//   std::sort(vec.begin(), vec.end(), byListCompare);
//   return vec;
// }

bool byPacketCompare(const Packet &i, const Packet &j) {
  // true if i before j
  return i.compare(j) > 0;
}

std::string PacketInt::to_string() {
  return std::to_string(m_data);
}

std::string PacketList::to_string() {
  std::string buf = "[";
  bool first = true;
  for (auto &x: m_data) {
    if (first) first = false;
    else buf += ',';
    buf += x.get()->to_string();
  }
  return buf + "]";
}

std::string Packet::to_string() {
  return m_data.to_string();
}
