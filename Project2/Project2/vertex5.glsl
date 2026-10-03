#version 330 core

uniform vec2 offset;

void main()
{
   const vec4 vertex[12] = vec4[12]
    (
        // 점: 0번 정점
        vec4( 0.1,  0.05, 0.5, 1.0),
        vec4(-0.1,  0.05, 0.5, 1.0),
        vec4( -0.1,  -0.05, 0.5, 1.0),
        vec4(0.1, -0.05, 0.5, 1.0),

        vec4(-0.2,-0.8,0.5,1.0),
        vec4(0.2, -0.8,0.5,1.0),
        vec4(0.2,0.8,0.5,1.0),
        vec4(-0.2,0.8,0.5,1.0),
        
        vec4(-0.5,-0.15,0.5,1.0),
        vec4(0.5,-0.15,0.5,1.0),
        vec4(0.5,0.15,0.5,1.0),
        vec4(-0.5,0.15,0.5,1.0)
    );

    gl_Position = vertex[gl_VertexID];
    gl_Position.x += offset.x;
    gl_Position.y += offset.y;
}
