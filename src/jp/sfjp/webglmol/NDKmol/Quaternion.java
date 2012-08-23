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

public class Quaternion {
	public float x = 0, y = 0, z = 0, w = 1;
	
	public static Quaternion multiply(Quaternion p, Quaternion q) {
		Quaternion ret = new Quaternion();
		
		ret.x = p.x * q.w + p.y * q.z - p.z * q.y + p.w * q.x; // 1
		ret.y = -p.x * q.z + p.y * q.w + p.z * q.x + p.w * q.y; // 2
		ret.z = p.x * q.y - p.y * q.x + p.z * q.w + p.w * q.z; //3
		ret.w = -p.x * q.x - p.y * q.y - p.z * q.z + p.w * q.w; // 0
		
		return ret;
	}
	
	public Vector3 rotateVector(Vector3 vec) {
		Quaternion q = Quaternion.multiply(Quaternion.multiply(this.clone().invert(), new Quaternion(vec.x, vec.y, vec.z, 0)), this);
		
		return new Vector3(q.x, q.y, q.z);
	}
	
	public Quaternion clone() {
		Quaternion q = new Quaternion(x, y, z, w);
		return q;
	}
	
	public Quaternion invert() {
		this.x *= -1;
		this.y *= -1;
		this.z *= -1;
		return this;
	}
	
	public float getAngle() {
		return (float)Math.acos(this.w) * 2;
	}
	
	public Vector3 getAxis() {
		float angle = getAngle();
		float sin = (float)Math.sin(angle / 2);
		
		if (Math.abs(sin) < 0.001) return new Vector3(1, 0, 0);
		
		Vector3 ret = new Vector3();
		ret.x = this.x / sin;
		ret.y = this.y / sin;
		ret.z = this.z / sin;
		return ret;
	}
	
	public Quaternion() {
		
	}
	
	public String toString() {
		return "(" + x + ", " + y + ", " + z + ", " + w + ")";
	}
	
	public Quaternion(float x, float y, float z, float w) {
		this.x = x; this.y = y; this.z = z; this.w = w;
	}
}
