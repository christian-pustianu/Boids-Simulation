#version 430
layout (location = 0) in vec3 iPosition;

layout( location = 0 ) uniform mat4 uWorld2projection;
layout( location = 1 ) uniform mat4 view;

out vec3 v2fTexCoords;

void main()
{
    v2fTexCoords = iPosition;
    gl_Position = uWorld2projection * view * vec4(iPosition, 1.0);
}  