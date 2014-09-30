#version 150

in vec3 vfColor;
in vec2 vfTexCoord;

out vec4 outColor;

uniform sampler2D tex;

void main()
{

    outColor = texture(tex,vec2(1-vfTexCoord.x,vfTexCoord.y));

}

