#pragma once

#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include "bx/math.h"
#include <bx/bx.h>

#include "Cube.hpp"

struct PosColorTexVertex
{
    float x;
    float y;
    float z;
    uint32_t abgr;
    float texu;
    float texv;
};

static const struct PosColorTexVertex SpriteVertices[] =
    {
        {-1.0f,  1.0f,  1.0f, 0xff000000, 0, 0},
        { 1.0f,  1.0f,  1.0f, 0xff000000, 1, 0},
        {-1.0f, -1.0f,  1.0f, 0xff000000, 0, 1},
        { 1.0f, -1.0f,  1.0f, 0xff000000, 1, 1}
    };

static const uint16_t SpriteTriList[] =
    {
        0, 2, 1,
        1, 2, 3
    };

class Sprite
{
public:
    Sprite() {
        cube_vertexDeclaration(&vbh, &ibh);
        vsh = loadShader("vs_cubes.bin");
        fsh = loadShader("fs_cubes.bin");
        program = bgfx::createProgram(vsh, fsh, true);
        counter = 0;
    }

    void render() {
            float view[16];
            bx::mtxLookAt(view, eye, at);
            float proj[16];

            const float offset = 0.1f; // TODO: not sure
            const bool homogeneousNdc = bgfx::getCaps()->homogeneousDepth;
            bx:mtxOrtho(proj, 0.0f, 800.f, 600.f, 0.0f, 1.0f, -1.0f, offset, homogeneousNdc, bx::Handness::Left);

            bgfx::setViewTransform(0, view, proj);

            // Rotate:
            //float mtx[16];
            //bx::mtxRotateXY(mtx, counter * 0.01f, counter * 0.01f);
            //bgfx::setTransform(mtx);        

            bgfx::setVertexBuffer(0, vbh);
            bgfx::setIndexBuffer(ibh);

            bgfx::submit(0, program);
            counter ++;
    }

private:

    void cube_vertexDeclaration(bgfx::VertexBufferHandle *pvbh, bgfx::IndexBufferHandle *pibh)
    {
        bgfx::VertexLayout pcvDecl;
        pcvDecl.begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
        .end();
        *pvbh = bgfx::createVertexBuffer(bgfx::makeRef(SpriteVertices, sizeof(SpriteVertices)), pcvDecl);
        *pibh = bgfx::createIndexBuffer(bgfx::makeRef(SpriteTriList, sizeof(SpriteTriList)));
    }

    bgfx::ShaderHandle loadShader(const char *FILENAME)
    {
        const char* shaderDir = "???";
        std::string filePath = "";

        switch(bgfx::getRendererType()) {
            case bgfx::RendererType::Noop:
            case bgfx::RendererType::Direct3D9:  shaderDir = "shaders/dx9/";   break;
            case bgfx::RendererType::Direct3D11:
            case bgfx::RendererType::Direct3D12: shaderDir = "shaders/dx11/";  break;
            case bgfx::RendererType::Gnm:        shaderDir = "shaders/pssl/";  break;
            case bgfx::RendererType::Metal:      shaderDir = "shaders/metal/"; break;
            case bgfx::RendererType::OpenGL:     shaderDir = "shaders/glsl/";  break;
            case bgfx::RendererType::OpenGLES:   shaderDir = "shaders/essl/";  break;
            case bgfx::RendererType::Vulkan:     shaderDir = "shaders/spirv/"; break;
        }
        filePath.append(shaderDir);
        filePath.append(FILENAME);

        FILE *file = fopen(filePath.c_str(), "rb");
        if (file == NULL) {
            std::cout<<"Error: Cannot load shader "<<filePath<<std::endl;
            exit(1); // TODO: Free resources or do a proper error handling
        }

        // Get the file's size to alloc the memory
        fseek(file, 0, SEEK_END);
        long fileSize = ftell(file);
        fseek(file, 0, SEEK_SET);

        const bgfx::Memory *mem = bgfx::alloc(fileSize + 1);
        fread(mem->data, 1, fileSize, file);
        mem->data[mem->size - 1] = '\0';
        fclose(file);
        // TODO: Do I need to clean the memory ?

        return bgfx::createShader(mem);
    }
    bgfx::VertexBufferHandle vbh;
    bgfx::IndexBufferHandle ibh;
    bgfx::ProgramHandle program;
    bgfx::ShaderHandle vsh;
    bgfx::ShaderHandle fsh;
    const bx::Vec3 at = {0.0f, 0.0f,  0.0f};
    const bx::Vec3 eye = {0.0f, 0.0f, -5.0f};
    unsigned int counter;
};