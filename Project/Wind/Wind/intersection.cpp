#include "geometry.h"

using namespace geom;

Intersection::Intersection()
	: type(INTERSECTION_NONE)
{

}

Intersection::Intersection(Triangle tri, Line line)
{
	Vector triNormal = tri.normal();
	Vector v = line.getV();

	float f1 = triNormal | (tri.vec1 - line.vec1);
	float f2 = triNormal | v;

	if(f2 == 0.0f)
	{
		if(f1 != 0.0f)
		{
			type = INTERSECTION_NONE;
		}
		else
		{
			type = INTERSECTION_LINE;
		}
	}
	else
	{
		float t = f1 / f2;

		if((t < 0.0f) | (t > 1.0f))
		{
			type = INTERSECTION_NONE;
		}
		else
		{
			Vector vec = line.trace(t);
			if((((tri.vec2 - tri.vec1) & (vec - tri.vec1)) | triNormal) >= 0.0f && (((tri.vec3 - tri.vec2) & (vec - tri.vec2)) | triNormal) >= 0.0f && (((tri.vec1 - tri.vec3) & (vec - tri.vec3)) | triNormal) >= 0.0f)
			{
				type = INTERSECTION_VECTOR;
			}
			else
			{
				type = INTERSECTION_NONE;
			}
		}
	}
}

Intersection::Intersection(Plane plane, Line line)
{
	Vector v = line.getV();

	float f1 = plane.normal | (plane.pos - line.vec1);
	float f2 = plane.normal | v;

	if(f2 == 0.0f)
	{
		if(f1 != 0.0f)
		{
			type = INTERSECTION_NONE;
		}
		else
		{
			type = INTERSECTION_LINE;
		}
	}
	else
	{
		float t = f1 / f2;

		if((t < 0.0f) | (t > 1.0f))
		{
			type = INTERSECTION_NONE;
		}
		else
		{
			type = INTERSECTION_VECTOR;
		}
	}
}

Intersection::Intersection(Quad quad, Line line)
{
	Vector quadNormal = quad.normal();
	Vector v = line.getV();

	float f1 = quadNormal | (quad.vec1 - line.vec1);
	float f2 = quadNormal | v;

	if(f2 == 0.0f)
	{
		if(f1 != 0.0f)
		{
			type = INTERSECTION_NONE;
		}
		else
		{
			type = INTERSECTION_LINE;
		}
	}
	else
	{
		float t = f1 / f2;

		if((t < 0.0f) | (t > 1.0f))
		{
			type = INTERSECTION_NONE;
		}
		else
		{
			Vector vec = line.trace(t);
			if((((quad.vec2 - quad.vec1) & (vec - quad.vec1)) | quadNormal) >= 0.0f && (((quad.vec3 - quad.vec2) & (vec - quad.vec2)) | quadNormal) >= 0.0f && (((quad.vec4 - quad.vec3) & (vec - quad.vec3)) | quadNormal) >= 0.0f && (((quad.vec1 - quad.vec4) & (vec - quad.vec4)) | quadNormal) >= 0.0f)
			{
				type = INTERSECTION_VECTOR;
			}
			else
			{
				type = INTERSECTION_NONE;
			}
		}
	}
}

Intersection::Intersection(AxisAlignedXY face, Line line)
{
	float f1 = face.pos.z - line.vec1.z;
	float f2 = line.vec2.z - line.vec1.z;

	if(f2 == 0.0f)
	{
		if(f1 != 0.0f)
		{
			type = INTERSECTION_NONE;
		}
		else
		{
			//TODO: Line intersects parallel face
		}
	}
	else
	{
		float t = f1 / f2;

		if((t < 0.0f) | (t > 1.0f))
		{
			type = INTERSECTION_NONE;
		}
		else
		{
			Vector vec = line.trace(t);
			if(face.inside(vec))
			{
				type = INTERSECTION_VECTOR;
			}
			else
			{
				type = INTERSECTION_NONE;
			}
		}
	}
}

Intersection::Intersection(AxisAlignedXZ face, Line line)
{
	float f1 = face.pos.y - line.vec1.y;
	float f2 = line.vec2.y - line.vec1.y;

	if(f2 == 0.0f)
	{
		if(f1 != 0.0f)
		{
			type = INTERSECTION_NONE;
		}
		else
		{
			//TODO: Line intersects parallel face
		}
	}
	else
	{
		float t = f1 / f2;

		if((t < 0.0f) | (t > 1.0f))
		{
			type = INTERSECTION_NONE;
		}
		else
		{
			Vector vec = line.trace(t);
			if(face.inside(vec))
			{
				type = INTERSECTION_VECTOR;
			}
			else
			{
				type = INTERSECTION_NONE;
			}
		}
	}
}

Intersection::Intersection(AxisAlignedYZ face, Line line)
{
	float f1 = face.pos.x - line.vec1.x;
	float f2 = line.vec2.x - line.vec1.x;

	if(f2 == 0.0f)
	{
		if(f1 != 0.0f)
		{
			type = INTERSECTION_NONE;
		}
		else
		{
			//TODO: Line intersects parallel face
		}
	}
	else
	{
		float t = f1 / f2;

		if((t < 0.0f) | (t > 1.0f))
		{
			type = INTERSECTION_NONE;
		}
		else
		{
			Vector vec = line.trace(t);
			if(face.inside(vec))
			{
				type = INTERSECTION_VECTOR;
			}
			else
			{
				type = INTERSECTION_NONE;
			}
		}
	}
}

Intersection::Intersection(AxisAlignedCube cube, Line line)
{
	bool left = line.vec1.x == cube.pos.x & line.vec2.x == cube.pos.x;
	bool right = line.vec1.x == cube.pos.x + cube.size.x & line.vec2.x == cube.pos.x + cube.size.x;
	bool bottom = line.vec1.y == cube.pos.y & line.vec2.y == cube.pos.y;
	bool top = line.vec1.y == cube.pos.y + cube.size.y & line.vec2.y == cube.pos.y + cube.size.y;
	bool front = line.vec1.z == cube.pos.z & line.vec2.z == cube.pos.z;
	bool back = line.vec1.z == cube.pos.z + cube.size.z & line.vec2.z == cube.pos.z + cube.size.z;

	if(left)
	{
		*this = Intersection(cube.getLeft(), line);
	}
	else if(right)
	{
		*this = Intersection(cube.getRight(), line);
	}
	else if(bottom)
	{
		*this = Intersection(cube.getBottom(), line);
	}
	else if(top)
	{
		*this = Intersection(cube.getTop(), line);
	}
	else if(front)
	{
		*this = Intersection(cube.getFront(), line);
	}
	else if(back)
	{
		*this = Intersection(cube.getBack(), line);
	}
	else
	{
		bool leftOut = line.vec1.x < cube.pos.x & line.vec2.x < cube.pos.x;
		bool rightOut = line.vec1.x > cube.pos.x + cube.size.x & line.vec2.x > cube.pos.x + cube.size.x;
		bool bottomOut = line.vec1.y < cube.pos.y & line.vec2.y < cube.pos.y;
		bool topOut = line.vec1.y > cube.pos.y + cube.size.y & line.vec2.y > cube.pos.y + cube.size.y;
		bool frontOut = line.vec1.z < cube.pos.z & line.vec2.z < cube.pos.z;
		bool backOut = line.vec1.z > cube.pos.z + cube.size.z & line.vec2.z > cube.pos.z + cube.size.z;

		if(leftOut | rightOut | bottomOut | topOut | frontOut | backOut)
		{
			type = INTERSECTION_NONE;
		}
		else
		{
			if(line.vec1.x < cube.pos.x ^ line.vec2.x < cube.pos.x)
			{
				if(Intersection(cube.getLeft(), line).type == INTERSECTION_VECTOR)
				{
					type = INTERSECTION_LINE;
					return;
				}
			}
			if(line.vec1.x < cube.pos.x + cube.size.x ^ line.vec2.x < cube.pos.x + cube.size.x)
			{
				if(Intersection(cube.getRight(), line).type == INTERSECTION_VECTOR)
				{
					type = INTERSECTION_LINE;
					return;
				}
			}
			if(line.vec1.y < cube.pos.y ^ line.vec2.y < cube.pos.y)
			{
				if(Intersection(cube.getBottom(), line).type == INTERSECTION_VECTOR)
				{
					type = INTERSECTION_LINE;
					return;
				}
			}
			if(line.vec1.y < cube.pos.y + cube.size.y ^ line.vec2.y < cube.pos.y + cube.size.y)
			{
				if(Intersection(cube.getTop(), line).type == INTERSECTION_VECTOR)
				{
					type = INTERSECTION_LINE;
					return;
				}
			}
			if(line.vec1.z < cube.pos.z ^ line.vec2.z < cube.pos.z)
			{
				if(Intersection(cube.getFront(), line).type == INTERSECTION_VECTOR)
				{
					type = INTERSECTION_LINE;
					return;
				}
			}
			if(line.vec1.z < cube.pos.z + cube.size.z ^ line.vec2.z < cube.pos.z + cube.size.z)
			{
				if(Intersection(cube.getBack(), line).type == INTERSECTION_VECTOR)
				{
					type = INTERSECTION_LINE;
					return;
				}
			}
		}
	}
}

Intersection::Intersection(Triangle tri, Ray ray)
{
	Vector triNormal = tri.normal();

	float f1 = triNormal | (tri.vec1 - ray.pos);
	float f2 = triNormal | ray.dir;

	if(f2 == 0.0f)
	{
		if(f1 != 0.0f)
		{
			type = INTERSECTION_NONE;
		}
		else
		{
			type = INTERSECTION_LINE;
		}
	}
	else
	{
		float t = f1 / f2;
		Vector vec = ray.trace(t);

		if((((tri.vec2 - tri.vec1) & (vec - tri.vec1)) | triNormal) >= 0.0f && (((tri.vec3 - tri.vec2) & (vec - tri.vec2)) | triNormal) >= 0.0f && (((tri.vec1 - tri.vec3) & (vec - tri.vec3)) | triNormal) >= 0.0f)
		{
			type = INTERSECTION_VECTOR;
		}
		else
		{
			type = INTERSECTION_NONE;
		}
	}
}

Intersection::Intersection(Plane plane, Ray ray)
{
	float f1 = plane.normal | (plane.pos - ray.pos);
	float f2 = plane.normal | ray.dir;

	if(f2 == 0.0f)
	{
		if(f1 != 0.0f)
		{
			type = INTERSECTION_NONE;
		}
		else
		{
			type = INTERSECTION_LINE;
		}
	}
	else
	{
		float t = f1 / f2;

		type = INTERSECTION_VECTOR;
	}
}

Intersection::Intersection(Quad quad, Ray ray)
{
	Vector quadNormal = quad.normal();

	float f1 = quadNormal | (quad.vec1 - ray.pos);
	float f2 = quadNormal | ray.dir;

	if(f2 == 0.0f)
	{
		if(f1 != 0.0f)
		{
			type = INTERSECTION_NONE;
		}
		else
		{
			type = INTERSECTION_LINE;
		}
	}
	else
	{
		float t = f1 / f2;
		Vector vec = ray.trace(t);

		if((((quad.vec2 - quad.vec1) & (vec - quad.vec1)) | quadNormal) >= 0.0f && (((quad.vec3 - quad.vec2) & (vec - quad.vec2)) | quadNormal) >= 0.0f && (((quad.vec4 - quad.vec3) & (vec - quad.vec3)) | quadNormal) >= 0.0f && (((quad.vec1 - quad.vec4) & (vec - quad.vec4)) | quadNormal) >= 0.0f)
		{
			type = INTERSECTION_VECTOR;
		}
		else
		{
			type = INTERSECTION_NONE;
		}
	}
}

Intersection::Intersection(AxisAlignedXY face, Ray ray)
{
	float f1 = face.pos.z - ray.pos.z;
	float f2 = ray.dir.z;

	if(f2 == 0.0f)
	{
		if(f1 != 0.0f)
		{
			type = INTERSECTION_NONE;
		}
		else
		{
			//TODO: Line intersects parallel face
		}
	}
	else
	{
		float t = f1 / f2;
		Vector vec = ray.trace(t);

		if(face.inside(vec))
		{
			type = INTERSECTION_VECTOR;
		}
		else
		{
			type = INTERSECTION_NONE;
		}
	}
}

Intersection::Intersection(AxisAlignedXZ face, Ray ray)
{
	float f1 = face.pos.y - ray.pos.y;
	float f2 = ray.dir.y;

	if(f2 == 0.0f)
	{
		if(f1 != 0.0f)
		{
			type = INTERSECTION_NONE;
		}
		else
		{
			//TODO: Line intersects parallel face
		}
	}
	else
	{
		float t = f1 / f2;
		Vector vec = ray.trace(t);

		if(face.inside(vec))
		{
			type = INTERSECTION_VECTOR;
		}
		else
		{
			type = INTERSECTION_NONE;
		}
	}
}

Intersection::Intersection(AxisAlignedYZ face, Ray ray)
{
	float f1 = face.pos.x - ray.pos.x;
	float f2 = ray.dir.x;

	if(f2 == 0.0f)
	{
		if(f1 != 0.0f)
		{
			type = INTERSECTION_NONE;
		}
		else
		{
			//TODO: Line intersects parallel face
		}
	}
	else
	{
		float t = f1 / f2;
		Vector vec = ray.trace(t);

		if(face.inside(vec))
		{
			type = INTERSECTION_VECTOR;
		}
		else
		{
			type = INTERSECTION_NONE;
		}
	}
}

Intersection::Intersection(AxisAlignedCube cube, Ray ray)
{
	if(ray.pos.x == cube.pos.x & ray.dir.x == 0.0f)
	{
		*this = Intersection(cube.getLeft(), ray);
	}
	else if(ray.pos.x == cube.pos.x + cube.size.x & ray.dir.x == 0.0f)
	{
		*this = Intersection(cube.getRight(), ray);
	}
	else if(ray.pos.y == cube.pos.y & ray.dir.y == 0.0f)
	{
		*this = Intersection(cube.getBottom(), ray);
	}
	else if(ray.pos.y == cube.pos.y + cube.size.y & ray.dir.y == 0.0f)
	{
		*this = Intersection(cube.getTop(), ray);
	}
	else if(ray.pos.z == cube.pos.z & ray.dir.z == 0.0f)
	{
		*this = Intersection(cube.getFront(), ray);
	}
	else if(ray.pos.z == cube.pos.z + cube.size.z & ray.dir.z == 0.0f)
	{
		*this = Intersection(cube.getBack(), ray);
	}
	else
	{
		if(Intersection(cube.getLeft(), ray).type == INTERSECTION_VECTOR)
		{
			type = INTERSECTION_LINE;
			return;
		}
		
		if(Intersection(cube.getRight(), ray).type == INTERSECTION_VECTOR)
		{
			type = INTERSECTION_LINE;
			return;
		}
		
		if(Intersection(cube.getBottom(), ray).type == INTERSECTION_VECTOR)
		{
			type = INTERSECTION_LINE;
			return;
		}
		
		if(Intersection(cube.getTop(), ray).type == INTERSECTION_VECTOR)
		{
			type = INTERSECTION_LINE;
			return;
		}
		
		if(Intersection(cube.getFront(), ray).type == INTERSECTION_VECTOR)
		{
			type = INTERSECTION_LINE;
			return;
		}
		
		if(Intersection(cube.getBack(), ray).type == INTERSECTION_VECTOR)
		{
			type = INTERSECTION_LINE;
			return;
		}
	}
}



TracedIntersection::TracedIntersection()
	: Intersection(), amount(0)
{

}

TracedIntersection::TracedIntersection(Triangle tri, Line line)
{
	Vector triNormal = tri.normal();
	Vector v = line.getV();

	float f1 = triNormal | (tri.vec1 - line.vec1);
	float f2 = triNormal | v;

	if(f2 == 0.0f)
	{
		if(f1 != 0.0f)
		{
			type = INTERSECTION_NONE;
		}
		else
		{
			//TODO: Line ON triangle
			type = INTERSECTION_LINE;
		}
	}
	else
	{
		float t = f1 / f2;

		if((t < 0.0f) | (t > 1.0f))
		{
			type = INTERSECTION_NONE;
		}
		else
		{
			Vector vec = line.trace(t);
			if((((tri.vec2 - tri.vec1) & (vec - tri.vec1)) | triNormal) >= 0.0f && (((tri.vec3 - tri.vec2) & (vec - tri.vec2)) | triNormal) >= 0.0f && (((tri.vec1 - tri.vec3) & (vec - tri.vec3)) | triNormal) >= 0.0f)
			{
				type = INTERSECTION_VECTOR;
				result = new Vector(vec);
				amount = 1;
			}
			else
			{
				type = INTERSECTION_NONE;
			}
		}
	}
}

TracedIntersection::TracedIntersection(Plane plane, Line line)
{
	Vector v = line.getV();

	float f1 = plane.normal | (plane.pos - line.vec1);
	float f2 = plane.normal | v;

	if(f2 == 0.0f)
	{
		if(f1 != 0.0f)
		{
			type = INTERSECTION_NONE;
		}
		else
		{
			//TODO: Line ON plane
			type = INTERSECTION_LINE;
		}
	}
	else
	{
		float t = f1 / f2;

		if((t < 0.0f) | (t > 1.0f))
		{
			type = INTERSECTION_NONE;
		}
		else
		{
			type = INTERSECTION_VECTOR;
			result = new Vector(line.trace(t));
			amount = 1;
		}
	}
}

TracedIntersection::TracedIntersection(Quad quad, Line line)
{
	Vector quadNormal = quad.normal();
	Vector v = line.getV();

	float f1 = quadNormal | (quad.vec1 - line.vec1);
	float f2 = quadNormal | v;

	if(f2 == 0.0f)
	{
		if(f1 != 0.0f)
		{
			type = INTERSECTION_NONE;
		}
		else
		{
			//TODO: Line ON quad
			type = INTERSECTION_LINE;
		}
	}
	else
	{
		float t = f1 / f2;

		if((t < 0.0f) | (t > 1.0f))
		{
			type = INTERSECTION_NONE;
		}
		else
		{
			Vector vec = line.trace(t);
			if((((quad.vec2 - quad.vec1) & (vec - quad.vec1)) | quadNormal) >= 0.0f && (((quad.vec3 - quad.vec2) & (vec - quad.vec2)) | quadNormal) >= 0.0f && (((quad.vec4 - quad.vec3) & (vec - quad.vec3)) | quadNormal) >= 0.0f && (((quad.vec1 - quad.vec4) & (vec - quad.vec4)) | quadNormal) >= 0.0f)
			{
				type = INTERSECTION_VECTOR;
				result = new Vector(vec);
				amount = 1;
			}
			else
			{
				type = INTERSECTION_NONE;
			}
		}
	}
}

TracedIntersection::TracedIntersection(AxisAlignedXY face, Line line)
{
	float f1 = face.pos.z - line.vec1.z;
	float f2 = line.vec2.z - line.vec1.z;

	if(f2 == 0.0f)
	{
		if(f1 != 0.0f)
		{
			type = INTERSECTION_NONE;
		}
		else
		{
			//TODO: Line intersects parallel face
		}
	}
	else
	{
		float t = f1 / f2;

		if((t < 0.0f) | (t > 1.0f))
		{
			type = INTERSECTION_NONE;
		}
		else
		{
			Vector vec = line.trace(t);
			if(face.inside(vec))
			{
				type = INTERSECTION_VECTOR;
				result = new Vector(vec);
				amount = 1;
			}
			else
			{
				type = INTERSECTION_NONE;
			}
		}
	}
}

TracedIntersection::TracedIntersection(AxisAlignedXZ face, Line line)
{
	float f1 = face.pos.y - line.vec1.y;
	float f2 = line.vec2.y - line.vec1.y;

	if(f2 == 0.0f)
	{
		if(f1 != 0.0f)
		{
			type = INTERSECTION_NONE;
		}
		else
		{
			//TODO: Line intersects parallel face
		}
	}
	else
	{
		float t = f1 / f2;

		if((t < 0.0f) | (t > 1.0f))
		{
			type = INTERSECTION_NONE;
		}
		else
		{
			Vector vec = line.trace(t);
			if(face.inside(vec))
			{
				type = INTERSECTION_VECTOR;
				result = new Vector(vec);
				amount = 1;
			}
			else
			{
				type = INTERSECTION_NONE;
			}
		}
	}
}

TracedIntersection::TracedIntersection(AxisAlignedYZ face, Line line)
{
	float f1 = face.pos.x - line.vec1.x;
	float f2 = line.vec2.x - line.vec1.x;

	if(f2 == 0.0f)
	{
		if(f1 != 0.0f)
		{
			type = INTERSECTION_NONE;
		}
		else
		{
			//TODO: Line intersects parallel face
		}
	}
	else
	{
		float t = f1 / f2;

		if((t < 0.0f) | (t > 1.0f))
		{
			type = INTERSECTION_NONE;
		}
		else
		{
			Vector vec = line.trace(t);
			if(face.inside(vec))
			{
				type = INTERSECTION_VECTOR;
				result = new Vector(vec);
				amount = 1;
			}
			else
			{
				type = INTERSECTION_NONE;
			}
		}
	}
}

TracedIntersection::TracedIntersection(AxisAlignedCube cube, Line line)
{
	bool left = line.vec1.x == cube.pos.x & line.vec2.x == cube.pos.x;
	bool right = line.vec1.x == cube.pos.x + cube.size.x & line.vec2.x == cube.pos.x + cube.size.x;
	bool bottom = line.vec1.y == cube.pos.y & line.vec2.y == cube.pos.y;
	bool top = line.vec1.y == cube.pos.y + cube.size.y & line.vec2.y == cube.pos.y + cube.size.y;
	bool front = line.vec1.z == cube.pos.z & line.vec2.z == cube.pos.z;
	bool back = line.vec1.z == cube.pos.z + cube.size.z & line.vec2.z == cube.pos.z + cube.size.z;

	if(cube.inside(line.vec1) && cube.inside(line.vec2))
	{
		type = INTERSECTION_LINE;
		result = new Line(line);
		amount = 1;
	}
	else if(left)
	{
		*this = TracedIntersection(cube.getLeft(), line);
	}
	else if(right)
	{
		*this = TracedIntersection(cube.getRight(), line);
	}
	else if(bottom)
	{
		*this = TracedIntersection(cube.getBottom(), line);
	}
	else if(top)
	{
		*this = TracedIntersection(cube.getTop(), line);
	}
	else if(front)
	{
		*this = TracedIntersection(cube.getFront(), line);
	}
	else if(back)
	{
		*this = TracedIntersection(cube.getBack(), line);
	}
	else
	{
		bool leftOut = line.vec1.x < cube.pos.x & line.vec2.x < cube.pos.x;
		bool rightOut = line.vec1.x > cube.pos.x + cube.size.x & line.vec2.x > cube.pos.x + cube.size.x;
		bool bottomOut = line.vec1.y < cube.pos.y & line.vec2.y < cube.pos.y;
		bool topOut = line.vec1.y > cube.pos.y + cube.size.y & line.vec2.y > cube.pos.y + cube.size.y;
		bool frontOut = line.vec1.z < cube.pos.z & line.vec2.z < cube.pos.z;
		bool backOut = line.vec1.z > cube.pos.z + cube.size.z & line.vec2.z > cube.pos.z + cube.size.z;

		if(leftOut | rightOut | bottomOut | topOut | frontOut | backOut)
		{
			type = INTERSECTION_NONE;
		}
		else
		{
			TracedIntersection intersection;
			Vector vecs[2];
			int amount = 0;

			if(line.vec1.x < cube.pos.x ^ line.vec2.x < cube.pos.x)
			{
				if((intersection = TracedIntersection(cube.getLeft(), line)).type == INTERSECTION_VECTOR)
				{
					vecs[amount++] = *(Vector*)intersection.result;
				}
			}
			if(line.vec1.x < cube.pos.x + cube.size.x ^ line.vec2.x < cube.pos.x + cube.size.x)
			{
				if((intersection = TracedIntersection(cube.getRight(), line)).type == INTERSECTION_VECTOR)
				{
					vecs[amount++] = *(Vector*)intersection.result;
				}
			}
			if(line.vec1.y < cube.pos.y ^ line.vec2.y < cube.pos.y)
			{
				if((intersection = TracedIntersection(cube.getBottom(), line)).type == INTERSECTION_VECTOR)
				{
					vecs[amount++] = *(Vector*)intersection.result;
				}
			}
			if(line.vec1.y < cube.pos.y + cube.size.y ^ line.vec2.y < cube.pos.y + cube.size.y)
			{
				if((intersection = TracedIntersection(cube.getTop(), line)).type == INTERSECTION_VECTOR)
				{
					vecs[amount++] = *(Vector*)intersection.result;
				}
			}
			if(line.vec1.z < cube.pos.z ^ line.vec2.z < cube.pos.z)
			{
				if((intersection = TracedIntersection(cube.getFront(), line)).type == INTERSECTION_VECTOR)
				{
					vecs[amount++] = *(Vector*)intersection.result;
				}
			}
			if(line.vec1.z < cube.pos.z + cube.size.z ^ line.vec2.z < cube.pos.z + cube.size.z)
			{
				if((intersection = TracedIntersection(cube.getBack(), line)).type == INTERSECTION_VECTOR)
				{
					vecs[amount++] = *(Vector*)intersection.result;
				}
			}

			if(amount == 0)
			{
				type = INTERSECTION_NONE;
			}
			else if(amount == 1)
			{
				if(cube.inside(line.vec1))
				{

				}
			}
		}
	}
}

TracedIntersection::TracedIntersection(Triangle tri, Ray ray)
{
	Vector triNormal = tri.normal();

	float f1 = triNormal | (tri.vec1 - ray.pos);
	float f2 = triNormal | ray.dir;

	if(f2 == 0.0f)
	{
		if(f1 != 0.0f)
		{
			type = INTERSECTION_NONE;
		}
		else
		{
			//TODO: Line ON triangle
			type = INTERSECTION_LINE;
		}
	}
	else
	{
		float t = f1 / f2;
		Vector vec = ray.trace(t);

		if((((tri.vec2 - tri.vec1) & (vec - tri.vec1)) | triNormal) >= 0.0f && (((tri.vec3 - tri.vec2) & (vec - tri.vec2)) | triNormal) >= 0.0f && (((tri.vec1 - tri.vec3) & (vec - tri.vec3)) | triNormal) >= 0.0f)
		{
			type = INTERSECTION_VECTOR;
			result = new Vector(vec);
			amount = 1;
		}
		else
		{
			type = INTERSECTION_NONE;
		}
	}
}

TracedIntersection::TracedIntersection(Plane plane, Ray ray)
{
	float f1 = plane.normal | (plane.pos - ray.pos);
	float f2 = plane.normal | ray.dir;

	if(f2 == 0.0f)
	{
		if(f1 != 0.0f)
		{
			type = INTERSECTION_NONE;
		}
		else
		{
			//TODO: Line ON plane
			type = INTERSECTION_LINE;
		}
	}
	else
	{
		float t = f1 / f2;

		type = INTERSECTION_VECTOR;
		result = new Vector(ray.trace(t));
		amount = 1;
	}
}

TracedIntersection::TracedIntersection(Quad quad, Ray ray)
{
	Vector quadNormal = quad.normal();

	float f1 = quadNormal | (quad.vec1 - ray.pos);
	float f2 = quadNormal | ray.dir;

	if(f2 == 0.0f)
	{
		if(f1 != 0.0f)
		{
			type = INTERSECTION_NONE;
		}
		else
		{
			//TODO: Line ON quad
			type = INTERSECTION_LINE;
		}
	}
	else
	{
		float t = f1 / f2;
		Vector vec = ray.trace(t);

		if((((quad.vec2 - quad.vec1) & (vec - quad.vec1)) | quadNormal) >= 0.0f && (((quad.vec3 - quad.vec2) & (vec - quad.vec2)) | quadNormal) >= 0.0f && (((quad.vec4 - quad.vec3) & (vec - quad.vec3)) | quadNormal) >= 0.0f && (((quad.vec1 - quad.vec4) & (vec - quad.vec4)) | quadNormal) >= 0.0f)
		{
			type = INTERSECTION_VECTOR;
			result = new Vector(vec);
			amount = 1;
		}
		else
		{
			type = INTERSECTION_NONE;
		}
	}
}

TracedIntersection::TracedIntersection(AxisAlignedXY face, Ray ray)
{
	float f1 = face.pos.z - ray.pos.z;
	float f2 = ray.dir.z;

	if(f2 == 0.0f)
	{
		if(f1 != 0.0f)
		{
			type = INTERSECTION_NONE;
		}
		else
		{
			//TODO: Line intersects parallel face
		}
	}
	else
	{
		float t = f1 / f2;
		Vector vec = ray.trace(t);

		if(face.inside(vec))
		{
			type = INTERSECTION_VECTOR;
			result = new Vector(vec);
			amount = 1;
		}
		else
		{
			type = INTERSECTION_NONE;
		}
	}
}

TracedIntersection::TracedIntersection(AxisAlignedXZ face, Ray ray)
{
	float f1 = face.pos.y - ray.pos.y;
	float f2 = ray.dir.y;

	if(f2 == 0.0f)
	{
		if(f1 != 0.0f)
		{
			type = INTERSECTION_NONE;
		}
		else
		{
			//TODO: Line intersects parallel face
		}
	}
	else
	{
		float t = f1 / f2;
		Vector vec = ray.trace(t);

		if(face.inside(vec))
		{
			type = INTERSECTION_VECTOR;
			result = new Vector(vec);
			amount = 1;
		}
		else
		{
			type = INTERSECTION_NONE;
		}
	}
}

TracedIntersection::TracedIntersection(AxisAlignedYZ face, Ray ray)
{
	float f1 = face.pos.x - ray.pos.x;
	float f2 = ray.dir.x;

	if(f2 == 0.0f)
	{
		if(f1 != 0.0f)
		{
			type = INTERSECTION_NONE;
		}
		else
		{
			//TODO: Line intersects parallel face
		}
	}
	else
	{
		float t = f1 / f2;
		Vector vec = ray.trace(t);

		if(face.inside(vec))
		{
			type = INTERSECTION_VECTOR;
			result = new Vector(vec);
			amount = 1;
		}
		else
		{
			type = INTERSECTION_NONE;
		}
	}
}

TracedIntersection::TracedIntersection(AxisAlignedCube cube, Ray ray)
{
	if(ray.pos.x == cube.pos.x && ray.dir.x == 0.0f)
	{
		*this = TracedIntersection(cube.getLeft(), ray);
	}
	else if(ray.pos.x == cube.pos.x + cube.size.x && ray.dir.x == 0.0f)
	{
		*this = TracedIntersection(cube.getRight(), ray);
	}
	else if(ray.pos.y == cube.pos.y && ray.dir.y == 0.0f)
	{
		*this = TracedIntersection(cube.getBottom(), ray);
	}
	else if(ray.pos.y == cube.pos.y + cube.size.y && ray.dir.y == 0.0f)
	{
		*this = TracedIntersection(cube.getTop(), ray);
	}
	else if(ray.pos.z == cube.pos.z && ray.dir.z == 0.0f)
	{
		*this = TracedIntersection(cube.getFront(), ray);
	}
	else if(ray.pos.z == cube.pos.z + cube.size.z && ray.dir.z == 0.0f)
	{
		*this = TracedIntersection(cube.getBack(), ray);
	}
	else
	{
		TracedIntersection intersection;
		Vector vecs[2];
		int amount = 0;

		for(int i = 0; i < 6 && amount < 2; i++)
		{
			switch(i)
			{
			case 0:
				intersection = TracedIntersection(cube.getLeft(), ray);
				break;
			case 1:
				intersection = TracedIntersection(cube.getRight(), ray);
				break;
			case 2:
				intersection = TracedIntersection(cube.getBottom(), ray);
				break;
			case 3:
				intersection = TracedIntersection(cube.getTop(), ray);
				break;
			case 4:
				intersection = TracedIntersection(cube.getFront(), ray);
				break;
			case 5:
				intersection = TracedIntersection(cube.getBack(), ray);
				break;
			}

			if(intersection.type == INTERSECTION_VECTOR)
			{
				vecs[amount++] = *(Vector*)intersection.result;
			}
		}

		if(amount == 0)
		{
			type = INTERSECTION_NONE;
		}
		else
		{
			type = INTERSECTION_LINE;
			result = new Line(vecs[0], vecs[1]);
			amount = 1;
		}
	}
}

TracedIntersection::~TracedIntersection()
{
	if(amount == 1)
	{
		delete result;
	}
	else if(amount > 1)
	{
		delete[] result;
	}
}