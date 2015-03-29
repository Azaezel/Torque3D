//-----------------------------------------------------------------------------
// Torque 3D
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#include "shadergen:/autogenConditioners.h"
#include "./../postFx.hlsl"
#include "./../../torque.hlsl"

uniform sampler2D prepassTex        : register(S0);
uniform sampler2D lightPrePassTex   : register(S1);
uniform sampler2D wetMap            : register(S2);
uniform sampler2D backbuff          : register(S3);

uniform float accumTime             : register(C1);

uniform float3    eyePosWorld;

//----------------------------------------------
// Reflection Shader
//----------------------------------------------
struct ConnectData
{
	float2 texCoord : TEXCOORD0;
	float4 noiseCoord : TEXCOORD1;
};

float4 main(ConnectData In) : COLOR
{
   //return float4(0, 0, 0, 0);
   float4 color;
   float3 lightcolor, refraction, refNorm;   
   float nl_Att, specular, refSpec;
   float amount = 0.75;
   
   // Get the prepass texture for uv channel 0
   float4 prepass = prepassUncondition( prepassTex, In.texCoord );
   float3 normal = prepass.rgb; // Get the normal from the prepass texture
   float depth = prepass.a; // Get the depth form the prepass
    
   // Early out if too far away
   if ( depth > 0.99999999 )
      return float4(0, 0, 0, 0);
      
   // Get the refraction UV
   float4 refUV = In.noiseCoord;
   
	//Fetch the normals and decompress
	float4 normalColor = (tex2D(wetMap, refUV.xy) * 2) - 1.0f;
	float4 animColor = (tex2D(wetMap, refUV.zw) * 2) - 1.0f;

	normalColor.z += animColor.w;
	
	//Find the refraction from the normal.  It's essentially the normal restrained in such a way that 
	float3 refractionVec = normalize(normalColor.xyz);

	//Fetch the screen by displacing the texture coordinates by the refraction
	float3 screenColor = tex2D(backbuff, In.texCoord - (refractionVec.xy * 0.2f * amount)).xyz;

	//Find a specular component from the refraction
	float2 refractHighlight = refractionVec.xy * amount;
	float3 refColor = (saturate(pow(refractHighlight.x, 6.0f)) + saturate(pow(refractHighlight.y, 6.0f)))
	* float3(0.85f, 0.85f, 1.0f);
   
   // Get the speculariry of the object we're interacting with
   lightinfoUncondition( tex2D( lightPrePassTex, In.texCoord ), lightcolor, nl_Att, specular );
   if(specular < 0.2)
      return float4(0, 0, 0, 1);
   
   color = float4(screenColor + refColor, 1);
   color.rgb *= specular;
	return hdrEncode( color ); 
}