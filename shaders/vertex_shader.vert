#version 430 core

in vec3 position;
in vec3 color;

uniform mat4 perspProj;
uniform mat4 rotMatrix;
uniform vec3 camPos;
uniform float time;


out vec3 vertColor;

void main()
{
  gl_Position = perspProj*rotMatrix*vec4(position-camPos,1.0f);
  vertColor = color;
}
