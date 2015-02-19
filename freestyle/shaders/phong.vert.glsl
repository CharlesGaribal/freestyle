// Vertex Shader – file "default.vert"
//#extension GL_ARB_uniform_buffer_object : enable


//******	WITH UBO    ******
//layout(std140) uniform MatriceBlock{
uniform	mat4 modelViewMatrix;
uniform	mat4 inverseViewMatrix;
uniform	mat4 projectionMatrix;
uniform	mat4 MVP;
uniform	mat4 normalMatrix;
//};
//****************************
uniform vec3 uniLightPosition; /// spot light position
uniform vec3 uniLightDirection; /// spot light direction

uniform vec4 vertexSelected;
uniform bool validSelection;
uniform float toolRadius;

in vec3 inPosition;
in vec3 inNormal;
in vec4 inTexCoord;

out vec3 varNormal;
out vec3 varEyeVec;
out vec3 varLightVec;

out vec3 varLightSpotDir;
out vec4 varTexCoord;

out float varDist;

float dist(vec3 p1, vec3 p2) {
    return sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2) + pow(p1.z - p2.z, 2));
}

void main(void)
{
  varNormal   = (normalMatrix * vec4(inNormal,0.0)).xyz;

  varTexCoord = inTexCoord;

  if (validSelection)
      varDist = dist(inPosition, vertexSelected.xyz);
  else
      varDist = -1;

  vec4 viewPosition = (modelViewMatrix * vec4(inPosition, 1.));
  viewPosition /= viewPosition.w;

  // lighting
  varLightVec = (uniLightPosition-viewPosition.xyz);
  varEyeVec = -viewPosition.xyz;
  varLightSpotDir = uniLightDirection;

  gl_Position = MVP*vec4(inPosition.xyz, 1.0);

}
