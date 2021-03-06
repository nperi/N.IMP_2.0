#version 150

uniform float MaterialThickness;
uniform vec3 ExtinctionCoefficient;
uniform vec4 LightColor;
uniform vec4 BaseColor;
uniform vec4 SpecColor;
uniform float SpecPower;
uniform float RimScalar;
uniform float AttenuationOffset;
uniform sampler2D Texture;

in vec3 worldNormal;
in vec3 eyeVec;
in vec3 lightVec;
in vec3 vertPos;
in vec3 lightPos;
in vec2 varyingtexcoord;
out vec4 outputcolor;

float halfLambert(vec3 vect1, vec3 vect2){
    float product = dot(vect1,vect2);
    return product * 0.5 + 0.5;
}

float blinnPhongSpecular(vec3 normalVec, vec3 lightVec, float specPower){
    vec3 halfAngle = normalize(normalVec + lightVec);
    return pow(clamp(0.0,1.0,dot(normalVec,halfAngle)),specPower);
}

vec4 subScatterFS(){
    vec2 vUv = varyingtexcoord;
    float attenuation = 10.0 * (1.0 / distance(lightPos,vertPos)) + AttenuationOffset;
    vec3 eVec = normalize(eyeVec);
    vec3 lVec = normalize(lightVec);
    vec3 wNorm = normalize(worldNormal);
    
    vec4 dotLN = vec4(halfLambert(lVec,wNorm) * attenuation);
    dotLN *= texture(Texture, vertPos);
    dotLN *= BaseColor;
    
    vec3 indirectLightComponent = vec3(MaterialThickness * max(0.0,dot(-wNorm,lVec)));
    indirectLightComponent += MaterialThickness * halfLambert(-eVec,lVec);
    indirectLightComponent *= attenuation;
    indirectLightComponent.r *= ExtinctionCoefficient.r;
    indirectLightComponent.g *= ExtinctionCoefficient.g;
    indirectLightComponent.b *= ExtinctionCoefficient.b;
    indirectLightComponent.rgb *= texture(Texture, vertPos).rgb;
    vec3 rim = vec3(1.0 - max(0.0,dot(wNorm,eVec)));
    rim *= rim;
    rim *= max(0.0,dot(wNorm,lVec)) * SpecColor.rgb;
    
    vec4 finalCol = dotLN + vec4(indirectLightComponent,1.0);
    finalCol.rgb += (rim * RimScalar * attenuation * finalCol.a);
    finalCol.rgb += vec3(blinnPhongSpecular(wNorm,lVec,SpecPower) * attenuation * SpecColor * finalCol.a * 0.05);
    finalCol.rgb *= LightColor.rgb;
    
    return finalCol;
}


void main(){
    outputcolor = subScatterFS();
}