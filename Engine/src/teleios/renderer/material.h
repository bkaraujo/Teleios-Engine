#ifndef TELEIOS_RENDERER_MATERIAL_H
#define TELEIOS_RENDERER_MATERIAL_H

#include "teleios/types.h"
#include "teleios/graphics/types.h"

b8 tl_renderer_material_bind(const TLMaterial* material);
b8 tl_renderer_material_unbind(const TLMaterial* material);

TLEXPORT b8 tl_renderer_material_attach(TLMaterial* material);

#endif // TELEIOS_RENDERER_MATERIAL_H