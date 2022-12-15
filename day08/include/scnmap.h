#pragma once

#include "main.h"

class ScenicMap {
  public:
    ScenicMap(Forest &f) {
      m_rows = f.size();
      m_cols = f[0].length();
      m_map = new int*[m_rows];
      for (int i = 0; i < m_rows; i++) {
        m_map[i] = new int[m_cols];
        for (int j = 0; j < m_cols; j++) m_map[i][j] = 1;
      }
      calcSouthViz(f);
      calcNorthViz(f);
      calcEastViz(f);
      calcWestViz(f);
    }
    ~ScenicMap() {
      for (int i = 0; i < m_rows; i++) delete [] m_map[i];
      delete [] m_map;
    }

    int rows() const {return m_rows;}
    int cols() const {return m_cols;}

    int max() {
      int x = 0;
      for (int i = 1; i < m_rows; i++) for (int j = 1; j < m_cols; j++)
        if (m_map[i][j] > x) x = m_map[i][j];
      return x;
    }

  private:
    int m_rows;
    int m_cols;
    int** m_map;

    void calcSouthViz(const Forest &f) {
      int rm1 = m_rows - 1, cm1 = m_cols - 1;
      for (int c = 0; c < m_cols; c++) m_map[0][c] = 0;

      char me;
      int i;
      for (int r = 1; r < rm1; r++) {
        for (int c = 1; c < cm1; c++) {
          me = f[r][c];
          i = 1;
          while ((r + i) < rm1 && f[r + i][c] < me) i++;
          m_map[r][c] *= i;
        }
      }
    }

    void calcNorthViz(const Forest &f) {
      int rm1 = m_rows - 1, cm1 = m_cols - 1;
      for (int c = 0; c < m_cols; c++) m_map[rm1][c] = 0;

      char me;
      char i;
      for (int r = rm1 - 1; r > 0; r--) {
        for (int c = 1; c < cm1; c++) {
          me = f[r][c];
          i = 1;
          while ((r - i) > 0 && f[r - i][c] < me) i++;
          m_map[r][c] *= i;
        }
      }
    }

    void calcWestViz(const Forest &f) {
      int rm1 = m_rows - 1, cm1 = m_cols - 1;
      for (int r = 0; r < m_rows; r++) m_map[r][cm1] = 0;

      char me;
      int i;
      for (int c = cm1 - 1; c > 0; c--) {
        for (int r = 1; r < rm1; r++) {
          me = f[r][c];
          i = 1;
          while ((c - i) > 0 && f[r][c - i] < me) i++;
          m_map[r][c] *= i;
        }
      }
    }

    void calcEastViz(const Forest &f) {
      int rm1 = m_rows - 1, cm1 = m_cols - 1;
      for (int r = 0; r < m_rows; r++) m_map[r][0] = 0;

      char me;
      int i;
      for (int c = 1; c < cm1; c++) {
        for (int r = 1; r < rm1; r++) {
          me = f[r][c];
          i = 1;
          while ((c + i) < cm1 && f[r][c + i] < me) i++;
          m_map[r][c] *= i;
        }
      }
    }
};
