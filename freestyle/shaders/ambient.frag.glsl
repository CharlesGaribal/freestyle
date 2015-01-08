precision highp float; // needed only for version 1.30
#extension GL_ARB_explicit_attrib_location : enable

uniform float ambientIntensity;
uniform vec3 Ka;

in vec3 varNormal;
in vec4 varTexCoord;

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec4 outNormal;

uniform samplerCube uniEnvMap;
uniform vec3 Ks;
uniform float Ns;

#ifdef TEXTURE_AMBIENT
uniform sampler2D map_ambient;
#endif

#ifdef TEXTURE_DIFFUSE
uniform sampler2D map_diffuse;
#endif

vec3 getKa(){
#ifdef TEXTURE_AMBIENT
    vec3 amb = texture(map_ambient, varTexCoord.st).rgb;
    return max(vec3(0.001), 0.005*amb);
#else
#ifdef TEXTURE_DIFFUSE
    vec3 amb = texture(map_diffuse, varTexCoord.st).rgb;
    return max(vec3(0.001), 0.005*amb);
#endif
#endif
    return  max(vec3(0.001), Ka);
}

void main(void)
{
    outColor = vec4(ambientIntensity*getKa(), 1.0);
    outNormal = vec4(normalize(-varNormal+vec3(0,0,.5))*.5+.5, 1);
}
