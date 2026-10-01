#version 330 core

uniform vec2 offset;
uniform float scale;

void main()
{
   const vec4 vertex[6] = vec4[6]
    (
       vec4( 0.0, 0.0, 0.0, 1.0),

       // 이등변 삼각형
       vec4(-0.1, -0.08, 0.5, 1.0),
       vec4( 0.1, -0.08, 0.5, 1.0),
       vec4( 0.0,  0.16, 0.5, 1.0),

       vec4(0.5,  1.0, 0.5, 1.0),
       vec4(0.5, -1.0, 0.5, 1.0)

    );

    gl_Position = vertex[gl_VertexID];
    gl_Position.x = gl_Position.x*scale+offset.x;
    gl_Position.y = gl_Position.y*scale+offset.y;
}
