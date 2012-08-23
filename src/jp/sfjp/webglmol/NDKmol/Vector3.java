/*  NDKmol - Molecular Viewer on Android NDK

     (C) Copyright 2011 - 2012, biochem_fan

     This file is part of NDKmol.

     NDKmol is free software: you can redistribute it and/or modify
     it under the terms of the GNU Lesser General Public License as published by
     the Free Software Foundation, either version 3 of the License, or
     (at your option) any later version.

     This program is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU Lesser General Public License for more details.

     You should have received a copy of the GNU Lesser General Public License
     along with this program.  If not, see <http://www.gnu.org/licenses/>. */

package jp.sfjp.webglmol.NDKmol;

public class Vector3 {
	public float x, y, z;
	
	public Vector3(float x, float y, float z) {
		this.x = x;
		this.y = y;
		this.z = z;
	}
	
	public static float dot(Vector3 p, Vector3 q) {
		return p.x * q.x + p.y * q.y + p.z * q.z;
	}
	
	public static Vector3 cross(Vector3 p, Vector3 q) {
		return new Vector3(p.y * q.z - p.z * q.y, p.z * q.x - p.x * q.z, p.x * q.y - p.y * q.x);
	}
	
	public Vector3() {
		// TODO Auto-generated constructor stub
	}

	public static double norm(float x, float y, float z) {
		return Math.sqrt(x * x + y * y + z * z);
	}
	
	public void normalize() {
		float norm = (float)norm(this.x, this.y, this.z);
		this.x /= norm;
		this.y /= norm;
		this.z /= norm;
	}
}
