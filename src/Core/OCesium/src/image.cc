// -*- tab-width: 4; -*-
// vi: set sw=2 ts=4 expandtab:

// Copyright 2010-2020 The Khronos Group Inc.
// SPDX-License-Identifier: Apache-2.0

//!
//! @internal
//! @~English
//! @file image.cc
//!
//! @brief Image creation functions
//!

#include <sstream>
#include <stdexcept>
#include <vector>

#include "image.hpp"

//const std::vector<Image::CreateFunction> Image::CreateFunctions = {
//    CreateFromNPBM,
//    CreateFromPNG,
//    CreateFromJPG
//};

Image* Image::CreateFromFile(const _tstring& name,
                             bool transformOETF, bool rescaleTo8Bit) {
    return nullptr; // Keep compilers happy.
}

