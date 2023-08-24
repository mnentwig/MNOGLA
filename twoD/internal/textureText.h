#pragma once
#include <glm/mat3x3.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <iostream>
#include <string>

#include "../../MNOGLA.h"  // freetype
#include "../../core/MNOGLA_util.h"
#include "fontAtlas.h"
#ifndef MHPP
#define MHPP(arg)
#endif
namespace MNOGLA {
using ::std::string, ::glm::vec2, ::glm::vec3, ::glm::mat3, ::std::unique_ptr, ::std::make_unique;
class textureText {
    MHPP("begin textureText") // === autogenerated code. Do not edit ===
    public:
    	static void glInit();
    	static void glDeinit();
    	static void draw(const glm::vec2& pt, const string& text, const mat3& world2screen, const vec3& rgb);
    protected:
    	static bool nextChar(std::string::const_iterator& iBegin, const std::string::const_iterator iEnd, uint32_t& outUTF32);
    	static GLuint p0;
    	static GLuint p0_rgb;
    	static GLuint p0_vertex;
    	static GLuint p0_world2NDC;
    	static bool canClean;
    	static unique_ptr<MNOGLA::fontAtlas> pAtlas;
    	static GLuint charmapTexture;
    MHPP("end textureText")
   protected:
    float fontsize;
};

void glInit_textureText();
void glDeinit_textureText();

}  // namespace MNOGLA