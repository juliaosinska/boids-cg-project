#ifndef SKYBOX_H
#define SKYBOX_H

GLfloat skyboxVertices[] =
{
    //   coordinates
    -1.0f, -1.0f,  1.0f,//        7--------6
     1.0f, -1.0f,  1.0f,//       /|       /|
     1.0f, -1.0f, -1.0f,//      4--------5 |
    -1.0f, -1.0f, -1.0f,//      | |      | |
    -1.0f,  1.0f,  1.0f,//      | 3------|-2
     1.0f,  1.0f,  1.0f,//      |/       |/
     1.0f,  1.0f, -1.0f,//      0--------1
    -1.0f,  1.0f, -1.0f
};

GLuint skyboxIndices[] =
{
    // right
    1, 2, 6,
    6, 5, 1,
    // left
    0, 4, 7,
    7, 3, 0,
    // top
    4, 5, 6,
    6, 7, 4,
    // bottom
    0, 3, 2,
    2, 1, 0,
    // back
    0, 1, 5,
    5, 4, 0,
    // front
    3, 7, 6,
    6, 2, 3
};

GLfloat quadVertices[] = {
    // position    // texture
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
     1.0f, -1.0f,  1.0f, 0.0f,

    -1.0f,  1.0f,  0.0f, 1.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
     1.0f,  1.0f,  1.0f, 1.0f
};

#endif
