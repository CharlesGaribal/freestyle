// Vertex Shader – file "default.vert"
//#extension GL_ARB_uniform_buffer_object : enable

//******	WITH UBO    ******
//layout(std140) uniform MatriceBlock{
uniform	mat4 MVP;
uniform	mat4 normalMatrix;
//};
//****************************

in vec3 inPosition;
in vec3 inNormal;
in vec3 inTangent;
in vec4 inTexCoord;

out vec3 varNormal;
out vec4 varTexCoord;

void main(void)
{
  varNormal = (normalMatrix*vec4(inNormal,0.0)).xyz;
  gl_Position = MVP*vec4(inPosition.xyz, 1.0);
  varTexCoord = inTexCoord;
}
