#version 450
layout(set = 0, binding = 0) uniform Rotation {
    float uRot;
};
layout(location = 0) in  vec2 aPos;
layout(location = 1) in  vec3 aCol;
layout(location = 0) out vec3 vCol;
void main() {
    float cosA = cos(radians(uRot));
    float sinA = sin(radians(uRot));
    mat3 rot = mat3(cosA, sinA, 0.0,
                    -sinA, cosA, 0.0,
                    0.0,  0.0,  1.0);
    gl_Position = vec4(rot * vec3(aPos, 1.0), 1.0);
    vCol = aCol;
 }