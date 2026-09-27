#version 330 core

uniform vec2 offset;

void main()
{
   const vec4 vertex[10] = vec4[10]
    (
        // 점: 0번 정점
        vec4( 0.0,  0.0, 0.5, 1.0),

        // 선: 1~2번 정점
        vec4(-0.1,  0.0, 0.5, 1.0),
        vec4( 0.1,  0.0, 0.5, 1.0),

        // 삼각형: 3~5번 정점
        vec4(-0.1, -0.1, 0.5, 1.0),
        vec4( 0.1, -0.1, 0.5, 1.0),
        vec4( 0.0,  0.1, 0.5, 1.0),

        // 사각형: 6~9번 정점
        vec4(-0.1, -0.1, 0.5, 1.0),
        vec4( 0.1, -0.1, 0.5, 1.0),
        vec4(-0.1,  0.1, 0.5, 1.0),
        vec4( 0.1,  0.1, 0.5, 1.0)
    );

    gl_Position = vertex[gl_VertexID];
    gl_Position.x += offset.x;
    gl_Position.y += offset.y;
}
