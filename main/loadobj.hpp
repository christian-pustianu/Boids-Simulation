#pragma once

#include "Mesh.hpp"

SimpleMesh load_wavefront_obj_to_simplemesh( char const* aPath );

MultiMaterialMesh load_wavefront_obj_to_MMmesh( char const* aPath );
