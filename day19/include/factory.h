#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <algorithm>
#include <iostream>

#define RUNTIME 24
#define RUNTIMEPT2 32

// std::string maxchain;
// int maxmaxgeo = 0;

struct Blueprint {
  int ore_ore, clay_ore, obs_ore, obs_clay, geo_ore, geo_obs;
};

#define BOTCOUNT 4
enum BotIdx {orebot_i, claybot_i, obsbot_i, geobot_i};
// enum BotFlag {orebot_f = 1, claybot_f = 2, obsbot_f = 4, geobot_f = 8};
const char BotChar[BOTCOUNT] = {'R', 'C', 'O', 'G'};

struct Inventory {
  int ore = 0, clay = 0, obs = 0, geo = 0;
  int orebot = 1, claybot = 0, obsbot = 0, geobot = 0;
};

typedef std::unordered_map<std::string, Inventory> InvMap;

class Factory {
  public:
    Factory() {}

    void parseLine(const std::string &s) {
      // 0         1  2    3   4     5     6 7    8    9    10    11   12 13   14   15       16    17   18 19  20  21 22    23   24    25    26   27 28  29 30 31
      // Blueprint 1: Each ore robot costs 4 ore. Each clay robot costs 2 ore. Each obsidian robot costs 3 ore and 14 clay. Each geode robot costs 2 ore and 7 obsidian.
      std::stringstream sstm(s);
      std::vector<std::string> stok;
      std::string tmp;
      while (std::getline(sstm, tmp, ' ')) stok.push_back(tmp);
      Blueprint btmp;
      btmp.ore_ore = std::stoi(stok[6]);
      btmp.clay_ore = std::stoi(stok[12]);
      btmp.obs_ore = std::stoi(stok[18]);
      btmp.obs_clay = std::stoi(stok[21]);
      btmp.geo_ore = std::stoi(stok[27]);
      btmp.geo_obs = std::stoi(stok[30]);
      m_bp.push_back(btmp);
    }

    int sumMaxQuality() {
      int sum = 0;

      std::cout << "Blueprint: ";
      for (int i = 1; i <= m_bp.size(); i++) {
        std::cout << i << ", ";
        sum += i*maxGeodes(m_bp[i - 1], RUNTIME);
      }
      std::cout << "\n";
      return sum;
    }

    long long firstThreeMaxGeodeProduct() {
      long long prod = 1;
      int tmp;

      for (int i = 1; i <= std::min(3, (int)m_bp.size()); i++) {
        std::cout << "Blueprint " << i << ": ";
        tmp = maxGeodes(m_bp[i - 1], RUNTIMEPT2);
        prod *= tmp;
        std::cout << tmp << "\n";
      }
      return prod;
    }

  private:
    std::vector<Blueprint> m_bp;

    int computeTimeToBot(Inventory &inv, char idx, const Blueprint &bp) {
      int maxtgo;

      // + 1 rounds up if you're one short, +1 for mining during the minute the bot is built
      // pre-subtract off usage to simplify code at the end
      // NOTE: C++ integer division with negatives rounds UP towards 0,
      // hence needing to force 2go values that will result in -1 for that term.
      switch (idx)
      {
        case orebot_i: {
          int ore2go = bp.ore_ore - inv.ore;
          if (ore2go <= 0) ore2go = -inv.orebot;
          maxtgo = (ore2go - 1)/inv.orebot + 2;
          inv.ore -= bp.ore_ore;
          break;
        }

        case claybot_i: {
          int ore2go = bp.clay_ore - inv.ore;
          if (ore2go <= 0) ore2go = -inv.orebot;
          maxtgo = (ore2go - 1)/inv.orebot + 2;
          inv.ore -= bp.clay_ore;
          break;
        }

        case obsbot_i: { // ore and clay
          if (!inv.claybot) return -1;
          int ore2go = bp.obs_ore - inv.ore;
          if (ore2go <= 0) ore2go = -inv.orebot;
          int clay2go = bp.obs_clay - inv.clay;
          if (clay2go <= 0) clay2go = -inv.claybot;
          maxtgo = std::max((ore2go - 1)/inv.orebot + 2, (clay2go - 1)/inv.claybot + 2);
          inv.ore -= bp.obs_ore;
          inv.clay -= bp.obs_clay;
          break;
        }

        case geobot_i: { // ore and obs
          if (!inv.obsbot) return -1;
          int ore2go = bp.geo_ore - inv.ore;
          if (ore2go <= 0) ore2go = -inv.orebot;
          int obs2go = bp.geo_obs - inv.obs;
          if (obs2go <= 0) obs2go = -inv.obsbot;
          maxtgo = std::max((ore2go - 1)/inv.orebot + 2, (obs2go - 1)/inv.obsbot + 2);
          inv.ore -= bp.geo_ore;
          inv.obs -= bp.geo_obs;
          break;
        }
      }
      // other minerals
      inv.ore += inv.orebot*maxtgo;
      inv.clay += inv.claybot*maxtgo;
      inv.obs += inv.obsbot*maxtgo;
      // inv.geo += inv.geobot*maxtgo; // handle this in main loop

      switch (idx)
      {
        case orebot_i: {inv.orebot++; break;}
        case claybot_i: {inv.claybot++; break;}
        case obsbot_i: {inv.obsbot++; break;}
        case geobot_i: {inv.geobot++; break;}
      }
      return maxtgo;
    }

    int computeGeodes(int maxgeo, const Blueprint &bp, InvMap &inv, int timeleft,
      std::string chain)
    {
      int tmptime;
      Inventory tmpinv, baseinv;
      std::string newchain;
      if (inv.count(chain)) baseinv = inv[chain];
      for (int i = 0; i < BOTCOUNT; i++) {
        newchain = chain + BotChar[i];
        if (inv.count(newchain)) {
          maxgeo = std::max(maxgeo, inv[newchain].geo);
        } else {
          // compute inventory and add to map
          tmpinv = baseinv;
          tmptime = computeTimeToBot(tmpinv, i, bp);
          if (tmptime < 0 || tmptime > timeleft) continue;
          tmptime = timeleft - tmptime;
          // run out the clock for geodes in case we do nothing else
          if (i == geobot_i) {
            tmpinv.geo += tmptime;
            maxgeo = std::max(maxgeo, tmpinv.geo);
          }
          // if we can't possibly beat the max by adding geobots, give up.
          if (tmpinv.geo + (tmptime - 1)*tmptime/2 < maxgeo) continue;
          inv[newchain] = tmpinv;
          // if (tmpgeo > maxmaxgeo) {maxchain = newchain; maxmaxgeo = tmpgeo;}

          maxgeo = computeGeodes(maxgeo, bp, inv, tmptime, newchain);
        }

      }
      return maxgeo;
    }

    int maxGeodes(const Blueprint &bp, int timelimit) {
      // InvMap inv;
      // maxchain = "";
      // maxmaxgeo = 0;
      // int maxgeo = computeGeodes(0, bp, inv, RUNTIME, "");
      // if (inv.count("CCCOCOGG")) std::cout << "CCCOCOGG: " << inv["CCCOCOGG"].geo << "\n";
      // if (inv.count("RRCCCCCCOOOOCOGOGOG")) std::cout << "RRCCCCCCOOOOCOGOGOG: " << inv["RRCCCCCCOOOOCOGOGOG"].geo << "\n";
      // std::cout << maxchain << ": " << inv[maxchain].geo << "\n";
      // debugChain("CCCOCOGG", bp, RUNTIME);
      // debugChain("RRCCCCCCOOOOCOGOGOG", bp, RUNTIME);
      int maxgeo = computeGeodesMapless(0, bp, Inventory(), timelimit);
      return maxgeo;
    }

    int debugChain(std::string chain, const Blueprint &bp, int starttime) {
      int tmptime, timeleft = starttime;
      Inventory tmpinv;
      int i, t = 0;

      for (auto c: chain) {
        for (int j = 0; j < BOTCOUNT; j++) if (c == BotChar[j]) i = j;

        // compute inventory and add to map
        tmptime = computeTimeToBot(tmpinv, i, bp);
        t += tmptime;
        if (tmptime < 0 || tmptime > timeleft) continue;
        tmptime = timeleft - tmptime;
        // run out the clock for geodes in case we do nothing else
        if (i == geobot_i) {
          tmpinv.geo += tmptime;
        }
        timeleft = tmptime;
      }
      return tmpinv.geo;
    }

    int computeGeodesMapless(int maxgeo, const Blueprint &bp, Inventory baseinv, int timeleft) {
      int tmptime;
      Inventory tmpinv;
      for (int i = 0; i < BOTCOUNT; i++) {
        // compute inventory and add to map
        tmpinv = baseinv;
        tmptime = computeTimeToBot(tmpinv, i, bp);
        if (tmptime < 0 || tmptime > timeleft) continue;
        tmptime = timeleft - tmptime;
        // run out the clock for geodes in case we do nothing else
        if (i == geobot_i) {
          tmpinv.geo += tmptime;
          maxgeo = std::max(maxgeo, tmpinv.geo);
        }
        // if we can't possibly beat the max by adding geobots, give up.
        if (tmpinv.geo + (tmptime - 1)*tmptime/2 < maxgeo) continue;
        maxgeo = computeGeodesMapless(maxgeo, bp, tmpinv, tmptime);
      }
      return maxgeo;
    }
};
