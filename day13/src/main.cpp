#include <iostream>
#include <fstream>
#include <vector>

#include "main.h"
#include "packet.h"

int main(int argc, char const *argv[]) {
  std::cout << "Day13A\n";

  std::ifstream ifs("input.txt");
  std::string line;
  std::vector<Packet> pkt;

  if (ifs.is_open()){
    while (ifs) {
      std::getline(ifs, line);
      if (line.length()) {
        pkt.push_back(Packet(line));
      }
    }
  } else {
    std::cout << "failed to open input.txt";
    return 1;
  }

  int len = pkt.size()/2, sum = 0, tmp;
  for (int i = 0; i < len; i++) {
    tmp = pkt[2*i].compare(pkt[2*i+1]);
    if (tmp == 1) sum += i + 1;
  }

  std::cout << sum << "\n";

  std::cout << "Day13B\n";

  pkt.push_back(Packet("[[2]]"));
  pkt.push_back(Packet("[[6]]"));
  std::sort(pkt.begin(), pkt.end(), byPacketCompare);
  len = pkt.size();
  int idx1, idx2;
  std::string tmpstr;
  for (int i = 0; i < len; i++) {
    tmpstr = pkt[i].to_string();
    if (tmpstr == "[[2]]") idx1 = i + 1;
    if (tmpstr == "[[6]]") idx2 = i + 1;
  }

  std::cout << idx1*idx2 <<"\n";

  return 0;
}
