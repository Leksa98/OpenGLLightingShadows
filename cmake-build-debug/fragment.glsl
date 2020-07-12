#version 330 core

in vec2 TexCoord;

out vec4 color;

uniform sampler2D ourTexture;
uniform int hasTexture;
uniform vec3 ourColor;

void main()
{
    if (hasTexture != 0) {
        color = texture(ourTexture, TexCoord);
    } else {
        color = vec4(ourColor, 0.0f);
    }

}