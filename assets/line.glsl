#shader vertex
#version 330 core

vec4 calcColor(uint col) {
	return vec4(float(col >> 24u) / 255.f, float((col >> 16u) & 0xFFu) / 255.f, float((col >> 8u) & 0xFFu) / 255.f, float(col & 0xFFu) / 255.f);
}

uniform vec2 uWinSize;

layout(location = 0) in vec2 iPos;
layout(location = 1) in uint iCol;

out vec4 vCol;

void main() {
	vCol = calcColor(iCol);
	gl_Position = vec4(iPos.x / uWinSize.x * 2.f, iPos.y / uWinSize.y * 2.f, 0.f, 1.f);
}

#shader fragment 
#version 330 core

in vec4 vCol;

out vec4 color;

void main() {
	color = vCol;
}
