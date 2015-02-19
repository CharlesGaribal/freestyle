precision highp float; // needed only for version 1.30

uniform vec3 uniLightSpecular;  ///light specular color
uniform vec3 uniLightDiffuse; /// light diffuse color
uniform float uniLightAngleOuterCone; /// spotlight cone size

uniform float uniLightAngleInnerCone; /// spotlight  inner cone size

uniform vec3 Ks; /// specular color
uniform vec3 Kd; /// diffuse color
uniform float Ns; /// shininess

uniform float toolRadius;

in vec3 varColor;
in vec3 varEyeVec;
in vec3 varNormal;
in vec3 varLightVec;
in vec3 varLightSpotDir;
in vec4 varTexCoord;

in float varDist;

out vec4 outColor;

#ifdef TEXTURE_DIFFUSE
uniform sampler2D map_diffuse;
#endif

#ifdef TEXTURE_SPECULAR
uniform sampler2D map_specular;
#endif


#ifdef TEXTURE_OPACITY
uniform sampler2D map_opacity;
uniform float opacityLevel;
#endif

#ifdef TEXTURE_NORMALS
uniform sampler2D map_normals;
#endif


const float PI=3.14159265;

vec3 getKd(){
    vec3 rho;
#ifdef TEXTURE_DIFFUSE
    rho = texture(map_diffuse, varTexCoord.st).rgb;
#else
    rho = Kd;
#endif

    if (varDist < toolRadius && varDist > 0)
        rho = 1 - rho;

    return pow(rho, vec3(2.2));
}

float getNs(){
#ifdef TEXTURE_SPECULAR
    float shineTex = texture(map_specular, varTexCoord.st).a;
    return max(Ns*shineTex, 0.01);
#else
    return max(Ns, 00.01);
#endif
}

vec3 getKs(){
    vec3 rho;
#ifdef TEXTURE_SPECULAR
    rho =  Ks*texture(map_specular, varTexCoord.st).rgb;
#else
    rho = Ks;
#endif
    return rho;
}

vec3 getNormal(){
    float dir = 1.;
    return dir*normalize(varNormal);
}

vec3 safeNormalize(vec3 v){
    float len = length(v);
    return len>0.0?v/len:vec3(0.0,0.0,0.0);
}

float lightAttenuation=1.0;

vec4 phong(vec3 normal, vec3 light, vec3 eye){
    float cosTr = clamp(dot(reflect(-light, normal), eye), 0.0, 1.0);
    float cosTi = clamp(dot(normal, light), 0.0, 1.0);
    vec3 Kd = getKd();
    float Ns = getNs();
    vec3 Ks = getKs();
    vec3 Rf0 = 2*Ks/(Ns+2);

    float spotLightCoef = lightAttenuation;
    vec3 Ed = uniLightDiffuse*spotLightCoef;
    vec3 Es = uniLightSpecular*spotLightCoef;

    return vec4( cosTi*(Kd/PI*Ed + (Ns+2)/(2*PI)*Rf0*pow(cosTr, Ns)*Es), 1.0  );
}

void main(void)
{
    vec3 normal = getNormal();
    vec3 eye = safeNormalize(varEyeVec);
    float lightDist = length(varLightVec);
    vec3 light = (varLightVec)/lightDist;
    //lightAttenuation = (1-smoothstep(.5*uniLightAngleInnerCone, 0.5*uniLightAngleOuterCone, acos(dot(light, normalize(-varLightSpotDir.xyz)))));
    lightAttenuation = 1.0-smoothstep(uniLightAngleInnerCone, uniLightAngleInnerCone+ (uniLightAngleOuterCone-uniLightAngleInnerCone)/20., acos(dot(light, normalize(-varLightSpotDir.xyz))));
    outColor = max(phong(normal, light, eye), vec4(0.0));
}
