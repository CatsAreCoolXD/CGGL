#include "draw.h"
#include "style.h"
#include "window.h"
#include "shape.h"
#include "input.h"
#include "text.h"

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "include/stb_image.h"

/* 
VERTEX SHADER
This shader sets all the needed variables that the fragment shader uses. 
*/
const std::string VERTEX_SHADER_SOURCE = 
"#version 330 core\n"
"layout (location = 0) in vec3 pos;"
"layout (location = 1) in vec4 vertColor;"
"layout (location = 2) in vec2 texCoords;"
""
"out vec4 color;"
"out vec2 texCoord;"
""
"void main()"
"{"
"    gl_Position = vec4(pos, 1.0);"
"    color = vertColor;"
"    texCoord = texCoords;"
"}";

/* 
FRAGMENT SHADER
This shader draws the triangles based on the active rendering mode, which can either be:
(0) normal triangles,
(1) Textures
(2) Glyphs
*/
const std::string FRAGMENT_SHADER_SOURCE =
"#version 330 core\n"
"out vec4 FragColor;"
"in vec4 color;"
"in vec2 texCoord;"
"uniform sampler2D tex;"
""
"uniform int renderType;"
""
"void main()"
"{"
"    if (renderType == 0){"
"        FragColor = color;"
"    }"
"    else if (renderType == 1){"
"        vec4 texCol = texture(tex, texCoord);"
"        FragColor = color * texCol;"
"    }"
"    else if (renderType == 2){"
"        float alpha = texture(tex, texCoord).r;"
"        FragColor = vec4(color.rgb, color.a * alpha);"
"    }"
"}";

namespace cg {
    namespace {
        std::vector<cg::TriangleBuffer> triangleBuffers;

        bool useTexture = false;

        cg::Color backgroundColor(255, 255, 255);

        unsigned int framerateLimit = 60;
        double lastFrameTimePoint = glfwGetTime();
        double deltaTime = 0;
        std::vector<double> fpsList;

        cg::Shader shaderProgram;
        GLuint VAO, VBO, EBO;

        GLuint usage = GL_DYNAMIC_DRAW;

        std::vector<std::pair<int, int>> expectedVectorSizes;
    }

    void InitializeDrawing(){
        // Create the shader program
        shaderProgram.CreateFromStrings(VERTEX_SHADER_SOURCE, FRAGMENT_SHADER_SOURCE);
        
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        // Enable blending
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Push default style to style stack
        cg::PushStyle(cg::GetStyle());
    }

    double GetDeltatime(){
        return deltaTime;
    }

    double GetAverageFPS(){
        if (fpsList.empty()) return 0.0;
        double sum = 0.0;
        for (double fps : fpsList){
            sum += fps;
        }
        return sum / fpsList.size();
    }

    void SetFPSLimit(unsigned int fpsLimit){
        framerateLimit = fpsLimit;
    }

    void SetBackgroundColor(cg::Color newColor) {
        backgroundColor = newColor;
    }

    cg::Color GetBackgroundColor() {
        return backgroundColor;
    }

    void SetRenderingMode(int mode){
        glPolygonMode(GL_FRONT_AND_BACK, mode);
    }

    void SetUsageMode(int newUsage){
        usage = newUsage;
    }

    void NewFrame(){
        expectedVectorSizes.clear();
        expectedVectorSizes.resize(triangleBuffers.size(), {0, 0});
        for (int i = 0; i < triangleBuffers.size(); i++){
            cg::TriangleBuffer& buffer = triangleBuffers[i];
            expectedVectorSizes[i].first = buffer.GetVertices().size();
            expectedVectorSizes[i].second = buffer.GetIndices().size();
        }

        triangleBuffers.clear();

        glClearColor(colorToFloat(backgroundColor));
        glClear(GL_COLOR_BUFFER_BIT);

        // Check for events and update the window state
        glfwPollEvents();

        // Update input states
        cg::UpdateInputStates();

        // Max frames per second
        while (true){
            double now = glfwGetTime();
            if (now - lastFrameTimePoint > 1.0 / (double)framerateLimit)
                break;
        }

        double now = glfwGetTime();
        deltaTime = now - lastFrameTimePoint;
        lastFrameTimePoint = now;

        // Always have 1 second of data
        if (fpsList.size() > 1.0 / deltaTime){
            while (fpsList.size() > 1.0 / deltaTime - 1) fpsList.pop_back();
            fpsList.insert(fpsList.begin(), 1.0 / deltaTime);
        } else fpsList.insert(fpsList.begin(), 1.0 / deltaTime);

        useTexture = false;
    }

    size_t HashVertex(float* vertex, int size){
        size_t hash = 0;
        for (int i = 0; i < size; i++){
            uint32_t bits;
            memcpy(&bits, &vertex[i], sizeof(float));

            hash ^= bits * 0x9e3779b97f4a7c15ULL;
            hash = (hash << 13) | (hash >> 51);
        }
        return hash;
    }

    void PushNewTriangleBuffer(cg::Texture* tex, cg::Shader* shader){
        cg::TriangleBuffer& buffer = triangleBuffers.emplace_back();
        if (tex) {
            buffer.SetTexture(tex);
            useTexture = true;
        } else useTexture = false;

        if (shader) buffer.SetShader(shader);
        else buffer.SetShader(&shaderProgram);
        
        // Only reserve the vectors if it actually matters
        int i = triangleBuffers.size() - 1;
        if (expectedVectorSizes.size() >= triangleBuffers.size() && expectedVectorSizes[i].first > 100){
            buffer.GetVertices().reserve(expectedVectorSizes[i].first);
            buffer.GetIndices().reserve(expectedVectorSizes[i].second);
        }
    }

    cg::TriangleBuffer& GetActiveTriangleBuffer(){
        return triangleBuffers.back();
    }

    void PushTriangle(float* triangle){
        // Push a new triangle buffer into the list if its empty
        if (triangleBuffers.empty()) 
            cg::PushNewTriangleBuffer();

        cg::TriangleBuffer& buffer = triangleBuffers.back();
        std::vector<float>& verticesUsed = buffer.GetVertices();
        std::vector<unsigned int>& indicesUsed = buffer.GetIndices();
        auto& map = buffer.GetVerticesMap();
        int vertexSize = useTexture ? 9 : 7;
        for (int i = 0; i < 3; i++){
            float* vertex = &triangle[i * vertexSize];
            int verticesAmount = verticesUsed.size() / vertexSize;

            // Look if this vertex has already been added to the list of vertices
            int pos = 0;
            bool foundVertex = false;
            int indice = verticesAmount;

            // To balance performance and memory usage, only loop over the last 5 vertices since those are most likely to contain duplicates.
            // This reduces the amount of vertices by about 60%. If I were to loop over all vertices, 
            // memory usage would be reduced by about 70% in my tests, but performance would be bad.
            int start = std::max(verticesAmount - 5, 0);
            for (int j = start; j < verticesAmount; j++){
                bool equal = true;
                for (int k = 0; k < vertexSize; k++){
                    if (vertex[k] != verticesUsed[j * vertexSize + k]){
                        equal = false;
                        break;
                    }
                }
                if (equal) {
                    foundVertex = true;
                    indice = j;
                    break;
                }
            }
            if (!foundVertex) { // Vertex is not a duplicate, so add it to the list.
                verticesUsed.insert(verticesUsed.end(), vertex, vertex + vertexSize);
            }
            indicesUsed.push_back(indice);
        }
    }

    cg::TriangleBuffer& NewTriangleBuffer(){
        return triangleBuffers.emplace_back();
    }

    void DrawTriangles(cg::TriangleBuffer& buffer){
        cg::Shader& shader = *buffer.GetShader();
        if (!buffer.IsTexture()){
            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, buffer.GetVertices().size() * sizeof(float), buffer.GetVertices().data(), usage);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, buffer.GetIndices().size() * sizeof(unsigned int), buffer.GetIndices().data(), usage);

            // Tell OpenGL how it should interpret vertex data
            // Positions
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

            // Color
            glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3*sizeof(float)));
            glEnableVertexAttribArray(1);

            // Use the shader program
            shader.SetInt("renderType", NORMAL_RENDERING);
            shader.Use();

            // Draw the triangles
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, buffer.GetIndices().size(), GL_UNSIGNED_INT, 0);
        } else {
            glBindVertexArray(VAO);

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, buffer.GetVertices().size() * sizeof(float), buffer.GetVertices().data(), usage);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, buffer.GetIndices().size() * sizeof(unsigned int), buffer.GetIndices().data(), usage);

            // Tell OpenGL how it should interpret vertex data
            // Vertex
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

            // Color
            glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(1);

            // Texture coordinates
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(7 * sizeof(float)));
            glEnableVertexAttribArray(2);

            // Use the texture shader program
            shader.SetInt("renderType", (buffer.GetTexture()->IsGlyph() ? GLYPH_RENDERING : TEXTURE_RENDERING));
            shader.Use();

            // Bind the texture and draw the triangles
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, buffer.GetTexture()->textureId);
            glBindVertexArray(VAO);

            glDrawElements(GL_TRIANGLES, buffer.GetIndices().size(), GL_UNSIGNED_INT, 0);
        }
    }

    void Draw(){
        // Draw buffers
        for (cg::TriangleBuffer& buffer : triangleBuffers){
            cg::DrawTriangles(buffer);
        }

        glfwSwapBuffers(cg::GetWindow());
    }

    void Draw(cg::Texture& tex, int flags){
        cg::UseTexture(tex);
        cg::Rectangle(tex.GetOrigin(), tex.GetSize(), flags & ~FLAG_CENTERED);
        cg::StopUsingTexture();
    }

    void Draw(cg::Texture& tex, cg::Vec2f origin, cg::Vec2f size, int flags){
        tex.SetOrigin(origin);
        tex.SetSize(size);

        cg::Draw(tex, flags);
    }

    // Todo: Actually make 3d rendering, this doesnt really work
    void Draw(cg::Scene& scene, cg::Vec3f cameraPos){
        cg::PushNewTriangleBuffer();
        std::vector<cg::TriangleObject> triangles = scene.GetTriangles();

        for (cg::TriangleObject triangle : triangles){
            cg::Material mat = scene.GetMaterials()[triangle.materialIndex];
            float t[21];

            for (int i = 0; i < 3; i++) {
                t[i * 7 + 3] = mat.color[0];
                t[i * 7 + 4] = mat.color[1];
                t[i * 7 + 5] = mat.color[2];
                t[i * 7 + 6] = 1.f;
            }

            t[0] = triangle.p1[0] - cameraPos.x;
            t[1] = triangle.p1[1] - cameraPos.y;
            t[2] = triangle.p1[2] - cameraPos.z;

            t[7] = triangle.p2[0] - cameraPos.x;
            t[8] = triangle.p2[1] - cameraPos.y;
            t[9] = triangle.p2[2] - cameraPos.z;

            t[14] = triangle.p3[0] - cameraPos.x;
            t[15] = triangle.p3[1] - cameraPos.y;
            t[16] = triangle.p3[2] - cameraPos.z;

            cg::PushTriangle(t);
        }
    }

    void DrawFullscreenShader(cg::Shader* shader, cg::Texture* tex){
        shader->SetFloats("resolution", cg::Vec2f(cg::GetWindowSize()));

        cg::PushNewTriangleBuffer(tex, shader);
        cg::Rectangle(cg::Vec2f(0,0), cg::GetWindowSize(), FLAG_NO_BORDER); // Draw a full-screen quad
        cg::StopUsingTexture();
    }

    int GetVerticesAmount(){
        int amount = 0;
        for (auto& b : triangleBuffers){
            amount += b.GetVertices().size();
        }
        return amount;
    }
    int GetIndicesAmount(){
        int amount = 0;
        for (auto& b : triangleBuffers){
            amount += b.GetIndices().size();
        }
        return amount;
    }
    int GetTexturesAmount(){
        int amount = 0;
        for (auto& b : triangleBuffers){
            amount += b.IsTexture();
        }
        return amount;
    }

    void UseTexture(cg::Texture& tex){
        useTexture = true;
        cg::PushNewTriangleBuffer(&tex, &shaderProgram);
    }

    void StopUsingTexture(){
        useTexture = false;
        cg::PushNewTriangleBuffer();
    }

    bool IsUsingTexture() { 
        return useTexture; 
    }

    cg::Vec2f GetTextureOrigin() { 
        return triangleBuffers.back().GetTexture()->GetOriginNDC(); 
    }

    cg::Vec2f GetTextureSize() { 
        return triangleBuffers.back().GetTexture()->GetSizeNDC(); 
    }

    cg::Color GetTextureTint() { 
        return triangleBuffers.back().GetTexture()->GetTint(); 
    }

    cg::Vec2f GetTextureFlip() { 
        return triangleBuffers.back().GetTexture()->flip; 
    }

    cg::Vec2f GetTextureOffset() { 
        return triangleBuffers.back().GetTexture()->offset; 
    }

    cg::Texture* GetCurrentTexture(){
        return triangleBuffers.back().GetTexture();
    }

    GLuint GetUsage(){
        return usage;
    }

    Texture::Texture(const char* path, int wrapping) {
        Texture::LoadImage(path, wrapping);
    }

    void UseShaderProgram(){
        shaderProgram.Use();
    }

    void Texture::LoadImage(const char* path, int wrapping){
        // Load the file and get it's data using stb_image.h
        unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);

        if (data){
            // Create and bind the texture
            glGenTextures(1, &textureId);
            glBindTexture(GL_TEXTURE_2D, textureId);

            // Set the texture wrapping/filtering options
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapping);	
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapping);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            // Generate the texture
            int colorType = nrChannels == 4 ? GL_RGBA : GL_RGB;
            glTexImage2D(GL_TEXTURE_2D, 0, colorType, width, height, 0, colorType, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        } else {
            throw std::runtime_error("Failed to load texture. Ensure path is correct.");
        }

        size.x = width;
        size.y = height;

        // Calculate NDC
        Texture::SetSize(size);
        Texture::SetOrigin(cg::Vec2f(0,0));

        stbi_image_free(data);

        tint = cg::Color(255, 255, 255);
    }

    void Texture::LoadGlyph(FT_Face& face){
        // Create and bind the texture
        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);

        // Set the texture wrapping/filtering options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        width = face->glyph->bitmap.width;
        height = face->glyph->bitmap.rows;

        // Generate the texture
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

        size.x = width;
        size.y = height;

        // Calculate NDC
        Texture::SetSize(size);
        Texture::SetOrigin(cg::Vec2f(0,0));

        tint = cg::Color(255, 255, 255);
        isGlyph = true;
    }

    void Texture::SetOrigin(cg::Vec2f pos, bool centered){
        if (centered) pos = pos - cg::Vec2f(size.x / 2.f, size.y / 2.f);
        origin = pos;

        // Convert pos to NDC
        int w, h;
        glfwGetWindowSize(cg::GetWindow(), &w, &h);

        originNDC.x = (pos.x / w - 0.5f) * 2.f;
        originNDC.y = (pos.y / h - 0.5f) * 2.f;
    }

    void Texture::SetSize(cg::Vec2f newSize){
        size.x = newSize.x;
        size.y = newSize.y;

        // Convert size to NDC
        int w, h;
        glfwGetWindowSize(cg::GetWindow(), &w, &h);

        sizeNDC.x = (newSize.x / w) * 2.f;
        sizeNDC.y = (newSize.y / h) * 2.f;
    }

    void Texture::Scale(float scale){
        Texture::SetSize(size * scale);
    }
}