#version 150

in vec3 vfColor;
in vec2 vfTexCoord;

out vec4 outColor;

void main()
{

    outColor = vec4(vfTexCoord,1.0f,1.0f);

}

