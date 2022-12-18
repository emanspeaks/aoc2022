#pragma once

#include <vector>
#include <string>
#include <deque>

// ####

// .#.
// ###
// .#.

// ..#
// ..#
// ###

// #
// #
// #
// #

// ##
// ##

struct ShapeDef {
  int width;
  int height;
  uint16_t body; // bottom to top, left to right
};

#define SHAPECOUNT 5
const ShapeDef bar {4, 1, 0b1111};
const ShapeDef plus {3, 3, 0b010111010};
const ShapeDef ell {3, 3, 0b111001001};
const ShapeDef pipe {1, 4, 0b1111};
const ShapeDef sq {2, 2, 0b1111};
const ShapeDef shapelist[SHAPECOUNT] = {bar, plus, ell, pipe, sq};

#define ROOMWIDTH 7
#define LASTCOL (ROOMWIDTH - 1)

class Rock {
  public:
    Rock(ShapeDef sd): m_def(sd), m_rest(false) {}

    bool isAtRest() {return m_rest;}
    void setXY(int x, long long y) {m_x = x; m_y = y;}
    int height() {return m_def.height;}
    int width() {return m_def.width;}

    void tryWind(char wind, std::deque<char> &rows, long long bottomrow = 0) {
      //  0123456
      // |.###...|
      //  6543210
      int newx = m_x, newright = m_x + m_def.width - 1;
      int offset = (wind == '>')? 1 : -1;
      newx += offset;
      newright += offset;
      if (newright > LASTCOL) return;
      if (newx < 0) return;

      // try "drawing" and see if it overlaps anything
      char drawrow;
      uint16_t rowmask = 0;
      // 876543210
      // 010111010
      for (int x = 0; x < m_def.width; x++) rowmask |= 1 << (x + (m_def.height - 1)*m_def.width);
      for (int y = 0; y < m_def.height; y++) {
        drawrow = ((m_def.body & rowmask) >> ((m_def.height - 1 - y)*m_def.width)) << (LASTCOL - newright);
        if (rows[m_y - bottomrow + y] & drawrow) return;
        rowmask >>= m_def.width;
      }

      // we made it this far, make it permanent
      m_x = newx;
    }

    void tryDrop(std::deque<char> &rows, long long bottomrow = 0) {
      long long newy = m_y - 1;
      if (newy < 0) {setRest(rows, bottomrow); return;}

      // try "drawing" and see if it overlaps anything
      char drawrow;
      uint16_t rowmask = 0;
      // 876543210
      // 010111010
      for (int x = 0; x < m_def.width; x++) rowmask |= 1 << (x + (m_def.height - 1)*m_def.width);
      for (int y = 0; y < m_def.height; y++) {
        drawrow = ((m_def.body & rowmask) >> ((m_def.height - 1 - y)*m_def.width)) << (ROOMWIDTH - m_x - m_def.width);
        if (rows[newy - bottomrow + y] & drawrow) {setRest(rows, bottomrow); return;}
        rowmask >>= m_def.width;
      }

      // we made it this far, make it permanent
      m_y = newy;
    }

  private:
    ShapeDef m_def;
    bool m_rest;
    int m_x;
    long long m_y;

    void setRest(std::deque<char> &rows, long long bottomrow) {
      m_rest = true;

      char drawrow;
      uint16_t rowmask = 0;
      // 876543210
      // 010111010
      for (int x = 0; x < m_def.width; x++) rowmask |= 1 << (x + (m_def.height - 1)*m_def.width);
      for (int y = 0; y < m_def.height; y++) {
        drawrow = ((m_def.body & rowmask) >> ((m_def.height - 1 - y)*m_def.width)) << (ROOMWIDTH - m_x - m_def.width);
        rows[m_y - bottomrow + y] |= drawrow;
        rowmask >>= m_def.width;
      }
    }

};
