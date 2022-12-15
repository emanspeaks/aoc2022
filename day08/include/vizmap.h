#pragma once

#include "main.h"

class VizMap {
  public:
    VizMap(Forest &f) {
      m_rows = f.size();
      m_cols = f[0].length();
      m_map = new bool*[m_rows];
      for (int i = 0; i < m_rows; i++) {
        m_map[i] = new bool[m_cols];
        for (int j = 0; j < m_cols; j++) m_map[i][j] = false;
      }
      calcNorthViz(f);
      calcEastViz(f);
      calcSouthViz(f);
      calcWestViz(f);
    }
    ~VizMap() {
      for (int i = 0; i < m_rows; i++) delete [] m_map[i];
      delete [] m_map;
    }

    int rows() const {return m_rows;}
    int cols() const {return m_cols;}

    int sum() {
      int sum = 0;
      for (int r = 0; r < m_rows; r++) for (int c = 0; c < m_cols; c++) sum += m_map[r][c];
      return sum;
    }

  private:
    int m_rows;
    int m_cols;
    bool** m_map;

    void calcNorthViz(const Forest &f) {
      char* last = new char[m_cols];

      for (int c = 0; c < m_cols; c++) {
        m_map[0][c] = true;
        last[c] = f[0][c];
      }

      for (int r = 1; r < m_rows; r++) {
        for (int c = 0; c < m_cols; c++) {
          if (f[r][c] > last[c]) {
            last[c] = f[r][c];
            m_map[r][c] = true;
          }
        }
      }

      delete [] last;
    }

    void calcSouthViz(const Forest &f) {
      int rm1 = m_rows - 1;
      char* last = new char[m_cols];

      for (int c = 0; c < m_cols; c++) {
        m_map[rm1][c] = true;
        last[c] = f[rm1][c];
      }

      for (int r = rm1 - 1; r > 0; r--) {
        for (int c = 0; c < m_cols; c++) {
          if (f[r][c] > last[c]) {
            last[c] = f[r][c];
            m_map[r][c] = true;
          }
        }
      }

      delete [] last;
    }

    void calcEastViz(const Forest &f) {
      int cm1 = m_cols - 1;
      char* last = new char[m_rows];

      for (int r = 0; r < m_rows; r++) {
        m_map[r][cm1] = true;
        last[r] = f[r][cm1];
      }

      for (int c = cm1 - 1; c > 0; c--) {
        for (int r = 0; r < m_rows; r++) {
          if (f[r][c] > last[r]) {
            last[r] = f[r][c];
            m_map[r][c] = true;
          }
        }
      }

      delete [] last;
    }

    void calcWestViz(const Forest &f) {
      char* last = new char[m_rows];

      for (int r = 0; r < m_rows; r++) {
        m_map[r][0] = true;
        last[r] = f[r][0];
      }

      for (int c = 1; c < m_cols; c++) {
        for (int r = 0; r < m_rows; r++) {
          if (f[r][c] > last[r]) {
            last[r] = f[r][c];
            m_map[r][c] = true;
          }
        }
      }

      delete [] last;
    }
};
