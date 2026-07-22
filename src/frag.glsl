#version 330 core
out vec4 FragColor;
in vec3 color;
in vec2 texCoord;
uniform sampler2D tex;

uniform int renderType;

void main()
{
    if (renderType == 0){ // Triangles without a texture
        FragColor = vec4(color, 1.0);
    } 
    else if (renderType == 1){ // Texture
        vec4 texCol = texture(tex, texCoord);
        FragColor = vec4(color, 1.0) * texCol;
    } 
    else if (renderType == 2){ // Glyph
        float alpha = texture(tex, texCoord).r;
        FragColor = vec4(color, alpha);
    }
}