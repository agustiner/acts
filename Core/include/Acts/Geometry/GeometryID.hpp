// This file is part of the Acts project.
//
// Copyright (C) 2016-2019 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <cstdint>
#include <ostream>

#include "Acts/Utilities/Helpers.hpp"

namespace Acts {

/// Identifier for geometry nodes.
///
/// Each identifier can be split info the following components:
///
/// - Volumes                 - uses counting given by TrackingGeometry
/// - (Boundary)  Surfaces    - counts through boundary surfaces
/// - (Layer)     Surfaces    - counts confined layers
/// - (Approach)  Surfaces    - counts approach surfaces
/// - (Sensitive) Surfaces    - counts through sensitive surfaces
///
class GeometryID {
 public:
  using Value = uint64_t;

  /// Construct default GeometryID with all values set to zero.
  constexpr GeometryID() : m_value(0) {}
  /// Construct from an already encoded value.
  constexpr GeometryID(Value encoded) : m_value(encoded) {}
  constexpr GeometryID(GeometryID&&) = default;
  constexpr GeometryID(const GeometryID&) = default;
  GeometryID& operator=(GeometryID&&) = default;
  GeometryID& operator=(const GeometryID&) = default;

  /// Return the encoded value.
  constexpr Value value() const { return m_value; }

  /// Return the volume identifier.
  constexpr Value volume() const {
    return ACTS_BIT_DECODE(m_value, volume_mask);
  }
  /// Return the boundary identifier.
  constexpr Value boundary() const {
    return ACTS_BIT_DECODE(m_value, boundary_mask);
  }
  /// Return the layer identifier.
  constexpr Value layer() const { return ACTS_BIT_DECODE(m_value, layer_mask); }
  /// Return the approach identifier.
  constexpr Value approach() const {
    return ACTS_BIT_DECODE(m_value, approach_mask);
  }
  /// Return the sensitive identifier.
  constexpr Value sensitive() const {
    return ACTS_BIT_DECODE(m_value, sensitive_mask);
  }

  /// Set the volume identifier.
  constexpr GeometryID& setVolume(Value volume) {
    return setBits(volume_mask, volume);
  }
  /// Set the boundary identifier.
  constexpr GeometryID& setBoundary(Value boundary) {
    return setBits(boundary_mask, boundary);
  }
  /// Set the layer identifier.
  constexpr GeometryID& setLayer(Value layer) {
    return setBits(layer_mask, layer);
  }
  /// Set the approach identifier.
  constexpr GeometryID& setApproach(Value approach) {
    return setBits(approach_mask, approach);
  }
  /// Set the sensitive identifier.
  constexpr GeometryID& setSensitive(Value sensitive) {
    return setBits(sensitive_mask, sensitive);
  }

 private:
  // clang-format off
  constexpr static Value volume_mask    = 0xff00000000000000; // 255 volumes
  constexpr static Value boundary_mask  = 0x00ff000000000000; // 255 boundaries
  constexpr static Value layer_mask     = 0x0000fff000000000; // 4095 layers
  constexpr static Value approach_mask  = 0x0000000ff0000000; // 255 approach surfaces
  constexpr static Value sensitive_mask = 0x000000000fffffff; // (2^28)-1 sensitive surfaces
  // clang-format on

  Value m_value = 0;

  /// Set the subset of bits indicated by the mask
  constexpr GeometryID& setBits(Value mask, Value id) {
    m_value = (m_value & ~mask) | (ACTS_BIT_ENCODE(id, mask) & mask);
    return *this;
  }

  friend constexpr bool operator==(GeometryID lhs, GeometryID rhs) {
    return lhs.m_value == rhs.m_value;
  }
  friend constexpr bool operator<(GeometryID lhs, GeometryID rhs) {
    return lhs.m_value < rhs.m_value;
  }
  friend inline std::ostream& operator<<(std::ostream& os, GeometryID id) {
    os << "[ " << std::setw(3) << id.volume();
    os << " | " << std::setw(3) << id.boundary();
    os << " | " << std::setw(3) << id.layer();
    os << " | " << std::setw(3) << id.approach();
    os << " | " << std::setw(4) << id.sensitive() << " ]";
    return os;
  }
};

}  // namespace Acts
