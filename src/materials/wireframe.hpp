#pragma once

class Material;

// Enum that defines the type of uniform to be passed to a shader
enum {
    NumVec4 = 3
};

namespace wireframe {

enum class DrawMode {
    WireframeShaded,
    Wireframe,
    Shaded,
};

struct Wireframe {
    union {
        struct {
            /*0*/struct { float _camPos[3], _unused0; };
            /*1*/struct { float _wfColor[4]; };
            /*2*/struct { float _drawEdges, _wfThickness, _unused2[2]; };
        };
        float _params[NumVec4 * 4];
    };
    DrawMode _drawMode;
};

Material createInstance();
}