/*
Copyright 2017 Google Inc. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS-IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include "lullaby/systems/render/next/gl_helpers.h"

#include "fplbase/renderer.h"

namespace lull {
// Predefined attributes supported in shaders.
enum {
  kAttribPosition,
  kAttribNormal,
  kAttribTangent,
  kAttribOrientation,
  kAttribTexCoord,
  kAttribTexCoord1,
  kAttribTexCoord2,
  kAttribTexCoord3,
  kAttribTexCoord4,
  kAttribTexCoord5,
  kAttribTexCoord6,
  kAttribTexCoord7,
  kAttribColor,
  kAttribBoneIndices,
  kAttribBoneWeights,
  kAttribInvalid,

  kAttribTexCoordMax = kAttribTexCoord7,
};

GLenum GetGlInternalFormat(DepthStencilFormat format) {
  switch (format) {
    case DepthStencilFormat_None:
      return GL_NONE;
    case DepthStencilFormat_Depth16:
      return GL_DEPTH_COMPONENT16;
    case DepthStencilFormat_Depth24:
      return GL_DEPTH_COMPONENT24;
    case DepthStencilFormat_Depth32F:
      return GL_DEPTH_COMPONENT32F;
    case DepthStencilFormat_Depth24Stencil8:
      return GL_DEPTH24_STENCIL8;
    case DepthStencilFormat_Depth32FStencil8:
      return GL_DEPTH32F_STENCIL8;
    case DepthStencilFormat_Stencil8:
      return GL_STENCIL_INDEX8;
    default:
      LOG(DFATAL) << "Unknown depth stencil format: " << format;
      return GL_NONE;
  }
}

GLenum GetGlInternalFormat(TextureFormat format) {
  switch (format) {
    case TextureFormat_None:
      return GL_NONE;
    case TextureFormat_A8:
      return GL_ALPHA;
    case TextureFormat_R8:
      return GL_RGB;
    case TextureFormat_RGB8:
      return GL_RGB;
    case TextureFormat_RGBA8:
      return GL_RGBA;
    case TextureFormat_Depth16:
      return GL_DEPTH_COMPONENT16;
    case TextureFormat_Depth32F:
      return GL_DEPTH_COMPONENT32F;
    default:
      LOG(DFATAL) << "Unknown texture format: " << format;
      return GL_NONE;
  }
}

GLenum GetGlFormat(TextureFormat format) {
  switch (format) {
    case TextureFormat_None:
      return GL_NONE;
    case TextureFormat_A8:
      return GL_ALPHA;
    case TextureFormat_R8:
#ifdef FPLBASE_GLES
      return GL_RGB;  // For GLES2, the format must match internalformat.
#else
      return GL_RED;
#endif
    case TextureFormat_RGB8:
      return GL_RGB;
    case TextureFormat_RGBA8:
      return GL_RGBA;
    case TextureFormat_Depth16:
      return GL_DEPTH_COMPONENT;
    case TextureFormat_Depth32F:
      return GL_DEPTH_COMPONENT;
    default:
      LOG(DFATAL) << "Unknown format: " << format;
      return GL_NONE;
  }
}

GLenum GetGlType(TextureFormat format) {
  switch (format) {
    case TextureFormat_None:
      return GL_NONE;
    case TextureFormat_A8:
      return GL_UNSIGNED_BYTE;
    case TextureFormat_R8:
      return GL_UNSIGNED_BYTE;
    case TextureFormat_RGB8:
      return GL_UNSIGNED_BYTE;
    case TextureFormat_RGBA8:
      return GL_UNSIGNED_BYTE;
    case TextureFormat_Depth16:
      return GL_UNSIGNED_SHORT;
    case TextureFormat_Depth32F:
      return GL_FLOAT;
    default:
      LOG(DFATAL) << "Unknown texture type: " << format;
      return GL_NONE;
  }
}

GLenum GetGlTextureFiltering(TextureFiltering filtering) {
  switch (filtering) {
    case TextureFiltering_Nearest:
      return GL_NEAREST;
    case TextureFiltering_Linear:
      return GL_LINEAR;
    case TextureFiltering_NearestMipmapNearest:
      return GL_NEAREST_MIPMAP_NEAREST;
    case TextureFiltering_LinearMipmapNearest:
      return GL_LINEAR_MIPMAP_NEAREST;
    case TextureFiltering_NearestMipmapLinear:
      return GL_NEAREST_MIPMAP_LINEAR;
    case TextureFiltering_LinearMipmapLinear:
      return GL_LINEAR_MIPMAP_LINEAR;
    default:
      LOG(DFATAL) << "Unknown texture filtering: " << filtering;
      return GL_NEAREST;
  }
}

GLenum GetGlTextureWrap(TextureWrap wrap) {
  switch (wrap) {
    case TextureWrap_ClampToBorder:
#ifdef GL_CLAMP_TO_BORDER
      return GL_CLAMP_TO_BORDER;
#else
      LOG(ERROR) << "TextureWrap_ClampToBorder is not supported.";
      return GL_CLAMP_TO_EDGE;
#endif
    case TextureWrap_ClampToEdge:
      return GL_CLAMP_TO_EDGE;
    case TextureWrap_MirroredRepeat:
      return GL_MIRRORED_REPEAT;
    case TextureWrap_MirrorClampToEdge:
      LOG(ERROR) << "TextureWrap_MirrorClampToEdge is not supported.";
      return GL_CLAMP_TO_EDGE;
    case TextureWrap_Repeat:
      return GL_REPEAT;
    default:
      LOG(DFATAL) << "Unknown texture wrap mode: " << wrap;
      return GL_REPEAT;
  }
}

GLenum GetGlPrimitiveType(MeshData::PrimitiveType type) {
  switch (type) {
    case MeshData::kPoints:
      return GL_POINTS;
    case MeshData::kLines:
      return GL_LINES;
    case MeshData::kTriangles:
      return GL_TRIANGLES;
    case MeshData::kTriangleFan:
      return GL_TRIANGLE_FAN;
    case MeshData::kTriangleStrip:
      return GL_TRIANGLE_STRIP;
    default:
      LOG(DFATAL) << "Unknown index type: " << type;
      return GL_UNSIGNED_BYTE;
  }
}

GLenum GetGlVertexType(VertexAttributeType type) {
  switch (type) {
    case VertexAttributeType_Scalar1f:
    case VertexAttributeType_Vec2f:
    case VertexAttributeType_Vec3f:
    case VertexAttributeType_Vec4f:
      return GL_FLOAT;
    case VertexAttributeType_Vec2us:
    case VertexAttributeType_Vec4us:
      return GL_UNSIGNED_SHORT;
    case VertexAttributeType_Vec4ub:
      return GL_UNSIGNED_BYTE;
    default:
      LOG(DFATAL) << "Unknown vertex attribute type.";
      return GL_UNSIGNED_BYTE;
  }
}

GLint GetNumElementsInVertexType(VertexAttributeType type) {
  switch (type) {
    case VertexAttributeType_Scalar1f:
      return 1;
    case VertexAttributeType_Vec2f:
      return 2;
    case VertexAttributeType_Vec3f:
      return 3;
    case VertexAttributeType_Vec4f:
      return 4;
    case VertexAttributeType_Vec2us:
      return 2;
    case VertexAttributeType_Vec4us:
      return 4;
    case VertexAttributeType_Vec4ub:
      return 4;
    default:
      LOG(DFATAL) << "Unknown vertex attribute type.";
      return 0;
  }
}

GLenum GetGlIndexType(MeshData::IndexType type) {
  switch (type) {
    case MeshData::kIndexU16:
      return GL_UNSIGNED_SHORT;
    case MeshData::kIndexU32:
      return GL_UNSIGNED_INT;
    default:
      LOG(DFATAL) << "Unknown index type: " << type;
      return GL_UNSIGNED_BYTE;
  }
}

ShaderProfile GetShaderProfile() {
#ifdef FPLBASE_GLES
  return ShaderProfile::Gles;
#else
  return ShaderProfile::Core;
#endif
}

int GlMaxVertexUniformComponents() {
  return fplbase::RendererBase::Get()->max_vertex_uniform_components();
}

bool GlSupportsVertexArrays() {
  return fplbase::RendererBase::Get()->feature_level() >=
         fplbase::kFeatureLevel30;
}

bool GlSupportsTextureNpot() {
  return fplbase::RendererBase::Get()->SupportsTextureNpot();
}

bool GlSupportsAstc() {
  return fplbase::RendererBase::Get()->SupportsTextureFormat(
      fplbase::kFormatASTC);
}

bool GlSupportsEtc2() {
  // FPLBase uses ktx (or pkm) to store etc2 data.
  return fplbase::RendererBase::Get()->SupportsTextureFormat(
      fplbase::kFormatKTX);
}

void SetVertexAttributes(const VertexFormat& vertex_format,
                         const uint8_t* buffer) {
  int tex_coord_count = 0;
  const int stride = static_cast<int>(vertex_format.GetVertexSize());
  for (size_t i = 0; i < vertex_format.GetNumAttributes(); ++i) {
    const VertexAttribute* attrib = vertex_format.GetAttributeAt(i);

    const GLenum gl_type = GetGlVertexType(attrib->type());
    const GLint count = GetNumElementsInVertexType(attrib->type());
    if (count == 0) {
      continue;
    }

    int location = kAttribInvalid;
    bool normalized = false;
    switch (attrib->usage()) {
      case VertexAttributeUsage_Position:
        location = kAttribPosition;
        break;
      case VertexAttributeUsage_Normal:
        location = kAttribNormal;
        break;
      case VertexAttributeUsage_Tangent:
        location = kAttribTangent;
        break;
      case VertexAttributeUsage_Orientation:
        location = kAttribOrientation;
        break;
      case VertexAttributeUsage_Color:
        location = kAttribColor;
        normalized = true;
        break;
      case VertexAttributeUsage_BoneIndices:
        location = kAttribBoneIndices;
        break;
      case VertexAttributeUsage_BoneWeights:
        location = kAttribBoneWeights;
        normalized = true;
        break;
      case VertexAttributeUsage_TexCoord:
        DCHECK(kAttribTexCoord + tex_coord_count < kAttribTexCoordMax);
        location = kAttribTexCoord + tex_coord_count;
        normalized = (gl_type != GL_FLOAT);
        ++tex_coord_count;
        break;
      default:
        break;
    }

    if (location != kAttribInvalid) {
      GL_CALL(glEnableVertexAttribArray(location));
      GL_CALL(glVertexAttribPointer(location, count, gl_type,
                                    normalized ? GL_TRUE : GL_FALSE, stride,
                                    buffer));
    }
    buffer += VertexFormat::GetAttributeSize(*attrib);
  }
}

void UnsetVertexAttributes(const VertexFormat& vertex_format) {
  int tex_coord_count = 0;
  for (size_t i = 0; i < vertex_format.GetNumAttributes(); ++i) {
    const VertexAttribute* attrib = vertex_format.GetAttributeAt(i);
    switch (attrib->usage()) {
      case VertexAttributeUsage_Position:
        GL_CALL(glDisableVertexAttribArray(kAttribPosition));
        break;
      case VertexAttributeUsage_Normal:
        GL_CALL(glDisableVertexAttribArray(kAttribNormal));
        break;
      case VertexAttributeUsage_Tangent:
        GL_CALL(glDisableVertexAttribArray(kAttribTangent));
        break;
      case VertexAttributeUsage_Orientation:
        GL_CALL(glDisableVertexAttribArray(kAttribOrientation));
        break;
      case VertexAttributeUsage_Color:
        GL_CALL(glDisableVertexAttribArray(kAttribColor));
        break;
      case VertexAttributeUsage_BoneIndices:
        GL_CALL(glDisableVertexAttribArray(kAttribBoneIndices));
        break;
      case VertexAttributeUsage_BoneWeights:
        GL_CALL(glDisableVertexAttribArray(kAttribBoneWeights));
        break;
      case VertexAttributeUsage_TexCoord:
        DCHECK(kAttribTexCoord + tex_coord_count < kAttribTexCoordMax);
        GL_CALL(glDisableVertexAttribArray(kAttribTexCoord + tex_coord_count));
        ++tex_coord_count;
        break;
      default:
        break;
    }
  }
}

void DrawMeshData(const MeshData& mesh_data) {
  const VertexFormat& vertex_format = mesh_data.GetVertexFormat();
  const int num_vertices = static_cast<int>(mesh_data.GetNumVertices());
  const int num_indices = static_cast<int>(mesh_data.GetNumIndices());
  const uint8_t* vertices = num_vertices ? mesh_data.GetVertexBytes() : nullptr;
  const uint8_t* indices = num_indices ? mesh_data.GetIndexBytes() : nullptr;

  if (num_vertices == 0) {
    return;
  }
  if (vertices == nullptr) {
    LOG(DFATAL) << "Can't draw mesh without vertex read access.";
    return;
  }

  GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
  GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
  const GLenum gl_mode = GetGlPrimitiveType(mesh_data.GetPrimitiveType());

  SetVertexAttributes(vertex_format, vertices);
  if (num_indices > 0 && indices != nullptr) {
    const GLenum gl_type = GetGlIndexType(mesh_data.GetIndexType());
    GL_CALL(glDrawElements(gl_mode, num_indices, gl_type, indices));
  } else {
    const GLenum gl_mode = GetGlPrimitiveType(mesh_data.GetPrimitiveType());
    GL_CALL(glDrawArrays(gl_mode, 0, num_vertices));
  }
  UnsetVertexAttributes(vertex_format);
}

void DrawQuad(const mathfu::vec3& bottom_left, const mathfu::vec3& top_right,
              const mathfu::vec2& tex_bottom_left,
              const mathfu::vec2& tex_top_right) {
  const VertexPT vertices[] = {
      {bottom_left.x, bottom_left.y, bottom_left.z, tex_bottom_left.x,
       tex_bottom_left.y},
      {bottom_left.x, top_right.y, top_right.z, tex_bottom_left.x,
       tex_top_right.y},
      {top_right.x, bottom_left.y, bottom_left.z, tex_top_right.x,
       tex_bottom_left.y},
      {top_right.x, top_right.y, top_right.z, tex_top_right.x, tex_top_right.y},
  };
  static const uint16_t indices[] = {0, 1, 2, 1, 3, 2};
  static const int kNumIndices = sizeof(indices) / sizeof(indices[0]);

  GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
  GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
  SetVertexAttributes(VertexPT::kFormat,
                      reinterpret_cast<const uint8_t*>(vertices));
  GL_CALL(
      glDrawElements(GL_TRIANGLES, kNumIndices, GL_UNSIGNED_SHORT, indices));
  UnsetVertexAttributes(VertexPT::kFormat);
}

Span<std::pair<const char*, int>> GetDefaultVertexAttributes() {
  static const std::pair<const char*, int> kDefaultAttributes[] = {
      {"aPosition", kAttribPosition},
      {"aNormal", kAttribNormal},
      {"aTangent", kAttribTangent},
      {"aOrientation", kAttribOrientation},
      {"aTexCoord", kAttribTexCoord},
      {"aTexCoordAlt", kAttribTexCoord1},
      {"aColor", kAttribColor},
      {"aBoneIndices", kAttribBoneIndices},
      {"aBoneWeights", kAttribBoneWeights}};

  return Span<std::pair<const char*, int>>(
      kDefaultAttributes,
      sizeof(kDefaultAttributes) / sizeof(kDefaultAttributes[0]));
}
}  // namespace lull
