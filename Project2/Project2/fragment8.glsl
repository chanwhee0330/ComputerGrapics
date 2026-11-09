#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec3 fragmentColor;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform bool useLighting;

out vec4 color;

void main()
{
    // 궤도선은 조명 계산 없이 지정한 색으로 표시한다.
    if (!useLighting)
    {
        color = vec4(fragmentColor, 1.0);
        return;
    }

    // 빛을 등진 면도 조금 보이게 하는 기본 밝기
    vec3 ambient = 0.2 * lightColor;

    vec3 N = normalize(Normal);
    vec3 L = normalize(lightPos - FragPos);

    // 표면이 광원을 향할수록 밝아짐
    float diffuseAmount = max(dot(N, L), 0.0);
    vec3 diffuse = 0.8 * diffuseAmount * lightColor;

    vec3 result = (ambient + diffuse) * fragmentColor;

    color = vec4(result, 1.0);
}
