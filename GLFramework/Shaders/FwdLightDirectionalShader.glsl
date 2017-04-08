<VERTEX>
	#version 330 core
	layout (location = 0) in vec3 pos;
	layout (location = 1) in vec2 texCoords;

	out vec2 TexCoords;

	void main()
	{
		TexCoords = texCoords;
		gl_Position = vec4(pos, 1.0);
	}
</VERTEX>
<FRAGMENT>
	#version 330 core
	
	#include "Common.glsl"
	#include "CommonDeferred.glsl"
	#include "CommonPBR.glsl"
	
	in vec2 TexCoords;
	
	//out
	layout (location = 0) out vec4 outColor;
	layout (location = 1) out vec4 brightColor;//probably going to be done differently
	
	GBUFFER_SAMPLER
	
	//Light
	uniform vec3 Direction;
	uniform vec3 Color;
	
	uniform vec3 camPos;
	
	//Lighting function
	vec3 DirLighting(vec3 baseCol, float rough, float metal, vec3 F0, vec3 norm, vec3 viewDir)
	{
		vec3 lightDir = Direction;
		vec3 H = normalize(lightDir+viewDir);
		
		vec3 radiance = Color;
		
		vec3 F  = FresnelSchlick(max(dot(H, viewDir), 0.0), F0);	//Fresnel
		float NDF = DistributionGGX(norm, H, rough); 				//Normalized distribution funciton
		float G   = GeometrySmith(norm, viewDir, lightDir, rough);  //Geometry shadowing
		
		//Calculate how much the light contributes
		vec3 kS = F;
		vec3 kD = vec3(1.0) - kS;
		kD *= 1.0 - metal;
		
		//Cook torrance BRDF
		vec3 nominator 	  = NDF * G * F;
		float denominator = 4 * max(dot(norm, viewDir), 0.0) * max(dot(norm, lightDir), 0.0) + 0.001;
		vec3 brdf		  = nominator / denominator;
		
		// add to outgoing radiance Lo
		float NdotL = max(dot(norm, lightDir), 0.0);                
		return (kD * baseCol / PI + brdf) * radiance * NdotL; 
	}	
	
	void main()
	{
		UNPACK_GBUFFER(TexCoords) //maybe use ao??
		
		//precalculations	
		vec3 F0 = vec3(0.04);//for dielectric materials use this simplified constant
		F0 		= mix(F0, baseCol, metal);//for metal we should use the albedo value
		//View dir and reflection
		vec3 viewDir = normalize(camPos - pos);
		
		vec3 finalCol = DirLighting(baseCol, rough, metal, F0, norm, viewDir);
		
		//output
		outColor = vec4(clamp(finalCol, 0.0, maxExposure), 1.0);
		float brightness = dot(outColor.rgb, vec3(0.2126, 0.7152, 0.0722));
		if(brightness > 1.0) brightColor = vec4(outColor.rgb, 1.0);
	}
</FRAGMENT>