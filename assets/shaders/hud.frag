#version 330 core
in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D text;
uniform vec3 color;

void main()
{
    vec4 sampled = texture(text, TexCoords);
    FragColor = sampled * vec4(color, 1.0);
}
