#include "Mesh.h"
#include "qtextstream.h"
#include <set>
#include <map>
#include <iostream>

Mesh::Mesh() :
    m_dirty(false),
    m_material()
{
    m_material.roughness = 0.1;
    m_material.cAmbient = CS123SceneColor(1, 1, 1, 0);
    m_material.cDiffuse = CS123SceneColor(1, 1, 1, 0);
    m_material.cSpecular = CS123SceneColor(1, 1, 1, 0);
}

Mesh::Mesh(const std::string& filename) {
    QFile file{QString::fromStdString(filename)};
    parseObj(file);
    genHE();
    genNormals();
    genglData();
    m_dirty = false;
}

Mesh::Mesh(QFile& file) : Mesh()
{
    parseObj(file);
    genHE();
    genNormals();
    genglData();
    m_dirty = false;
}

void Mesh::parseObj(QFile& file) {
    m_vertData.clear();
    m_faceData.clear();
    m_edgeData.clear();
    m_heData.clear();
    if (file.open(QIODevice::ReadOnly))
    {
       QTextStream in(&file);
       std::size_t norm_i = 0;
       std::size_t vert_i = 0;
       std::size_t face_i = 0;
       while (!in.atEnd()) {
          QString line = in.readLine();
          QString str = line.split("#")[0];
          if (str.isEmpty())
              continue;
          QStringList terms = str.split(" ");
          terms.removeAll("");
          terms.removeAll(" ");
          if (terms.length() < 3)
              continue;

          if (terms[0][0] == 'v') {
              if (terms[0].length() == 1) {
                  if (vert_i >= norm_i)
                      m_vertData.push_back(Vertex());
                  m_vertData[vert_i].m_pos = glm::vec3{
                          terms[1].toFloat(),
                          terms[2].toFloat(),
                          terms[3].toFloat()
                  };
                  vert_i++;
              } else {
                  switch (terms[0][1].digitValue()) {
                    case 't':
                          break;
                    case 'n':
                          break;
                    default:
                          break;
                  }
              }
          } else if (terms[0][0] == 'f') {
            QStringList termsv;
            for (auto s : terms) {
                termsv.append(s.split("/")[0]);
            }
            m_faceData.push_back(Face());
            m_faceData[face_i].a = termsv[1].toUInt() - 1;
            m_faceData[face_i].b = termsv[2].toUInt() - 1;
            m_faceData[face_i].c = termsv[3].toUInt() - 1;
            face_i++;
            if (terms.length() > 4) {
                for (int i = 4; i < terms.length(); i++) {
                    m_faceData.push_back(Face());
                    m_faceData[face_i].a = termsv[1].toUInt() - 1;
                    m_faceData[face_i].b = termsv[i-1].toUInt() - 1;
                    m_faceData[face_i].c = termsv[i].toUInt() - 1;
                    face_i++;
                }
            }
          }
       }

       m_dirty = true;
       file.close();
    }
}

void Mesh::genHE() {
    auto emapp = new std::map<std::pair<int, int>, std::size_t>();
    std::map<std::pair<int, int>, std::size_t>& emap = *emapp;
    m_heData.clear();
    m_heData.reserve(m_faceData.size() * 4 + 5);
    m_edgeData.clear();
    m_edgeData.reserve(m_vertData.size() + m_faceData.size() - 2);

    //Build halfedges
    for (std::size_t i = 0; i < m_faceData.size(); i++) {
        Face* f = m_faceData.data() + i;
        m_heData.push_back(HalfEdge());
        m_heData.push_back(HalfEdge());
        m_heData.push_back(HalfEdge());

        f->m_he = i * 3;

        if (m_vertData[f->a].m_he == SIZE_MAX)
            m_vertData[f->a].m_he = i * 3;
        if (m_vertData[f->b].m_he == SIZE_MAX)
            m_vertData[f->b].m_he = i * 3 + 1;
        if (m_vertData[f->c].m_he == SIZE_MAX)
            m_vertData[f->c].m_he = i * 3 + 2;

        m_heData[i * 3].m_face = i;
        m_heData[i * 3].m_next = i * 3 + 1;
        m_heData[i * 3].m_nvert = f->b;

        m_heData[i * 3 + 1].m_face = i;
        m_heData[i * 3 + 1].m_next = i * 3 + 2;
        m_heData[i * 3 + 1].m_nvert = f->c;

        m_heData[i * 3 + 2].m_face = i;
        m_heData[i * 3 + 2].m_next = i * 3;
        m_heData[i * 3 + 2].m_nvert = f->a;

        emap[std::pair<int, int>{f->a, f->b}] = i * 3;
        emap[std::pair<int, int>{f->b, f->c}] = i * 3 + 1;
        emap[std::pair<int, int>{f->c, f->a}] = i * 3 + 2;
    }

    //Link Halfedges and build Edges
    for (auto const& k : emap) {
        auto rev = std::pair<int, int>{k.first.second, k.first.first};
        if (emap.count(rev)) {
            auto lptr = m_heData.data() + k.second;
            auto rptr = m_heData.data() + emap[rev];
            if ((lptr->m_twin == SIZE_MAX) && (rptr->m_twin == SIZE_MAX)) {
                lptr->m_twin = emap[rev];
                rptr->m_twin = k.second;
                Edge tmp = Edge();
                tmp.m_he = k.second;
                m_edgeData.push_back(tmp);
                lptr->m_edge = m_edgeData.size() - 1;
                rptr->m_edge = m_edgeData.size() - 1;
            }
        }
    }

    //Handle boundaries
    std::size_t sz = m_heData.size();
    for (std::size_t i = 0; i < sz; i++) {
        if (m_heData[i].m_twin == SIZE_MAX) {
            m_heData.push_back(HalfEdge());
            m_heData[i].m_twin = m_heData.size() - 1;
            auto heptr = m_heData.data() + m_heData[i].m_twin;
            heptr->m_twin = i;
            heptr->m_face = SIZE_MAX;
            heptr->m_nvert = m_heData[i].next(*this)->next(*this)->m_nvert;
            heptr->m_next = SIZE_MAX;
            (m_vertData.data() + m_heData[i].m_nvert)->m_he = m_heData[i].m_twin;
        }
    }

    delete emapp;
}

void Mesh::genglData() {
    m_glData.clear();
    m_glData.emplace_back(std::make_unique<PolySoup>());
    for (auto f : m_faceData) {
        auto v1 = m_vertData[f.a];
        auto v2 = m_vertData[f.b];
        auto v3 = m_vertData[f.c];

        m_glData[0]->addVert(v1.m_pos, v1.m_norm);
        m_glData[0]->addVert(v2.m_pos, v2.m_norm);
        m_glData[0]->addVert(v3.m_pos, v3.m_norm);
    }
    m_glData[0]->refresh();
    m_glData[0]->transform(m_world);
}

void Mesh::genNormals() {
    for (std::size_t i = 0; i < m_vertData.size(); i++) {
        Vertex* v = m_vertData.data() + i;
        glm::vec3 pos = v->m_pos;
        glm::vec3 norm = glm::vec3{0, 0, 0};
        HalfEdge* cur = v->he(*this);
        //degenerate vertex
        if (v->m_he == SIZE_MAX)
            continue;
        //degenerate line
        if (cur->twin(*this)->m_next == SIZE_MAX)
            continue;
        Vertex* tick = cur->nvert(*this);
        cur = cur->orbit(*this);
        Vertex* tock = cur->nvert(*this);
        norm -= glm::cross(tick->m_pos - pos, tock->m_pos - pos);
        while (cur != v->he(*this) && cur->twin(*this)->m_next != SIZE_MAX) {
            tick = tock;
            cur = cur->orbit(*this);
            tock = cur->nvert(*this);
            norm -= glm::cross(tick->m_pos - pos, tock->m_pos - pos);
        }
        norm = glm::normalize(norm);
        v->m_norm = norm;
    }
}

void Mesh::draw() {
    if (m_dirty) {
        genNormals();
        genglData();
        m_dirty = false;
    }

    for (std::size_t i = 0; i < m_glData.size(); i++)
        m_glData[i]->draw();
}

void Mesh::draw(CS123::GL::CS123Shader* shader) {
    shader->applyMaterial(m_material);

    if (m_dirty) {
        genNormals();
        genglData();
        m_dirty = false;
    }

    for (std::size_t i = 0; i < m_glData.size(); i++)
        m_glData[i]->draw();
}

std::size_t Mesh::faceSplit(std::size_t f) {
    Face f1 = m_faceData[f];
    HalfEdge he1 = *(f1.he(*this));
    HalfEdge he2 = *(he1.next(*this));
    HalfEdge he3 = *(he2.next(*this));

    //New vertex
    Vertex _vd = Vertex();
    _vd.m_pos = he1.nvert(*this)->m_pos + 0.5f * (he3.nvert(*this)->m_pos - he1.nvert(*this)->m_pos);
    _vd.m_he = m_heData.size();
    m_vertData.push_back(_vd);

    return faceSplit(f, m_vertData.size() - 1);
}

std::size_t Mesh::faceSplit(std::size_t f, std::size_t v) {
    Face f1 = m_faceData[f];
    Face f2 = Face();
    HalfEdge he1 = *(f1.he(*this));
    HalfEdge he2 = *(he1.next(*this));
    HalfEdge he3 = *(he2.next(*this));
    HalfEdge nhe = HalfEdge();
    Edge ne = Edge();
    HalfEdge nhef1 = HalfEdge();
    HalfEdge nhef2 = HalfEdge();
    std::size_t va = he1.m_nvert;
    std::size_t vb = he2.m_nvert;
    std::size_t vc = he3.m_nvert;
    std::size_t vd = v;

    //Operation order crucial
    //New halfedge in split edge
    nhe.m_nvert = va;
    nhe.m_face = m_faceData.size();
    nhe.m_next = he1.m_next;
    m_heData.push_back(nhe);

    //New edge
    ne.m_he = m_heData.size();
    ne.is_new = true;
    m_edgeData.push_back(ne);

    //First halfedge for new edge
    nhef1.m_edge = m_edgeData.size() - 1;
    nhef1.m_face = he1.m_face;
    nhef1.m_next = he2.m_next;
    nhef1.m_nvert = vb;
    nhef1.m_twin = m_heData.size() + 1;
    m_heData.push_back(nhef1);

    //Second halfedge for new edge
    nhef2.m_edge = m_edgeData.size() - 1;
    nhef2.m_face = m_faceData.size();
    nhef2.m_next = m_heData.size() - 2;
    nhef2.m_nvert = vd;
    nhef2.m_twin = m_heData.size() - 1;
    m_heData.push_back(nhef2);

    //New face
    f2.a = vd;
    f2.b = va;
    f2.c = vb;
    f2.m_he = m_heData.size() - 1;
    m_faceData.push_back(f2);

    //Update base halfedge
    he1.m_next = m_heData.size() - 2;
    he1.m_nvert = vd;
    m_heData[f1.m_he] = he1;

    //Update old face
    m_faceData[f].a = vd;
    m_faceData[f].b = vb;
    m_faceData[f].c = vc;

    //Update old halfedges
    he2.m_next = m_heData.size() - 1;
    he2.m_face = m_faceData.size() - 1;
    m_heData[nhe.m_next] = he2;

    return m_heData.size() - 3;
}

void Mesh::edgeSplit(std::size_t edge) {
    if (edge >= m_edgeData.size()) {
        return;
    }

    Edge ed = m_edgeData[edge];
    Edge ned = Edge();
    if (ed.he(*this)->m_face == SIZE_MAX) {
        ed.m_he = ed.he(*this)->m_twin;
        //Degenerate face
        if (ed.he(*this)->m_face == SIZE_MAX)
            return;
    }
    HalfEdge he1 = *ed.he(*this);
    HalfEdge he2 = *he1.twin(*this);

    ned.m_he = he1.m_twin;
    m_edgeData.push_back(ned);

    he1.face(*this)->m_he = ed.m_he;
    he2.m_edge = m_edgeData.size() - 1;
    he2.m_twin = faceSplit(he1.m_face);
    he2.twin(*this)->m_twin = he1.m_twin;
    he2.twin(*this)->m_edge = he2.m_edge;

    if (he2.m_face != SIZE_MAX) {
        he2.face(*this)->m_he = he1.m_twin;
        he1.m_twin = faceSplit(he2.m_face, m_vertData.size() - 1);
        he1.twin(*this)->m_twin = ed.m_he;
        he1.twin(*this)->m_edge = he1.m_edge;
    } else {
        HalfEdge nt = HalfEdge();
        nt.m_face = SIZE_MAX;
        nt.m_edge = he1.m_edge;
        nt.m_next = SIZE_MAX;
        nt.m_nvert = he2.m_nvert;
        nt.m_twin = ed.m_he;
        he1.m_twin = m_heData.size();
        he2.m_nvert = m_vertData.size() - 1;
        m_heData[ned.m_he].m_nvert = he2.m_nvert;
        m_heData.push_back(nt);
        m_vertData[m_vertData.size() - 1].m_he = m_heData.size() - 1;
    }

    //Update the halfedges
    m_heData[ed.m_he].m_twin = he1.m_twin;
    m_heData[ned.m_he].m_twin = he2.m_twin;
    m_heData[ned.m_he].m_edge = he2.m_edge;

    m_dirty = true;
}

void Mesh::edgeFlip(std::size_t edge) {
    if (edge >= m_edgeData.size())
        return;

    Edge ed = m_edgeData[edge];
    HalfEdge he1 = m_heData[ed.m_he];
    HalfEdge he2 = m_heData[he1.m_twin];
    std::size_t f1va = he1.m_nvert;
    std::size_t f1vb = he1.next(*this)->m_nvert;
    std::size_t f1vc = he1.next(*this)->next(*this)->m_nvert;
    std::size_t f2vb = he2.next(*this)->m_nvert;

    if (he1.m_face == SIZE_MAX || he2.m_face == SIZE_MAX)
        return;

    Face f1 = m_faceData[he1.m_face];
    Face f2 = m_faceData[he2.m_face];

    //Update faces
    f1.a = f1vb;
    f1.b = f1vc;
    f1.c = f2vb;
    f1.m_he = he2.m_twin;

    f2.a = f2vb;
    f2.b = f1va;
    f2.c = f1vb;
    f2.m_he = he1.m_twin;

    //Update vertices
    if (he2.nvert(*this)->m_he == he2.m_twin) {
        he2.nvert(*this)->m_he = he2.m_next;
    }
    if (he1.nvert(*this)->m_he == he1.m_twin) {
        he1.nvert(*this)->m_he = he1.m_next;
    }

    //Update halfedges
    auto tmp1 = he1.next(*this);
    auto tmp2 = he2.next(*this);

    tmp1->next(*this)->m_next = he2.m_next;
    tmp2->next(*this)->m_next = he1.m_next;

    he1.m_nvert = tmp1->m_nvert;
    he1.m_next = tmp1->m_next;
    he2.m_nvert = tmp2->m_nvert;
    he2.m_next = tmp2->m_next;

    tmp1->m_next = he1.m_twin;
    tmp1->m_face = he2.m_face;
    tmp2->m_next = he2.m_twin;
    tmp2->m_face = he1.m_face;


    //Commit
    m_faceData[he1.m_face] = f1;
    m_faceData[he2.m_face] = f2;
    m_heData[he2.m_twin] = he1;
    m_heData[he1.m_twin] = he2;

    m_dirty = true;
}

glm::vec3 Mesh::loopCalcEven(std::size_t v) {
    Vertex ve = m_vertData[v];
    HalfEdge* ed = ve.he(*this);
    int n = 1;
    glm::vec3 sum = ed->nvert(*this)->m_pos;
    for (HalfEdge* cur = ed->orbit(*this); cur->twin(*this)->m_next != SIZE_MAX && cur != ed; cur = cur->orbit(*this), n++) {
        sum += cur->nvert(*this)->m_pos;
    }

    float u = n <= 3 ? 3.0 / 16 : 3.0 / (8 * n);
    ve.m_pos = (1.0f - (n * u)) * ve.m_pos + (u * sum);
    return ve.m_pos;
}

glm::vec3 Mesh::loopCalcOdd(std::size_t ed) {
    Edge edg = m_edgeData[ed];
    glm::vec3 A = edg.he(*this)->nvert(*this)->m_pos;
    glm::vec3 B = edg.he(*this)->twin(*this)->nvert(*this)->m_pos;
    glm::vec3 C{0, 0, 0};
    glm::vec3 D{0, 0, 0};

    if (edg.he(*this)->m_next != SIZE_MAX)
        C = edg.he(*this)->next(*this)->nvert(*this)->m_pos;
    if (edg.he(*this)->twin(*this)->m_next != SIZE_MAX)
        D = edg.he(*this)->twin(*this)->next(*this)->nvert(*this)->m_pos;

    return (3.0f / 8) * (A + B) + (1.0f / 8) * (C + D);
}

void Mesh::subdivide() {
    std::size_t len = m_edgeData.size();
    std::size_t marker = m_vertData.size();
    auto evens = std::make_unique<std::vector<glm::vec3>>();
    auto odds = std::make_unique<std::vector<glm::vec3>>();
    evens->reserve(m_vertData.size());
    odds->reserve(m_edgeData.size());
    m_edgeData.reserve(m_edgeData.size());
    m_faceData.reserve(3 * m_faceData.size());
    m_vertData.reserve(m_vertData.size());
    m_heData.reserve(m_heData.size());

    for (std::size_t i = 0; i < m_vertData.size(); i++) {
        evens->push_back(loopCalcEven(i));
    }

    for (std::size_t i = 0; i < m_edgeData.size(); i++) {
        odds->push_back(loopCalcOdd(i));
    }

    for (std::size_t i = 0; i < len; i+=1) {
        edgeSplit(i);
    }

    for (std::size_t i = 0; i < len; i++) {
        Vertex* v = m_edgeData[i].he(*this)->nvert(*this);
        v->m_pos = odds->at(i);
    }

    for (std::size_t i = 0; i < marker; i++) {
        m_vertData[i].m_pos = evens->at(i);
    }

    len = m_edgeData.size();

    for (std::size_t i = 0; i < len; i++) {
        Edge ed = m_edgeData[i];
        if (!ed.is_new)
            continue;
        m_edgeData[i].is_new = false;
        if ((ed.he(*this)->m_nvert >= marker) != (ed.he(*this)->twin(*this)->m_nvert >= marker))
            edgeFlip(i);
    }

    m_dirty = true;
}

void Mesh::transform(glm::mat4x4 m) {
    m_world = m;
    for (std::size_t i = 0; i < m_glData.size(); i++) {
        m_glData[i]->transform(m);
    }
}

glm::mat4x4 Mesh::world() {
    return m_world;
}

void Mesh::setMaterial(CS123SceneMaterial m) {
    m_material = m;
}

CS123SceneMaterial Mesh::getMaterial() {
    return m_material;
}
