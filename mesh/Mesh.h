#ifndef MESH_H
#define MESH_H

#include <vector>
#include <memory>
#include "glm.hpp"
#include "qfile.h"
#include "GL/glew.h"
#include "shapes/PolySoup.h"
#include "CS123SceneData.h"
#include "gl/shaders/CS123Shader.h"

class Mesh
{
public:
    Mesh();
    Mesh(const std::string& filename);
    Mesh(QFile& objFile);

    void draw();
    void draw(CS123::GL::CS123Shader* shader);
    void subdivide(std::size_t face);
    void edgeSplit(std::size_t edge);
    void edgeFlip(std::size_t edge);
    void subdivide();
    void setMaterial(CS123SceneMaterial m);
    CS123SceneMaterial getMaterial();
    void transform(glm::mat4x4 m);
    glm::mat4x4 world();
private:
    class Vertex;
    class HalfEdge;
    class Edge;
    class Face;

    class HalfEdge
    {
    public:
        inline HalfEdge* orbit(Mesh& m) {
            return twin(m)->next(m);
        }

        inline Vertex* tail(Mesh& m) {
            return next(m)->next(m)->nvert(m);
        }

        inline HalfEdge* twin(Mesh& m) {
            return m.m_heData.data() + m_twin;
        }

        inline HalfEdge* next(Mesh& m) {
            return m.m_heData.data() + m_next;
        }

        inline Edge* edge(Mesh& m) {
            return m.m_edgeData.data() + m_edge;
        }

        inline Vertex* nvert(Mesh& m) {
            return m.m_vertData.data() + m_nvert;
        }

        inline Face* face(Mesh& m) {
            return m.m_faceData.data() + m_face;
        }
        std::size_t m_face = SIZE_MAX;
        std::size_t m_edge = SIZE_MAX;
        std::size_t m_twin = SIZE_MAX;
        std::size_t m_next = SIZE_MAX;
        std::size_t m_nvert = SIZE_MAX;
    };

    class Vertex
    {
    public:
        inline HalfEdge* he(Mesh& m) {
            return m.m_heData.data() + m_he;
        }
        glm::vec3 m_pos;
        glm::vec3 m_norm;
        std::size_t m_he = SIZE_MAX;
    };

    class Face
    {
    public:
        inline HalfEdge* he(Mesh& m) {
            return m.m_heData.data() + m_he;
        }
        std::size_t m_he = SIZE_MAX;
        uint a;
        uint b;
        uint c;
    };

    class Edge
    {
    public:
        inline HalfEdge* he(Mesh& m) {
            return m.m_heData.data() + m_he;
        }
        std::size_t m_he = SIZE_MAX;
        bool is_new = false;
    };

    std::size_t faceSplit(std::size_t f);
    std::size_t faceSplit(std::size_t f, std::size_t v);
    glm::vec3 loopCalcEven(std::size_t v);
    glm::vec3 loopCalcOdd(std::size_t ed);
    void genglData();
    void genNormals();
    void genHE();
    void parseObj(QFile& file);

    bool m_dirty;
    CS123SceneMaterial m_material;
    glm::mat4x4 m_world;

    std::vector<Vertex> m_vertData;
    std::vector<Face> m_faceData;
    std::vector<Edge> m_edgeData;
    std::vector<HalfEdge> m_heData;
    std::vector<std::unique_ptr<PolySoup>> m_glData;
};

#endif // MESH_H
