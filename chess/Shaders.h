#ifndef SHADERS_H_
#define SHADERS_H_

#include "string"

namespace Shaders{

	const std::string vertexShader =
		R"(
		#version 460 core

		layout(location = 0) in vec2 position;
		layout(location = 1) in vec2 tcoords; 

		out vec2 positions;
		out vec2 vs_tcoords;

		uniform mat4 u_chessViewProjMat;
		uniform mat4 u_chessModMat;

		void main()
		{
			positions = position;
			gl_Position = u_chessViewProjMat*u_chessModMat*vec4(position,0.0, 1.0);
			vs_tcoords = tcoords;
		}
		)";
	


	const std::string fragmentShader =
		R"(
		#version 460 core

		#define M_PI 3.14159265358979323846 

		in vec2 vs_tcoords;
		in vec2 positions;

		out vec4 finalColor;
				
		uniform vec2 u_divisions=vec2(0.0);
		uniform vec2 u_selectorPosition=vec2(0.0);
		uniform vec4 u_selectorColor=vec4(0.0f,1.0f,0.0f,1.0f);
		uniform vec3 u_color1=vec3(0.0);
		uniform vec3 u_color2=vec3(1.0);
		uniform int u_texture;

		layout(binding=0) uniform sampler2D u_floorTextureSampler;

		void main()
		{

			if(sin(M_PI*u_divisions.y*positions.y)>0)
			{
				if(sin(M_PI*u_divisions.x*positions.x)>0)
				{
					finalColor = vec4(u_color1,1.0);
					
				}
				else
				{
					finalColor = vec4(u_color2,0.0);
					
				}
			}
			else
			{
				if(sin(M_PI*u_divisions.y*positions.x)>0)
				{
					finalColor = vec4(u_color2,0.0);
					
				}
				else
				{
					finalColor = vec4(u_color1,1.0);
					
				}
			}

			if(positions.x<(u_selectorPosition.x+1)/u_divisions.x - 0.5 && positions.x > u_selectorPosition.x/u_divisions.x - 0.5 &&
			   positions.y<(u_selectorPosition.y+1)/u_divisions.y - 0.5 && positions.y > u_selectorPosition.y/u_divisions.y - 0.5){
					finalColor = u_selectorColor;
				
			}

			if(u_texture == 1)
				finalColor = mix(finalColor,texture(u_floorTextureSampler, vs_tcoords),0.7);
		}
		)";
	

	const std::string cubeVertexShader =
		R"(
		#version 460 core

		layout(location = 0) in vec3 position;
		
		out vec3 vs_pos;
		
		uniform mat4 u_cubeViewProjMat;
		uniform mat4 u_cubeModMat;
		uniform vec2 u_selPos;

		void main(){
			
			gl_Position = u_cubeViewProjMat*u_cubeModMat*vec4(position, 1.0);
			vs_pos = position;
		}
		)";

	const std::string cubeFragmentShader =
		R"(
		#version 460 core
		
		in vec3 vs_pos;

		out vec4 finalColor;

		uniform vec4 cubeColor = vec4(0.0f,0.0f,1.0f,1.0f);
		uniform int u_texture;

		layout(binding=1) uniform samplerCube u_cubeTextureSampler;

		void main(){
			if(u_texture==0){
				finalColor = cubeColor;
			}
			else{
				finalColor = mix(cubeColor,texture(u_cubeTextureSampler, vs_pos),0.5);
			}
		}
		
		)";
}
#endif
