#version 430
layout (location = 0) in vec3 iPosition;

layout( location = 0 ) uniform mat4 uProjection;
layout( location = 1 ) uniform mat4 view;

out vec3 v2fTexCoords;

void main()
{
    v2fTexCoords = iPosition;
    vec4 pos = uProjection * view * vec4(iPosition, 1.0);
    gl_Position = pos.xyww;
}