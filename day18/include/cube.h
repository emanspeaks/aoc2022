#pragma once

#include <string>
#include <deque>
#include <sstream>
#include <memory>
#include <unordered_set>
#include <unordered_map>
#include <array>

enum Face {mx = 1, px = 2, my = 4, py = 8, mz = 16, pz = 32, present = 64};
enum FaceIdx {mxi, pxi, myi, pyi, mzi, pzi};

const char facelist[6] = {mx, px, my, py, mz, pz};

typedef std::array<int, 3> Coord;
typedef std::array<int, 4> FaceDef;
typedef std::array<std::string, 4> EdgeList;
typedef std::deque<char> ZData;
typedef ZData* YPtr;
typedef std::deque<std::unique_ptr<ZData>> YData;
typedef YData* XPtr;
typedef std::deque<std::unique_ptr<YData>> XData;

class CubeContainer {
  public:
    CubeContainer(): m_mx(0), m_my(0), m_mz(0), m_px(-1), m_py(-1), m_pz(-1) {}

    void parseAndInsert(const std::string &s) {
      std::stringstream sstm(s);
      std::string tmp;
      Coord coords;
      int i = 0;
      while (std::getline(sstm, tmp, ',')) coords[i++] = std::stoi(tmp);

      addFacesAndEdges(coords);
      checkExtents(coords);
      set(coords[0], coords[1], coords[2], 0b1111111);
      checkNeighbors(coords[0], coords[1], coords[2]);
    }

    int countFaces() {
      int sum = 0;
      char me;
      for (int x = m_mx; x <= m_px; x++) {
        for (int y = m_my; y <= m_py; y++) {
          for (int z = m_mz; z <= m_pz; z++) {
            me = get(x, y, z);
            for (int i = 0; i < 6; i++) sum += (me & (1 << i)) > 0;
          }
        }
      }
      return sum;
    }

    int countExteriorFaces() {
      int startx, starty, startz;
      char startcube;

      // get first face; it will be an mz face because of approach dir
      bool found = false;
      for (int z = m_mz; z <= m_pz; z++) {
        startz = z;
        for (int y = m_my; y <= m_py; y++) {
          starty = y;
          for (int x = m_mx; x <= m_px; x++) {
            startx = x;
            startcube = get(x, y, z);
            if (startcube) {
              found = true;
              break;
            }
          }
          if (found) break;
        }
        if (found) break;
      }

      std::unordered_set<std::string> counted;
      std::unordered_set<std::string> neighbors;
      neighbors.insert(faceName({startx, starty, startz}, mz));

      std::string herename, priority[3], selected;
      EdgeList myedges;
      FaceDef tmpface, here;
      std::vector<std::string> tmpfacelist;
      while (neighbors.size()) {
        herename = *neighbors.begin();

        // update sets
        neighbors.erase(herename);
        counted.insert(herename);

        here = m_facemap[herename];
        myedges = m_face2edge[herename];
        for (auto edge: myedges) {
          tmpfacelist.clear();
          for (auto fname: m_edgemap[edge]) {
            if (fname == herename) continue;
            if (m_surface.find(fname) != m_surface.end()) tmpfacelist.push_back(fname);
          }
          if (tmpfacelist.size() == 1) selected = tmpfacelist[0];
          else {
            for (int i = 0; i < 3; i++) priority[i] = "";
            for (auto fname: tmpfacelist) {
              tmpface = m_facemap[fname];
              priority[facePriority(here, tmpface)] = fname;
            }
            for (int i = 0; i < 3; i++) if (priority[i] != "") {
              selected = priority[i];
              break;
            }
          }
          if (counted.find(selected) == counted.end()) neighbors.insert(selected);
        }
      }

      return counted.size();
    }

  private:
    XData m_faces;
    int m_mx, m_my, m_mz, m_px, m_py, m_pz;
    std::unordered_map<std::string, FaceDef> m_facemap;
    std::unordered_map<std::string, std::vector<std::string>> m_edgemap;
    std::unordered_map<std::string, EdgeList> m_face2edge;
    std::unordered_set<std::string> m_surface;

    void addPX() {m_faces.push_back(std::make_unique<YData>());}
    void addMX() {m_faces.push_front(std::make_unique<YData>());}

    void addPY() {for (auto &x: m_faces) addPY(x.get());}
    void addPY(XPtr x) {
      x->push_back(std::make_unique<ZData>());
    }

    void addMY() {for (auto &x: m_faces) addMY(x.get());}
    void addMY(XPtr x) {
      x->push_front(std::make_unique<ZData>());
    }

    void addPZ() {for (auto &x: m_faces) addPZ(x.get());}
    void addPZ(XPtr x) {
      for (auto &y: *x) addPZ(y.get());
    }
    void addPZ(std::deque<char>* y) {
      y->push_back(0);
    }

    void addMZ() {for (auto &x: m_faces) addMZ(x.get());}
    void addMZ(XPtr x) {
      for (auto &y: *x) addMZ(y.get());
    }
    void addMZ(YPtr y) {
      y->push_front(0);
    }

    void addAndFillPX() {
      addPX();
      XPtr pxp = m_faces.back().get();
      for (int i = m_py - m_my + 1; i > 0; i--) addAndFillPY(pxp);
    }

    void addAndFillMX() {
      addMX();
      XPtr mxp = m_faces.front().get();
      for (int i = m_py - m_my + 1; i > 0; i--) addAndFillPY(mxp);
    }

    void addAndFillPY() {for (auto &x: m_faces) addAndFillPY(x.get());}
    void addAndFillPY(XPtr x) {
      addPY(x);
      YPtr pyp = x->back().get();
      for (int i = m_pz - m_mz + 1; i > 0; i--) addPZ(pyp);
    }

    void addAndFillMY() {for (auto &x: m_faces) addAndFillMY(x.get());}
    void addAndFillMY(XPtr x) {
      addMY(x);
      YPtr myp = x->front().get();
      for (int i = m_pz - m_mz + 1; i > 0; i--) addPZ(myp);
    }

    void set(int x, int y, int z, char v) {
      m_faces[x - m_mx].get()->at(y - m_my).get()->at(z - m_mz) = v;
    }

    char get(int x, int y, int z) {
      return m_faces[x - m_mx].get()->at(y - m_my).get()->at(z - m_mz);
    }

    void checkExtents(const Coord coords) {
      if (m_mx > m_px) {
        addPX();
        m_mx = m_px = coords[0];
      }
      if (m_my > m_py) {
        addPY();
        m_my = m_py = coords[1];
      }
      if (m_mz > m_pz) {
        addPZ();
        m_mz = m_pz = coords[2];
      }

      if (coords[0] > m_px) {
        for (int i = coords[0] - m_px; i > 0; i--) addAndFillPX();
        m_px = coords[0];
      } else if (coords[0] < m_mx) {
        for (int i = m_mx - coords[0]; i > 0; i--) addAndFillMX();
        m_mx = coords[0];
      }

      if (coords[1] > m_py) {
        for (int i = coords[1] - m_py; i > 0; i--) addAndFillPY();
        m_py = coords[1];
      } else if (coords[1] < m_my) {
        for (int i = m_my - coords[1]; i > 0; i--) addAndFillMY();
        m_my = coords[1];
      }

      if (coords[2] > m_pz) {
        for (int i = coords[2] - m_pz; i > 0; i--) addPZ();
        m_pz = coords[2];
      } else if (coords[2] < m_mz) {
        for (int i = m_mz - coords[2]; i > 0; i--) addMZ();
        m_mz = coords[2];
      }
    }

    void checkNeighbors(int x, int y, int z) {
      char me = get(x, y, z), them;

      if (x < m_px) {
        them = get(x + 1, y, z);
        if (them) {
          me &= ~px;
          set(x + 1, y, z, them & (~mx));
          m_surface.erase(faceName({x, y, z}, px));
          m_surface.erase(faceName({x + 1, y, z}, mx));
        }
      }

      if (x > m_mx) {
        them = get(x - 1, y, z);
        if (them) {
          me &= ~mx;
          set(x - 1, y, z, them & (~px));
          m_surface.erase(faceName({x, y, z}, mx));
          m_surface.erase(faceName({x - 1, y, z}, px));
        }
      }

      if (y < m_py) {
        them = get(x, y + 1, z);
        if (them) {
          me &= ~py;
          set(x, y + 1, z, them & (~my));
          m_surface.erase(faceName({x, y, z}, py));
          m_surface.erase(faceName({x, y + 1, z}, my));
        }
      }

      if (y > m_my) {
        them = get(x, y - 1, z);
        if (them) {
          me &= ~my;
          set(x, y - 1, z, them & (~py));
          m_surface.erase(faceName({x, y, z}, my));
          m_surface.erase(faceName({x, y - 1, z}, py));
        }
      }

      if (z < m_pz) {
        them = get(x, y, z + 1);
        if (them) {
          me &= ~pz;
          set(x, y, z + 1, them & (~mz));
          m_surface.erase(faceName({x, y, z}, pz));
          m_surface.erase(faceName({x, y, z + 1}, mz));
        }
      }

      if (z > m_mz) {
        them = get(x, y, z - 1);
        if (them) {
          me &= ~mz;
          set(x, y, z - 1, them & (~pz));
          m_surface.erase(faceName({x, y, z}, mz));
          m_surface.erase(faceName({x, y, z - 1}, pz));
        }
      }

      set(x, y, z, me);
    }

    std::string cubeName(const Coord coords) {
      return std::to_string(coords[0]) + ','
        + std::to_string(coords[1]) + ','
        + std::to_string(coords[2]);
    }

    std::string faceName(const Coord coords, int face) {
      return cubeName(coords) + ',' + std::to_string(face);
    }

    void addFacesAndEdges(const Coord coords) {
      // faces are named by the direction of their normals out of the cube.
      // edges are always positive along the axis to which they are parallel.

      std::string mxs = faceName(coords, mx);
      std::string pxs = faceName(coords, px);
      std::string mys = faceName(coords, my);
      std::string pys = faceName(coords, py);
      std::string mzs = faceName(coords, mz);
      std::string pzs = faceName(coords, pz);

      m_surface.insert(mxs);
      m_surface.insert(pxs);
      m_surface.insert(mys);
      m_surface.insert(pys);
      m_surface.insert(mzs);
      m_surface.insert(pzs);

      m_facemap[mxs] = {coords[0], coords[1], coords[2], mx};
      m_face2edge[mxs] = {
        faceName({coords[0], coords[1], coords[2]}, py),
        faceName({coords[0], coords[1], coords[2]}, pz),
        faceName({coords[0], coords[1], coords[2]+1}, py),
        faceName({coords[0], coords[1]+1, coords[2]}, pz)
      };
      for (int i = 0; i < 4; i++) m_edgemap[m_face2edge[mxs][i]].push_back(mxs);

      m_facemap[pxs] = {coords[0], coords[1], coords[2], px};
      m_face2edge[pxs] = {
        faceName({coords[0]+1, coords[1], coords[2]}, py),
        faceName({coords[0]+1, coords[1], coords[2]}, pz),
        faceName({coords[0]+1, coords[1], coords[2]+1}, py),
        faceName({coords[0]+1, coords[1]+1, coords[2]}, pz)
      };
      for (int i = 0; i < 4; i++) m_edgemap[m_face2edge[pxs][i]].push_back(pxs);

      m_facemap[mys] = {coords[0], coords[1], coords[2], my};
      m_face2edge[mys] = {
        faceName({coords[0], coords[1], coords[2]}, px),
        faceName({coords[0], coords[1], coords[2]}, pz),
        faceName({coords[0], coords[1], coords[2]+1}, px),
        faceName({coords[0]+1, coords[1], coords[2]}, pz)
      };
      for (int i = 0; i < 4; i++) m_edgemap[m_face2edge[mys][i]].push_back(mys);

      m_facemap[pys] = {coords[0], coords[1], coords[2], py};
      m_face2edge[pys] = {
        faceName({coords[0], coords[1]+1, coords[2]}, px),
        faceName({coords[0], coords[1]+1, coords[2]}, pz),
        faceName({coords[0], coords[1]+1, coords[2]+1}, px),
        faceName({coords[0]+1, coords[1]+1, coords[2]}, pz)
      };
      for (int i = 0; i < 4; i++) m_edgemap[m_face2edge[pys][i]].push_back(pys);

      m_facemap[mzs] = {coords[0], coords[1], coords[2], mz};
      m_face2edge[mzs] = {
        faceName({coords[0], coords[1], coords[2]}, px),
        faceName({coords[0], coords[1], coords[2]}, py),
        faceName({coords[0], coords[1]+1, coords[2]}, px),
        faceName({coords[0]+1, coords[1], coords[2]}, py)
      };
      for (int i = 0; i < 4; i++) m_edgemap[m_face2edge[mzs][i]].push_back(mzs);

      m_facemap[pzs] = {coords[0], coords[1], coords[2], pz};
      m_face2edge[pzs] = {
        faceName({coords[0], coords[1], coords[2]+1}, px),
        faceName({coords[0], coords[1], coords[2]+1}, py),
        faceName({coords[0], coords[1]+1, coords[2]+1}, px),
        faceName({coords[0]+1, coords[1], coords[2]+1}, py)
      };
      for (int i = 0; i < 4; i++) m_edgemap[m_face2edge[pzs][i]].push_back(pzs);
    }

    int facePriority(FaceDef here, FaceDef f) {
      // look for faces in the dir of here normal first, then same plane, then negative.

      if (here[0] == f[0] && here[1] == f[1] && here[2] == f[2]) return 2;

      switch (here[3])
      {
        case mx:
          if (f[3] == px) return 1;
          break;

        case px:
          if (f[3] == mx) return 1;
          break;

        case my:
          if (f[3] == py) return 1;
          break;

        case py:
          if (f[3] == my) return 1;
          break;

        case mz:
          if (f[3] == pz) return 1;
          break;

        case pz:
          if (f[3] == mz) return 1;
      }
      return here[3] == f[3];
    }
};
