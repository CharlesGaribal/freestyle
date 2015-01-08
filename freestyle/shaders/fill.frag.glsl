precision highp float; // needed only for version 1.30
#extension GL_ARB_explicit_attrib_location : enable

uniform vec4 color;

layout(location = 0) out vec4 outColor;

void main(void)
{  
    outColor = color;
}
