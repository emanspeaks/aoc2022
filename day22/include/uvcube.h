#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <array>
#include <iostream>

#include "forcefield.h"

/*
* an unrolled cube can only physically occupy, at most, nx(7-n) space, where 1 < n < 6.
* find the longest line and the number of lines, and the shorter of those two is the n side.
* the only practical aspect ratios are then 3x4 and 2x5.  try dividing the short side by 3 and
* see if it equals the long side divided by 4.  if the result is equal, it's that one, else 2x5.
*
* The trick is knowing how the faces are connected when you don't know the layout.
* I could cheat by looking at my input but what's the fun in that???
*
* There is only one column in common between two adjacent rows, which
* I call the "overlap." As you move between the rows, use the overlap side of the row above to
* determine the next side below.
*
* By my convention, I will define the top as the side we start on, first in the first row.
* The side below top is always front unless it is blank below top, in which case front is
* the first side on the row.
*/

//                t        r          l         f          m           k
enum CubeSideIdx {topside, rightside, leftside, frontside, bottomside, backside};

enum AxisIdx {
  noaxi, pxi, pyi, pzi, mxi, myi, mzi
};

typedef std::array<int, 3> Coord;
typedef std::array<int, 3> AxisDef;
const AxisDef axes[7] = {
  { 0,  0,  0}, // noax
  { 1,  0,  0}, //px
  { 0,  1,  0}, //py
  { 0,  0,  1}, //pz
  {-1,  0,  0}, //mx
  { 0, -1,  0}, //my
  { 0,  0, -1}  //mz
};
const AxisIdx cross[7][7] = { // row x col
  //noax     px     py     pz     mx     my     mz
  {noaxi, noaxi, noaxi, noaxi, noaxi, noaxi, noaxi}, // noax x (col)
  {noaxi, noaxi,   pzi,   myi, noaxi,   mzi,   pyi}, // px x (col)
  {noaxi,   mzi, noaxi,   pxi,   pzi, noaxi,   mxi}, // py x (col)
  {noaxi,   pyi,   mxi, noaxi,   myi,   pxi, noaxi}, // pz x (col)
  {noaxi, noaxi,   mzi,   pyi, noaxi,   pzi,   myi}, // mx x (col)
  {noaxi,   pzi, noaxi,   mxi,   mzi, noaxi,   pxi}, // my x (col)
  {noaxi,   myi,   pxi, noaxi,   pyi,   mxi, noaxi}, // mz x (col)
};
const AxisIdx invax[7] = {noaxi, mxi, myi, mzi, pxi, pyi, pzi};

//                             t    r    l    f    m    k
const AxisIdx sidenormal[6] = {mzi, pxi, mxi, pyi, pzi, myi};

struct CubeSide {
  CubeSideIdx idx;
  AxisIdx eu;
  AxisIdx ev;
  int r0;
  int c0;
  Coord origin;
};

class UVCube: public ForceField {
  public:
    UVCube(ForceField ff): ForceField(ff) {}

    long long password() {
      Facing dir = faceright;
      CubeSideIdx side = topside;
      int r = 0, c = buildCubeLayout();
      int u, v;
      rc2uv(r, c, u, v);

      std::string buf;
      for (auto q: m_instr) {
        if (q > '9') {
          uv2rc(u, v, side, r, c);
          std::cout << buf << ": " << r << ", " << c << ", " << dir << "\n";
          moveForward(std::stoi(buf), u, v, dir, side);
          buf = "";
          dir = Facing(((q == 'R')? dir + 1 : dir + 3) % 4);
        } else buf += q;
      }
      uv2rc(u, v, side, r, c);
      std::cout << buf << ": " << r << ", " << c << ", " << dir << "\n";
      moveForward(std::stoi(buf), u, v, dir, side);

      uv2rc(u, v, side, r, c);
      std::cout << "final: " << r << ", " << c << ", " << dir << "\n";
      return 1000LL*(long long)(r + 1) + 4LL*(long long)(c + 1) + (long long)dir;
    }

  private:
    int m_sidelen;
    CubeSide m_sides[6];

    void moveCoordAlongAxis(Coord &pt, AxisIdx ax, int dist = 1) {
      for (int i = 0; i < 3; i++) pt[i] += axes[ax][i]*dist;
    }

    void rc2uv(int r, int c, int &u, int &v) {
      u = c % m_sidelen;
      v = r % m_sidelen;
    }

    void uv2rc(int u, int v, CubeSideIdx side, int &r, int &c) {
      c = m_sides[side].c0 + u;
      r = m_sides[side].r0 + v;
    }

    void uv2xyz(int u, int v, CubeSideIdx side, Coord &pt) {
      pt = m_sides[side].origin;
      moveCoordAlongAxis(pt, m_sides[side].eu, u);
      moveCoordAlongAxis(pt, m_sides[side].ev, v);
    }

    int projAlongAxis(Coord pt, AxisIdx ax) {
      for (int i = 0; i < 3; i++) if (axes[ax][i]) return pt[i]*axes[ax][i];
      return 0;
    }

    void dxyz(Coord pf, Coord p0, Coord &delta) {
      for (int i = 0; i < 3; i++) delta[i] = pf[i] - p0[i];
    }

    void xyz2uv(Coord pt, CubeSideIdx side, int &u, int &v) {
      Coord delta;
      dxyz(pt, m_sides[side].origin, delta);
      u = projAlongAxis(delta, m_sides[side].eu);
      v = projAlongAxis(delta, m_sides[side].ev);
    }

    int buildCubeLayout() {
      int maplen = m_mapdata.size();
      int shortaxis = std::min(m_maxlen, maplen);
      int longaxis = std::max(m_maxlen, maplen);
      m_sidelen = (shortaxis/3 == longaxis/4)? shortaxis/3 : shortaxis/2;

      int firstrowstart = getRowStart(0);
      int laststart = firstrowstart, lastend = m_mapdata[0].length();
      int lastcount = (lastend - laststart)/m_sidelen;
      Coord origin = {0, 0, 0};
      AxisIdx u = pxi, v = pyi;
      CubeSideIdx side = topside;

      // manually set top data, then go from there
      m_sides[side].idx = side;
      m_sides[side].r0 = 0;
      m_sides[side].c0 = laststart;
      m_sides[side].eu = u;
      m_sides[side].ev = v;
      m_sides[side].origin = origin;
      moveCoordAlongAxis(origin, u, m_sidelen - 1);

      // run rest of top row manually
      for (int c = laststart + m_sidelen; c < lastend; c += m_sidelen) {
        side = rightOfSide(side);
        u = cross[sidenormal[side]][v];
        m_sides[side].idx = side;
        m_sides[side].r0 = 0;
        m_sides[side].c0 = c;
        m_sides[side].eu = u;
        m_sides[side].ev = v;
        m_sides[side].origin = origin;
        moveCoordAlongAxis(origin, u, m_sidelen - 1);
      }

      int start, end, count;
      for (int r = m_sidelen; r < maplen; r += m_sidelen) {
        // find overlap
        start = getRowStart(r);
        end = m_mapdata[r].length();
        count = (end - start)/m_sidelen;
        // set side to the overlap side from previous row
        if (lastend > end) for (int i = 0; i < lastcount - 1; i++) side = leftOfSide(side);

        // now find the data for the overlap face on the new row
        origin = m_sides[side].origin;
        u = m_sides[side].eu;
        moveCoordAlongAxis(origin, v, m_sidelen - 1);

        // rotate v about the u axis
        v = rotateAxis90AboutAxis(m_sides[side].ev, u);

        // now ultimately get the side below the overlap side,
        // presaving vectors so we can rotate
        side = belowSide(side);
        m_sides[side].eu = u;
        m_sides[side].ev = v;


        // and finally, we need to back it up, Terry, to the beginning of the row
        if (lastend >= end) {
          for (int i = count - 1; i > 0; i--) {
            side = leftOfSide(side);
            u = cross[sidenormal[side]][v];
            m_sides[side].eu = u;
            m_sides[side].ev = v;
            moveCoordAlongAxis(origin, invax[u], m_sidelen - 1);
          }
        }

        for (int c = start; c < end; c += m_sidelen) {
          if (c != start) side = rightOfSide(side);
          u = cross[sidenormal[side]][v];
          m_sides[side].idx = side;
          m_sides[side].r0 = r;
          m_sides[side].c0 = c;
          m_sides[side].eu = u;
          m_sides[side].ev = v;
          m_sides[side].origin = origin;
          moveCoordAlongAxis(origin, u, m_sidelen - 1);
        }
        lastcount = count;
        laststart = start;
        lastend = end;
      }

      return firstrowstart;
    }

    AxisIdx rotateAxis90AboutAxis(AxisIdx in, AxisIdx about) {
      return cross[about][in];
    }

    AxisIdx inertialFacing(Facing dir, CubeSideIdx side) {
      switch (dir) {
        case faceright: return m_sides[side].eu;
        case facedown: return m_sides[side].ev;
        case faceleft: return invax[m_sides[side].eu];
        case faceup: return invax[m_sides[side].ev];
      }
      throw;
    }

    Facing uvFacing(AxisIdx dir, CubeSideIdx side) {
      if (dir == m_sides[side].eu) return faceright;
      if (dir == m_sides[side].ev) return facedown;
      if (dir == invax[m_sides[side].eu]) return faceleft;
      if (dir == invax[m_sides[side].ev]) return faceup;
      throw;
    }

    CubeSideIdx leftOfSide(CubeSideIdx side) {
      AxisIdx normal = rotateAxis90AboutAxis(sidenormal[side], m_sides[side].ev);
      for (int i = 0; i < 6; i++) if (normal == sidenormal[i]) return CubeSideIdx(i);
      throw;
    }

    CubeSideIdx rightOfSide(CubeSideIdx side) {
      AxisIdx normal = rotateAxis90AboutAxis(sidenormal[side], invax[m_sides[side].ev]);
      for (int i = 0; i < 6; i++) if (normal == sidenormal[i]) return CubeSideIdx(i);
      throw;
    }

    CubeSideIdx belowSide(CubeSideIdx side) {
      AxisIdx normal = rotateAxis90AboutAxis(sidenormal[side], m_sides[side].eu);
      for (int i = 0; i < 6; i++) if (normal == sidenormal[i]) return CubeSideIdx(i);
      throw;
    }

    CubeSideIdx aboveSide(CubeSideIdx side) {
      AxisIdx normal = rotateAxis90AboutAxis(sidenormal[side], invax[m_sides[side].eu]);
      for (int i = 0; i < 6; i++) if (normal == sidenormal[i]) return CubeSideIdx(i);
      throw;
    }

    char getUV(int u, int v, CubeSideIdx side) {
      int r, c;
      uv2rc(u, v, side, r, c);
      return m_mapdata[r][c];
    }

    void moveForward(int i, int &u, int &v, Facing &dir, CubeSideIdx &side) {
      bool onedge = false;
      while (i) {
        i--;

        // check edge
        switch (dir) {
          case faceleft: onedge = !u; break;
          case faceright: onedge = u == m_sidelen - 1; break;
          case faceup: onedge = !v; break;
          case facedown: onedge = v == m_sidelen - 1; break;
        }

        if (onedge) {
          // get the new side and the current normal
          CubeSideIdx newside;
          switch (dir) {
            case faceleft: newside = leftOfSide(side); break;
            case faceright: newside = rightOfSide(side); break;
            case faceup: newside = aboveSide(side); break;
            case facedown: newside = belowSide(side); break;
          }
          AxisIdx normal = sidenormal[side];
          int newu = u, newv = v;

          // get the xyz of our current point
          Coord tmp;
          uv2xyz(newu, newv, side, tmp);

          // convert to uv in the new side
          xyz2uv(tmp, newside, newu, newv);

          // check if it's a valid move
          if (getUV(newu, newv, newside) == '#') return;

          // continue adventure on new side

          // update facing
          AxisIdx newinrtlface, rotax;
          rotax = cross[normal][sidenormal[newside]];
          newinrtlface = rotateAxis90AboutAxis(inertialFacing(dir, side), rotax);
          dir = uvFacing(newinrtlface, newside);

          // update side
          side = newside;

          u = newu;
          v = newv;
        } else switch (dir) {
          case faceleft: if (getUV(u - 1, v, side) == '#') return; else u--; break;
          case faceright: if (getUV(u + 1, v, side) == '#') return; else u++; break;
          case faceup: if (getUV(u, v - 1, side) == '#') return; else v--; break;
          case facedown: if (getUV(u, v + 1, side) == '#') return; else v++; break;
        };
      }
    }
};
