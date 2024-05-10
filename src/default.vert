
#version 330 core

layout (location = 0) in vec4 aPos;


uniform mat4 view_mat;
uniform mat4 project_mat;

out vec3 pos;
out int index;

void main() {

	pos = vec3(aPos.x, aPos.y, aPos.z);
	index = int(aPos.w);

	gl_Position = project_mat * view_mat * vec4(aPos.x, aPos.y, aPos.z, 1.0f);
	
}
