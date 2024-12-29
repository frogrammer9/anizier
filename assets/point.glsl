#shader vertex
#version 330 core

vec4 calcColor(uint col) {
	return vec4(float(col >> 24u) / 255.f, float((col >> 16u) & 0xFFu) / 255.f, float((col >> 8u) & 0xFFu) / 255.f, float(col & 0xFFu) / 255.f);
}

uniform vec2 uWinSize;

layout(location = 0) in vec2 iPos;
layout(location = 1) in uint iCol;

out VS_OUT {
	vec4 color;
} vs_out;

void main() {
	vs_out.color = calcColor(iCol);
	gl_Position = vec4(iPos.x / uWinSize.x * 2.f, iPos.y / uWinSize.y * 2.f, 0.f, 1.f);
}

#shader geometry
#version 330 core

layout(points) in; 
layout(triangle_strip, max_vertices = 4) out; 

in VS_OUT {
    vec4 color;
} gs_in[];

out vec4 fCol;
out vec2 fCord;

uniform float pointSize;
uniform vec2 uWinSize;

void main() {
    vec4 center = gl_in[0].gl_Position;
    float halfSize = pointSize * 0.5;
	fCol = gs_in[0].color;

    gl_Position = center + vec4(-halfSize / uWinSize.x, -halfSize / uWinSize.y, 0.0, 0.0); // Bottom-left
	fCord = vec2(0.f, 0.f);
    EmitVertex();
    gl_Position = center + vec4(halfSize / uWinSize.x, -halfSize / uWinSize.y, 0.0, 0.0); // Bottom-right
	fCord = vec2(1.f, 0.f);
    EmitVertex();
    gl_Position = center + vec4(-halfSize / uWinSize.x, halfSize / uWinSize.y, 0.0, 0.0); // Top-left
	fCord = vec2(0.f, 1.f);
    EmitVertex();
    gl_Position = center + vec4(halfSize / uWinSize.x, halfSize / uWinSize.y, 0.0, 0.0); // Top-right
	fCord = vec2(1.f, 1.f);
    EmitVertex();
    EndPrimitive(); 
}

#shader fragment 
#version 330 core

in vec4 fCol;
in vec2 fCord;
out vec4 color;

vec4 calcColor(uint col) {
	return vec4(float(col >> 24u) / 255.f, float((col >> 16u) & 0xFFu) / 255.f, float((col >> 8u) & 0xFFu) / 255.f, float(col & 0xFFu) / 255.f);
}

void main() {
	if(length(fCord - vec2(.5f, .5f)) > .5f) discard;
	color = fCol;
}
