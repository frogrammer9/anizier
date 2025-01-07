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
	gl_Position = vec4(iPos.x * 2.f, iPos.y * 2.f, 0.f, 1.f);
}

#shader geometry
#version 330 core

layout(lines) in;  
layout(triangle_strip, max_vertices = 4) out;

const float lineWidth = 7;  

uniform vec2 uWinSize;

in VS_OUT {
    vec4 color;
} gs_in[];

out vec4 fCol;

void main() {
	fCol = gs_in[0].color;

    vec4 p0 = gl_in[0].gl_Position;  
    vec4 p1 = gl_in[1].gl_Position; 

    vec2 direction = normalize(vec2(p1.x - p0.x, p1.y - p0.y));
    vec2 perpendicular = vec2(-direction.y, direction.x);  // Perpendicular vector to the line

    gl_Position = vec4((p0.x + perpendicular.x * lineWidth) / uWinSize.x, (p0.y + perpendicular.y * lineWidth) / uWinSize.y, 0.f, 1.0); 
    EmitVertex();
    gl_Position = vec4((p0.x - perpendicular.x * lineWidth) / uWinSize.x, (p0.y - perpendicular.y * lineWidth) / uWinSize.y, 0.f, 1.0); 
    EmitVertex();
    gl_Position = vec4((p1.x + perpendicular.x * lineWidth) / uWinSize.x, (p1.y + perpendicular.y * lineWidth) / uWinSize.y, 0.f, 1.0); 
    EmitVertex(); 
    gl_Position = vec4((p1.x - perpendicular.x * lineWidth) / uWinSize.x, (p1.y - perpendicular.y * lineWidth) / uWinSize.y, 0.f, 1.0); 
    EmitVertex();  

    EndPrimitive();  
}

#shader fragment 
#version 330 core

in vec4 fCol;

out vec4 color;

void main() {
	color = fCol;
}
