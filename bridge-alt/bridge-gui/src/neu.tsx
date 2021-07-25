
import { assert } from 'console';
import React from 'react';


class Ray {
  /// Angle on xOy plane in radians.
  theta: number;
  /// Angle on ωOz plane in radians.
  omega: number;

  constructor(theta: number, omega: number) {
    assert(theta >= 0.0 && theta <= 2 * Math.PI);
    assert(omega >= 0.0 && omega <= Math.PI / 2);
    this.theta = theta;
    this.omega = omega;
  }

  /// Project shadow to xOy plane.
  project(height: number): [number, number] {
    let dxOy = height * Math.cos(this.omega);
    let dx = dxOy * Math.cos(this.theta),
        dy = dxOy * Math.sin(this.theta);
    // map to css plane from cartesian plane
    return [dx, -dy];
  }
}


function get_neumorphic_params(
    ray: Ray,
    height: number,
    intensity: number,
    smoothness: number) {
  assert(height > 0.0);
  assert(intensity >= 0.0 && intensity <= 1.0);
  assert(smoothness >= 0.0);
  let [dx, dy] = ray.project(height);
  let blurRadius = Math.sqrt(dx * dx + dy * dy) * smoothness;
  return [dx, dy, blurRadius];
}
