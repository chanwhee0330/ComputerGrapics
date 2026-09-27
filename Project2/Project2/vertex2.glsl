#version 330 core

uniform vec2 offset;
uniform float scale;
uniform float angle;

void main()
{
   const vec4 vertex[4] = vec4[4]
    (
        // 삼각형: 0~2번 정점
        vec4(-0.08, -0.075, 0.5, 1.0),
        vec4( 0.08, -0.075, 0.5, 1.0),
        vec4( 0.0,   0.15,  0.5, 1.0),

        // 스파이럴 경로: 3번 정점
        vec4(0.0, 0.0, 0.5, 1.0)
    );

    gl_Position = vertex[gl_VertexID];

    float radian = radians(angle);

    float originalX = gl_Position.x * scale;
    float originalY = gl_Position.y * scale; 

    gl_Position.x = originalX * cos(radian) - originalY * sin(radian) + offset.x;
    gl_Position.y = originalX * sin(radian) + originalY * cos(radian) + offset.y;
}
