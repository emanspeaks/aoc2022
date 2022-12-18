#include <iostream>
#include <fstream>
#include <algorithm>

#include "main.h"
#include "tetris.h"

#define MAXROCKS 2022
#define MAXROCKSPT2 1000000000000ULL
#define DEQUEBUFFER 100

int main(int argc, char const *argv[]) {
  std::cout << "Day17A\n";

  std::ifstream ifs("input.txt");
  std::string line, wind;

  if (ifs.is_open()){
    while (ifs) {
      std::getline(ifs, line);
      if (line.length()) {
        wind = line;
      }
    }
  } else {
    std::cout << "failed to open input.txt";
    return 1;
  }

  std::deque<char> rows;
  int lastfullrow, len, windidx = 0, windlen = wind.length();
  for (int i = 0; i < MAXROCKS; i++) {
    Rock r(shapelist[i % SHAPECOUNT]);

    // 0123(45)  len=6 lastfullrow=3

    // add buffer rows as needed
    len = rows.size();
    lastfullrow = -1;
    for (auto j: rows) if (j) lastfullrow++; else break;
    for (int j = 3 - (len - lastfullrow - 1) + r.height(); j > 0; j--) rows.push_back(0);

    r.setXY(2, lastfullrow + 4);
    while (!r.isAtRest()) {
      r.tryWind(wind[windidx++ % windlen], rows);
      r.tryDrop(rows);
    }
  }

  lastfullrow = -1;
  for (auto j: rows) if (j) lastfullrow++; else break;

  std::cout << lastfullrow + 1 << "\n";

  std::cout << "Day17B\n";

  // AUTOCORRELATION METHOD
  // This would also take forever to complete as I originally wrote with a 200k target,
  // so I paused the debugger a few times while running and consistently was
  // reporting the following:
  // maxi = 2753
  // maxsum = 312517
  // rows size = 315562
  // a difference of only 292 rows... interesting.
  // Unfortunately, I didn't save off the actual autocorrelation data or the
  // rows data, so I can't see the distribution of the matches and see if a real
  // pattern has emerged definitively.

  // that maxi number is in a reachable range, though...maybe I can rerun with
  // lower maxrocks again now that i'm not short-circuting the autocorr like I
  // was when I initially thought it wasn't working...
  // Might be easier to replicate with a less unwieldy and time-consuming
  // dataset.
  // Running with MAXROCKS*10 gave the same maxi and difference as above.
  // Going to further reduce to MAXROCKS*3 for speed.  Sure enough, it works now,
  // but the extra data from 3x is better for confidence and still timely.

  // // run 200k trials (because (5 rocks)*(10092 input chars)*(3 cycles) = ~150k plus margin)
  long long rockidx;
  ShapeDef shape;
  int shapeidx;
  for (int i = MAXROCKS; i < MAXROCKS*3; i++) {
    rockidx = i;
    if (!(i % 1000)) std::cout << i << " (" << 100.0*i/(MAXROCKS*3) << "%)\n";
    shapeidx = i % SHAPECOUNT;
    shape = shapelist[shapeidx];
    Rock r(shape);

    // 0123(45)  len=6 lastfullrow=3

    // add buffer rows as needed
    len = rows.size();
    lastfullrow = -1;
    for (auto j: rows) if (j) lastfullrow++; else break;
    for (int j = 3 - (len - lastfullrow - 1) + r.height(); j > 0; j--) rows.push_back(0);

    r.setXY(2, lastfullrow + 4);
    while (!r.isAtRest()) {
      r.tryWind(wind[windidx++ % windlen], rows);
      r.tryDrop(rows);
    }
  }
  std::cout << "rockidx: " << rockidx << "\n";

  lastfullrow = -1;
  for (auto j: rows) if (j) lastfullrow++; else break;

  // do autocorrelation with previous solution (manual test as proof of concept)
  // len = rows.size();
  // int offset = 2753;
  // std::vector<int> autocorr;
  // for (int j = 0; j < len - offset; j++) autocorr.push_back(rows[j] == rows[offset + j]);
  // std::cout << 0 << "\n";

  len = rows.size();
  int offset = 0, sum, maxsum = 0;
  for (int i = 1; i < len; i++) {
    sum = 0;
    if (!(i % 1000)) std::cout << i << " (" << 100.0*i/len << "%)\n";
    for (int j = 0; j < len - i; j++) {
      sum += rows[j] == rows[i + j];
    }
    if (sum > maxsum) {
      maxsum = sum;
      offset = i;
    }
  }
  std::cout << "offset: " << offset << "\n";

  // assume the offset is the period, but test this.  The first part of the
  // signal has "noise" due to the "cave floor", and finding the start of the
  // autocorr is more work than I wanna do algorithmically.  Let's just take
  // last 2753 data points, and see if they match with the datapoints 2753
  // before that.  Also drop the last DEQUEBUFFER rows since they're probably
  // empty or have "tail noise"
  int idx1, idx2;
  for (int i = 0; i < offset; i++) {
    idx1 = lastfullrow - DEQUEBUFFER;
    idx2 = idx1 - offset;
    if (rows[idx1] != rows [idx2]) throw; // if it doesn't work, we just cry.
  }

  // to be extra certain, we do a second autocorr trimming off the fronts.
  int period = 0;
  maxsum = 0;
  for (int i = 1; i < len - offset; i++) {
    sum = 0;
    if (!(i % 1000)) std::cout << i << " (" << 100.0*i/len << "%)\n";
    for (int j = offset; j < len - i; j++) {
      sum += rows[j] == rows[i + j];
    }
    if (sum > maxsum) {
      maxsum = sum;
      period = i;
    }
  }
  if (period != offset) throw;  // creys
  std::cout << "period: " << period << "\n";
  std::cout << "passes period test\n";

  // but we don't know how many ROCKS are in that cycle, or where the cycle starts.
  // Let's try finding the row with the lowest vertical gap and start there.
  // Then we can add more rocks, keeping track this time,
  // and see when we arrive at our same current final state.

  // make a working copy of rows for this bit to preserve original rows for comparison later.
  std::deque<char> rows2(rows);

  // find the last row with a vertical gap
  long long bottomrow = lastfullrow;
  char cols2go = 0b1111111;
  while (cols2go) {
    cols2go &= ~rows2[bottomrow];
    bottomrow--;
  }

  std::cout << "len: " << rows2.size() << "\n";
  std::cout << "bottomrow: " << bottomrow << "\n";

  // snip snip
  for (int i = bottomrow; i > 0; i--) rows2.pop_front();

  // capture last state before we sim
  int lastrockidx = rockidx, lastrockshapeidx = lastrockidx % SHAPECOUNT;
  int lastwindidx = (windidx - 1) % windlen;

  // do the sim again, but count the rocks and state
  std::cout << "rockidx: " << rockidx << "\n";
  rockidx++;
  long long lastfullrow2, len2, windidx2 = windidx, deltarock = 0;
  bool match = false;
  lastfullrow2 = bottomrow - 1;
  for (auto j: rows2) if (j) lastfullrow2++; else break;

  // common sense implies the deltarock needs to be a multiple of the shape count.
  // do the matching measure from the last vertical gap to bottom row since the
  // lastfullrow2 can still change as rocks are added.
  long long bottomrow2;
  while (!(match && !(deltarock % SHAPECOUNT))) {
    deltarock++;
    Rock r(shapelist[rockidx++ % SHAPECOUNT]);

    len2 = rows2.size() + bottomrow;
    for (int j = 3 - (len2 - lastfullrow2 - 1) + r.height(); j > 0; j--) rows2.push_back(0);

    r.setXY(2, lastfullrow2 + 4);
    while (!r.isAtRest()) {
      r.tryWind(wind[windidx2++ % windlen], rows2, bottomrow);
      r.tryDrop(rows2, bottomrow);
    }

    lastfullrow2 = bottomrow - 1;
    for (auto j: rows2) if (j) lastfullrow2++; else break;

    // check matching
    if (lastfullrow2 >= (bottomrow + offset)) {
      match = true;
      for (long long i = bottomrow; i < (bottomrow + offset); i++) {
        if (rows2[i - bottomrow] != rows[i - offset]) {
          match = false;
          break;
        }
        if (rows2[i - bottomrow] != rows[i - offset*2]) {
          match = false;
          break;
        }
      }

      if (match) {
        bottomrow2 = lastfullrow2;
        cols2go = 0b1111111;
        while (cols2go) {
          cols2go &= ~rows2[bottomrow2 - bottomrow];
          bottomrow2--;
        }
        match = (bottomrow2 - bottomrow) == offset;
      }
    }
  }
  // since we postfix increment, rockidx is now the rock count

  // get delta states
  long long deltawind = windidx2 - windidx;
  std::cout << "rockidx (rock count): " << rockidx << "\n";
  std::cout << "deltarock: " << deltarock << "\n";
  std::cout << "deltawind: " << deltawind << "\n";

  // extrapolate
  long long rocks2go = MAXROCKSPT2 - rockidx;
  long long cycles2go = rocks2go/deltarock; // yes, integer math.  intentional.
  rockidx += cycles2go*deltarock;
  rocks2go = MAXROCKSPT2 - rockidx;
  std::cout << "rocks2go: " << rocks2go << "\n";
  // in an ideal world, just keep current wind since it's a cycle
  // windidx2 += cycles2go*deltawind;
  bottomrow += offset*cycles2go;
  lastfullrow2 += offset*cycles2go;

  // fill remaining gap
  while (rockidx < MAXROCKSPT2) {
    Rock r(shapelist[rockidx++ % SHAPECOUNT]);

    len2 = rows2.size() + bottomrow;
    for (int j = 3 - (len2 - lastfullrow2 - 1) + r.height(); j > 0; j--) rows2.push_back(0);

    r.setXY(2, lastfullrow2 + 4);
    while (!r.isAtRest()) {
      r.tryWind(wind[windidx2++ % windlen], rows2, bottomrow);
      r.tryDrop(rows2, bottomrow);
    }

    lastfullrow2 = bottomrow - 1;
    for (auto j: rows2) if (j) lastfullrow2++; else break;
  }

  std::cout << lastfullrow2 + 1 <<"\n";


  // DIRECT SIMULATION...WOULD TAKE 57 DAYS (rip)
  // long long lastfullrow2, len2, windidx2 = windidx, bottomrow = 0;
  // for (long long i = MAXROCKS; i < MAXROCKSPT2; i++) {
  //   if (!(i % 1000000)) std::cout << i << " (" << 100.0*i/MAXROCKSPT2 << "%)\n";

  //   Rock r(shapelist[i % SHAPECOUNT]);

  //   // 0123(45)  len=6 lastfullrow=3

  //   // add buffer rows as needed
  //   len2 = rows.size() + bottomrow;
  //   lastfullrow2 = bottomrow - 1;
  //   for (auto j: rows) if (j) lastfullrow2++; else break;
  //   for (int j = 3 - (len2 - lastfullrow2 - 1) + r.height(); j > 0; j--) rows.push_back(0);

  //   // delete rows except last DEQUEBUFFER
  //   if ((lastfullrow2 - DEQUEBUFFER) > bottomrow)
  //     for (int j = lastfullrow2 - DEQUEBUFFER - bottomrow; j > 0; j--) rows.pop_front();
  //   bottomrow = lastfullrow2 - DEQUEBUFFER;

  //   r.setXY(2, lastfullrow2 + 4);
  //   while (!r.isAtRest()) {
  //     r.tryWind(wind[windidx2++ % windlen], rows, bottomrow);
  //     r.tryDrop(rows, bottomrow);
  //   }
  // }

  // lastfullrow2 = bottomrow - 1;
  // for (auto j: rows) if (j) lastfullrow2++; else break;

  // std::cout << lastfullrow2 <<"\n";

  return 0;
}
