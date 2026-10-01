#version 330 core

uniform vec2 offset;
uniform vec2 scale;

void main()
{
   const vec4 vertex[10] = vec4[10]
    (
        vec4(-0.5,-0.5,0.5,1.0),
        vec4( 0.5,-0.5,0.5,1.0),
        vec4(0.5,0.5,0.5,1.0),
        vec4(-0.5,0.5,0.5,1.0),

        vec4(-0.5,-0.5,0.5,1.0),
        vec4(0.5,-0.5,0.5,1.0),
        vec4(0.0,0.5,0.5,1.0),

        vec4(-0.5,0.5,0.5,1.0),
        vec4(0.5,0.5,0.5,1.0),
        vec4(0.0,-0.5,0.5,1.0)

    );

    gl_Position = vertex[gl_VertexID];
    gl_Position.x = gl_Position.x*scale.x+offset.x;
    gl_Position.y = gl_Position.y*scale.y+offset.y;
}
