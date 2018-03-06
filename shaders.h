#ifndef SHADERS_H
#define SHADERS_H

/* Control points - edges - Delaunay triangulation */
const char *vertexShaderSource =
    "attribute vec4 posAttr;\n"
    "uniform mat4 matrix;\n"
    "void main() { gl_Position = matrix * posAttr; }";

const char *fragmentShaderSource =
    "uniform vec4 colorAttr;\n"
    "void main() { gl_FragColor = colorAttr; }";


/* textures */
const char *vertexTexture =
    "attribute vec4 posAttr;\n"
    "varying vec2 vTextCoord;\n"        // in
    "uniform mat4 matrix;\n"
    "varying vec2 textCoord;\n"         // out
    "void main() {\n"
    "   gl_Position = matrix * posAttr;\n"
    "   textCoord = vTextCoord;\n"
    "}\n";

const char *fragmentTexture =
    "varying vec2 textCoord;\n"         // in
    "uniform sampler2D texture_2D;\n"
    "void main() { gl_FragColor = texture2D(texture_2D, textCoord); }";

#endif // SHADERS_H
