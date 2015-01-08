// Vertex Shader – file "default.vert"
//#extension GL_ARB_uniform_buffer_object : enable

uniform	mat4 MVP;

in vec3 inPosition;

void main(void)
{
  gl_Position = MVP*vec4(inPosition.xyz, 1.0);
}
