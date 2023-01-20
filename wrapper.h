#pragma once
#include <cassert>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <iostream>

#include "glew.h"
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include "shader.h"

#define BUFFER_OFFSET(i) ((char*)NULL + (i))

struct ModelData
{
    unsigned int glVertexData;
    unsigned int glIndexData;
    size_t indexDataLength;
};

struct Vertex
{
    Vertex(float inX, float inY, float inZ) : x(inX), y(inY), z(inZ) {}
    float x, y, z;
};

struct Texture
{
    Texture(float inU, float inV) : u(inU), v(inV) {}
    float u, v;
};

struct Normal
{
    Normal(float inX, float inY, float inZ) : x(inX), y(inY), z(inZ) {}
    float x, y, z;
};

struct Face
{
    Face(int v[], int t[], int n[])
    {
        vIndex[0] = v[0];
        vIndex[1] = v[1];
        vIndex[2] = v[2];
        tIndex[0] = t[0];
        tIndex[1] = t[1];
        tIndex[2] = t[2];
        nIndex[0] = n[0];
        nIndex[1] = n[1];
        nIndex[2] = n[2];
    }
    unsigned int vIndex[3], tIndex[3], nIndex[3];
};

void draw_model(const ModelData& model, Shader& s, const glm::vec3& position, const float y_rot_angle, const glm::vec3& scale, const glm::vec3& color)
{
    glBindBuffer(GL_ARRAY_BUFFER, model.glVertexData);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model.glIndexData);

    auto trans = glm::mat4(1.0f);
    trans = glm::translate(trans, position);
    trans = glm::rotate(trans, glm::radians(y_rot_angle), { 0,1,0 });
    trans = glm::scale(trans, scale);

    s.Bind();
    s.SetUniformMat4f("u_Transformation", trans);
    s.SetUniformVec3f("u_LightPosition", position + glm::vec3{0, 0, 1});
    s.SetUniformVec3f("u_Color", color);

    glDrawElements(GL_TRIANGLES, model.indexDataLength, GL_UNSIGNED_INT, 0);
}

ModelData load_model(const std::string& fileName)
{
    std::fstream myfile;
    std::vector<Vertex> gVertices;
    std::vector<Texture> gTextures;
    std::vector<Normal> gNormals;
    std::vector<Face> gFaces;

    myfile.open(fileName.c_str(), std::ios::in);

    if (myfile.is_open())
    {
        std::string curLine;

        while (getline(myfile, curLine))
        {
            std::stringstream str(curLine);
            float c1, c2, c3;
            unsigned int index[9];
            std::string tmp;

            if (curLine.length() >= 2)
            {
                if (curLine[0] == '#') continue;
                else if (curLine[0] == 'v')
                {
                    if (curLine[1] == 't') // texture
                    {
                        str >> tmp; // consume "vt"
                        str >> c1 >> c2;
                        gTextures.push_back(Texture(c1, c2));
                    }
                    else if (curLine[1] == 'n') // normal
                    {
                        str >> tmp; // consume "vn"
                        str >> c1 >> c2 >> c3;
                        gNormals.push_back(Normal(c1, c2, c3));
                    }
                    else // vertex
                    {
                        str >> tmp; // consume "v"
                        str >> c1 >> c2 >> c3;
                        gVertices.push_back(Vertex(c1, c2, c3));
                    }
                }
                else if (curLine[0] == 'f') // face
                {
                    str >> tmp; // consume "f"
                    char c;
                    int vIndex[3], nIndex[3], tIndex[3];
                    str >> vIndex[0]; str >> c >> c; // consume "//"
                    str >> nIndex[0];
                    str >> vIndex[1]; str >> c >> c; // consume "//"
                    str >> nIndex[1];
                    str >> vIndex[2]; str >> c >> c; // consume "//"
                    str >> nIndex[2];

                    assert(vIndex[0] == nIndex[0] &&
                        vIndex[1] == nIndex[1] &&
                        vIndex[2] == nIndex[2]); // a limitation for now

                    // make indices start from 0
                    for (int c = 0; c < 3; ++c)
                    {
                        vIndex[c] -= 1;
                        nIndex[c] -= 1;
                        tIndex[c] -= 1;
                    }

                    gFaces.push_back(Face(vIndex, tIndex, nIndex));
                }
                else
                {
                    std::cout << "Ignoring unidentified line in obj file: " << curLine << std::endl;
                }
            }
        }
        myfile.close();
    }
    else
    {
        // HAHA
        exit(-31);
    }

    assert(gVertices.size() == gNormals.size());

    GLuint gVertexAttribBuffer, gIndexBuffer;
    GLuint vao;
    glGenVertexArrays(1, &vao);
    assert(vao > 0);
    glBindVertexArray(vao);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    assert(glGetError() == GL_NONE);

    glGenBuffers(1, &gVertexAttribBuffer);
    glGenBuffers(1, &gIndexBuffer);

    assert(gVertexAttribBuffer > 0 && gIndexBuffer > 0);

    glBindBuffer(GL_ARRAY_BUFFER, gVertexAttribBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIndexBuffer);

    int gVertexDataSizeInBytes = gVertices.size() * 3 * sizeof(GLfloat);
    int gNormalDataSizeInBytes = gNormals.size() * 3 * sizeof(GLfloat);
    int indexDataSizeInBytes = gFaces.size() * 3 * sizeof(GLuint);
    GLfloat* vertexData = new GLfloat[gVertices.size() * 3];
    GLfloat* normalData = new GLfloat[gNormals.size() * 3];
    GLuint* indexData = new GLuint[gFaces.size() * 3];

    for (int i = 0; i < gVertices.size(); ++i)
    {
        vertexData[3 * i] = gVertices[i].x;
        vertexData[3 * i + 1] = gVertices[i].y;
        vertexData[3 * i + 2] = gVertices[i].z;
    }

    for (int i = 0; i < gNormals.size(); ++i)
    {
        normalData[3 * i] = gNormals[i].x;
        normalData[3 * i + 1] = gNormals[i].y;
        normalData[3 * i + 2] = gNormals[i].z;
    }

    for (int i = 0; i < gFaces.size(); ++i)
    {
        indexData[3 * i] = gFaces[i].vIndex[0];
        indexData[3 * i + 1] = gFaces[i].vIndex[1];
        indexData[3 * i + 2] = gFaces[i].vIndex[2];
    }


    glBufferData(GL_ARRAY_BUFFER, gVertexDataSizeInBytes + gNormalDataSizeInBytes, 0, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, gVertexDataSizeInBytes, vertexData);
    glBufferSubData(GL_ARRAY_BUFFER, gVertexDataSizeInBytes, gNormalDataSizeInBytes, normalData);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexDataSizeInBytes, indexData, GL_STATIC_DRAW);

    delete[] vertexData;
    delete[] normalData;
    delete[] indexData;

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(gVertexDataSizeInBytes));

    return ModelData{ gVertexAttribBuffer, gIndexBuffer, gFaces.size() * 3};
}