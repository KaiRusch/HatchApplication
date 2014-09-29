#version 150

in vec2 pvPosition;
in vec3 pvColor;
in vec2 pvTexCoord;

out vec3 vfColor;
out vec2 vfTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main()
{
    gl_Position = proj*view*model*vec4(pvPosition,0.0,1.0);
     vfColor = pvColor;
     vfTexCoord = pvTexCoord;
}
