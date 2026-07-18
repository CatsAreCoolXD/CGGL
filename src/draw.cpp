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
#include "stb_image.h"

namespace cg {
    namespace {
        std::vector<float> vertices;
        std::vector<unsigned int> indices;

        std::vector<std::vector<float>> textureVertices;
        std::vector<std::vector<unsigned int>> textureIndices;
        std::vector<cg::Texture*> textures;

        bool useTexture = false;

        cg::Color backgroundColor(255, 255, 255);

        unsigned int framerateLimit = 60;
        double lastFrameTimePoint = glfwGetTime();
        double deltaTime = 0;
        std::vector<double> fpsList;

        cg::Shader shaderProgram;
        GLuint VAO, VBO, EBO;
    }

    void InitializeDrawing(){
        // Create the shader program
        shaderProgram.Create("src/vert.glsl", "src/frag.glsl");
        
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        // Enable blending
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Push default style to style stack
        cg::Style::PushStyle(cg::Style::GetStyle());
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

    void SetRenderingMode(int mode){
        glPolygonMode(GL_FRONT_AND_BACK, mode);
    }

    void NewFrame(){
        // Clear the vertexes and indices and allocate the same amount of memory, 
        // since the amount of memory used probably wont change much in between frames.
        int previousSize = vertices.size();
        vertices.clear();
        vertices.reserve(previousSize);
        
        previousSize = indices.size();
        indices.clear();
        indices.reserve(previousSize);

        textureVertices.clear();
        textureIndices.clear();
        textures.clear();

        glClearColor(colorToFloat(backgroundColor));
        glClear(GL_COLOR_BUFFER_BIT);

        // Check for events and update the window state
        glfwPollEvents();

        // Update input states
        cg::Input::UpdateInputStates();

        // Max frames per second
        while (true){
            double now = glfwGetTime();
            if (now - lastFrameTimePoint > 1.f / (float)framerateLimit)
                break;
        }

        double now = glfwGetTime();
        deltaTime = now - lastFrameTimePoint;
        lastFrameTimePoint = now;

        if (fpsList.size() > 30){
            fpsList.pop_back();
            fpsList.insert(fpsList.begin(), 1.0 / deltaTime);
        } else fpsList.push_back(1.0 / deltaTime);
    }

    void PushTriangle(float* triangle){
        std::vector<float>& verticesUsed = useTexture ? textureVertices.back() : vertices;
        std::vector<unsigned int>& indicesUsed = useTexture ? textureIndices.back() : indices;
        int vertexSize = useTexture ? 8 : 6;
        for (int i = 0; i < 3; i++){
            float* vertex = &triangle[i * vertexSize];

            int indice = verticesUsed.size() / vertexSize;

            // Look if this vertex has already been added to the list of vertices
            int pos = 0;
            bool foundVertex = false;
            for (int j = 0; j < verticesUsed.size() / vertexSize; j++){
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

    void DrawTriangles(std::vector<float> verticesToDraw, std::vector<unsigned int> indicesToDraw, int texture){
        if (texture == -1){
            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, verticesToDraw.size() * sizeof(float), verticesToDraw.data(), GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesToDraw.size() * sizeof(unsigned int), indicesToDraw.data(), GL_STATIC_DRAW);

            // Tell OpenGL how it should interpret vertex data
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3*sizeof(float)));
            glEnableVertexAttribArray(1);

            // Use the shader program
            shaderProgram.SetInt("renderType", NORMAL_RENDERING);
            shaderProgram.Use();

            // Draw the triangles
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, indicesToDraw.size(), GL_UNSIGNED_INT, 0);
        } else {
            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, verticesToDraw.size() * sizeof(float), verticesToDraw.data(), GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesToDraw.size() * sizeof(unsigned int), indicesToDraw.data(), GL_STATIC_DRAW);

            // Tell OpenGL how it should interpret vertex data
            // Vertex
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

            // Color
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(1);

            // Texture coordinates
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
            glEnableVertexAttribArray(2);

            // Use the texture shader program
            shaderProgram.SetInt("renderType", (textures[texture]->IsGlyph() ? GLYPH_RENDERING : TEXTURE_RENDERING));
            shaderProgram.Use();

            // Bind the texture and draw the triangles
            glBindTexture(GL_TEXTURE_2D, textures[texture]->textureId);
            glBindVertexArray(VAO);

            glDrawElements(GL_TRIANGLES, indicesToDraw.size(), GL_UNSIGNED_INT, 0);
        }
    }

    void Draw(){
        cg::DrawTriangles(vertices, indices);

        // Draw textures
        for (int i = 0; i < textures.size(); i++){
            cg::DrawTriangles(textureVertices[i], textureIndices[i], i);
        }

        glfwSwapBuffers(cg::GetWindow());
    }

    void Draw(cg::Texture& tex, int flags){
        cg::UseTexture(tex);
        cg::Rectangle(tex.GetOrigin(), tex.GetSize(), flags & ~FLAG_CENTERED);
        cg::StopUsingTexture();
    }

    void Draw(cg::Texture& tex, cg::Vec2f size, int flags){
        tex.SetSize(size);

        cg::Draw(tex, flags);
    }

    void Draw(cg::Texture& tex, cg::Vec2i origin, int flags){
        tex.SetOrigin(origin);

        cg::Draw(tex, flags);
    }

    void Draw(cg::Texture& tex, cg::Vec2f origin, cg::Vec2f size, int flags){
        tex.SetOrigin(origin);
        tex.SetSize(size);

        cg::Draw(tex, flags);
    }

    void UseTexture(cg::Texture& tex){
        useTexture = true;
        textures.push_back(&tex);
        textureVertices.push_back({});
        textureIndices.push_back({});
    }

    void StopUsingTexture(){
        useTexture = false;
    }

    bool IsUsingTexture() { 
        return useTexture; 
    }

    cg::Vec2f GetTextureOrigin() { 
        return textures.back()->GetOriginNDC(); 
    }

    cg::Vec2f GetTextureSize() { 
        return textures.back()->GetSizeNDC(); 
    }

    cg::Color GetTextureTint() { 
        return textures.back()->GetTint(); 
    }

    cg::Vec2f GetTextureFlip() { 
        return textures.back()->flip; 
    }

    cg::Texture* GetCurrentTexture(){
        return textures.back();
    }

    Texture::Texture(const char* path, int wrapping) {
        Texture::LoadImage(path, wrapping);
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