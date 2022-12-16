#include <iostream>
#include <fstream>

#include "main.h"
#include "sensormap.h"

#define TGTY 2000000
#define MAXCOORD 4000000
#define MINCOORD 0

// #define TGTY 10
// #define MAXCOORD 20
// #define MINCOORD 0

int main(int argc, char const *argv[]) {
  std::cout << "Day15A\n";

  std::ifstream ifs("input.txt");
  std::string line;
  SensorMap sm;

  if (ifs.is_open()){
    while (ifs) {
      std::getline(ifs, line);
      if (line.length()) {
        sm.parseLine(line);
      }
    }
  } else {
    std::cout << "failed to open input.txt";
    return 1;
  }

  Row r;
  sm.checkY(TGTY, r);

  std::cout << r.countTrue() << "\n";

  std::cout << "Day15B\n";
  // std::cout << sm.unknownBeaconSearch(MINCOORD, MAXCOORD) << "\n";
  std::cout << sm.fasterBeaconSearch(MINCOORD, MAXCOORD) << "\n";

  return 0;
}
