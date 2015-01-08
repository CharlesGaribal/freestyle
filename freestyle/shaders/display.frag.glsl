precision highp float; // needed only for version 1.30

in vec4 varTexCoord;
out vec4 outColor;
uniform sampler2D color;
void main(void)
{
   outColor = pow(vec4(texture(color, varTexCoord.xy)), vec4(1.0/(2.2)));
   //outColor = vec4(texture(color, varTexCoord.xy));
}
