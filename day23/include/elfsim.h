#pragma once

#include <string>

#include "elfmap.h"

#define MAXROUNDSPT1 10

enum ElfDirFlag {
  north = 0b0001,
  east  = 0b0010,
  south = 0b0100,
  west  = 0b1000,
};
enum ElfDirIdx {ni, ei, si, wi, nei, nwi, sei, swi};

struct Elf {
  Coord cd, proposed = {0, 0};
  bool canMove = true;
};

class ElfSim {
  public:
    ElfSim() {}

    void parseLine(const std::string &s) {
      static int y = 0;
      Coord tmp = {0, y++};
      for (auto c: s) {
        tmp[0]++;
        if (c == '#') {
          m_elves.push_back(Elf(tmp));
          m_currentmap.safeSet(tmp, m_elves.size());
        }
      }
    }

    int emptySpaces(int n = MAXROUNDSPT1, bool print = false) {
      if (print) m_currentmap.printGrid();

      for (int i = 0; i < n; i++) {
        elfRound(i);
        if (print) {
          std::cout << "iter " << i << ":\n";
          m_currentmap.printGrid();
        }
      }

      int mx, px, my, py, sum = 0;
      m_currentmap.getActualExtents(mx, px, my, py);
      for (int y = my; y <= py; y++) for (int x = mx; x <= px; x++)
        if (!m_currentmap.safeGet(x, y)) sum++;

      return sum;
    }

    int totalRounds() {
      int i = MAXROUNDSPT1;
      while (!elfRound(i)) i++;
      return i + 1;
    }

  private:
    ElfMap<int> m_currentmap;
    std::vector<Elf> m_elves;

    void doNothing(int i, ElfMap<int> &proposedmap) {
      m_elves[i].canMove = false;
      proposedmap.safeSet(m_elves[i].cd, i + 1);
    }

    void makeProposal(int i, Coord cd, ElfMap<int> &proposedmap) {
      Coord tmp = m_elves[i].cd;
      m_elves[i].proposed = cd;

      // check if there is an existing proposal first
      int tmpidx = proposedmap.safeGet(cd);
      if (tmpidx) {
        // conflict
        doNothing(i, proposedmap);
        if (tmpidx > 0) {
          // notify the other elf and fix them to the map
          doNothing(tmpidx - 1, proposedmap);
          proposedmap.safeSet(cd, -tmpidx);
        }
      } else {
        proposedmap.safeSet(cd, i + 1);  // success
      }
    }

    bool elfRound(int n) {
      int searchval[8], len = m_elves.size(), j;
      Coord tmp;
      std::array<Coord, 8> searchcd;
      bool done;
      ElfMap<int> proposedmap;

      for (int i = 0; i < len; i++) {
        tmp = m_elves[i].cd;
        searchcd.fill(tmp);
        for (auto k: {ei, nei, sei}) searchcd[k][0]++;
        for (auto k: {si, sei, swi}) searchcd[k][1]++;
        for (auto k: {wi, nwi, swi}) searchcd[k][0]--;
        for (auto k: {ni, nei, nwi}) searchcd[k][1]--;
        for (int k = 0; k < 8; k++) searchval[k] = m_currentmap.safeGet(searchcd[k]);


        // if no one is around, do nothing.
        done = true;
        for (auto k: searchval) if (k) done = false;
        if (done) doNothing(i, proposedmap);

        j = 0;
        while (!done && j < 4) {
          switch ((n + j++) % 4) {
            case 0:
              // If there is no Elf in the N, NE, or NW adjacent positions,
              // the Elf proposes moving north one step.
              if (searchval[ni] || searchval[nei] || searchval[nwi]) continue;
              else {
                done = true; // we're making a proposal, so call off the search
                makeProposal(i, searchcd[ni], proposedmap);
              }
              break;

            case 1:
              // If there is no Elf in the S, SE, or SW adjacent positions,
              // the Elf proposes moving south one step.
              if (searchval[si] || searchval[sei] || searchval[swi]) continue;
              else {
                done = true; // we're making a proposal, so call off the search
                makeProposal(i, searchcd[si], proposedmap);
              }
              break;

            case 2:
              // If there is no Elf in the W, NW, or SW adjacent positions,
              // the Elf proposes moving west one step.
              if (searchval[wi] || searchval[swi] || searchval[nwi]) continue;
              else {
                done = true; // we're making a proposal, so call off the search
                makeProposal(i, searchcd[wi], proposedmap);
              }
              break;

            case 3:
              // If there is no Elf in the E, NE, or SE adjacent positions,
              // the Elf proposes moving east one step.
              if (searchval[ei] || searchval[nei] || searchval[sei]) continue;
              else {
                done = true; // we're making a proposal, so call off the search
                makeProposal(i, searchcd[ei], proposedmap);
              }
              break;
          }
        }

        if (!done) doNothing(i, proposedmap); // do nothing if we have no solutions.
      }

      // execute moves
      done = true;
      for (int i = 0; i < len; i++)
        if (m_elves[i].canMove) {m_elves[i].cd = m_elves[i].proposed; done = false;}
        else m_elves[i].canMove = true;

      // clean the map, and then swap for production
      proposedmap.cleanMap();
      m_currentmap = std::move(proposedmap);
      return done;
    }
};
