#ifndef BOX_H
#define BOX_H

GLfloat boxVertices[] = {
    //     COORDINATES      //    COLORS  //      NORMALS    
   -0.1f, -0.1f,  0.1f,  0.8f, 0.52f, 0.25f,   0.0f, 1.0f,  // front face
     0.1f, -0.1f,  0.1f,  0.8f, 0.52f, 0.25f,   1.0f, -1.0f,  // back face
     0.1f,  0.1f,  0.1f,  0.8f, 0.52f, 0.25f,   1.0f, -1.0f,  // back face
     -0.1f,  0.1f,  0.1f,  0.8f, 0.52f, 0.25f,   0.0f, 1.0f,  // front face

     -0.1f, -0.1f, -0.1f,  0.8f, 0.52f, 0.25f,   0.0f, 1.0f,  // top face
     0.1f, -0.1f, -0.1f,   0.8f, 0.52f, 0.25f,  0.0f, -1.0f,  // bottom face
     0.1f,  0.1f, -0.1f,  0.8f, 0.52f, 0.25f,   0.0f, -1.0f,  // bottom face
     -0.1f,  0.1f, -0.1f,   0.8f, 0.52f, 0.25f,   0.0f, 1.0f   // top face
};

GLuint boxIndices[] = {
    // front face
    0, 1,
    1, 2,
    2, 3,
    3, 0,

    // back face
    4, 5,
    5, 6,
    6, 7,
    7, 4,

    // left face
    0, 4,
    1, 5,
    2, 6,
    3, 7,

    // right face
    0, 5,
    1, 4,
    2, 7,
    3, 6,

    // top face
    3, 2,
    7, 6,

    // bottom face
    0, 1,
    4, 5
};

#endif
