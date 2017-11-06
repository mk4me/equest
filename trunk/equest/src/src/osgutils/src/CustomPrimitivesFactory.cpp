#include "PCH.h"
#include <osgutils/CustomPrimitivesFactory.h>

using namespace osgutils;

const CustomCylinderDescription CustomPrimitivesFactory::createCylinder( const unsigned int complex,
	const double height,
	const double radius,
		const osg::Vec4 & color )
{
	CustomCylinderDescription ret;

	ret.complex = complex;
	ret.height = height;
	ret.radius = radius;

	ret.geom = new osg::Geometry();
	//ret.geom->setUseDisplayList(false);
	//ret.geom->setUseVertexBufferObjects(true);
	ret.geomBase = CustomPrimitivesHelper::createCylinderGeometryBase(height, radius, complex);

	ret.geom->setVertexArray(ret.geomBase.verticies);
	ret.geom->setNormalArray(ret.geomBase.normals);
	ret.geom->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);
	ret.geom->setDataVariance(osg::Object::DYNAMIC);

	const unsigned int mod = complex * 2;
	for (unsigned int i = 0; i < complex; ++i) {
		osg::ref_ptr<osg::DrawElementsUInt> d = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
		d->push_back((i * 2    ) % mod);
		d->push_back((i * 2 + 1) % mod);
		d->push_back((i * 2 + 2) % mod);
		ret.geom->addPrimitiveSet(d);

		osg::ref_ptr<osg::DrawElementsUInt> d2 = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
		d2->push_back((i * 2 + 1) % mod);
		d2->push_back((i * 2 + 2) % mod);
		d2->push_back((i * 2 + 3) % mod);
		ret.geom->addPrimitiveSet(d2);
	}

	ret.colors = new osg::Vec4Array;
	ret.colors->push_back(color);
	ret.geom->setColorArray(ret.colors);
	ret.geom->setColorBinding(osg::Geometry::BIND_OVERALL);
	
	return ret;
}

const CustomSphereDescription CustomPrimitivesFactory::createSphere( const unsigned int complex,
		const double radius,
		const osg::Vec4 & color )
{

	CustomSphereDescription ret;

	ret.complex = complex;
	ret.radius = radius;

	ret.geom = new osg::Geometry();
	//ret.geom->setUseDisplayList(false);
	//ret.geom->setUseVertexBufferObjects(true);
	ret.geomBase = CustomPrimitivesHelper::createSphereGeometryBase(radius, complex);

	const auto n = complex;	
	const auto numberOfSeparators = 4 * n + 4;

	ret.geom->setVertexArray(ret.geomBase.verticies);
	ret.geom->setNormalArray(ret.geomBase.normals);
	ret.geom->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);
	ret.geom->setDataVariance(osg::Object::DYNAMIC);

	const auto s = (n << 1);

	for (unsigned int e = 0; e < s; ++e) {
		for (unsigned int i = 0; i < numberOfSeparators; ++i) {
			osg::ref_ptr<osg::DrawElementsUInt> d = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
			d->push_back(e * numberOfSeparators + i);
			d->push_back(e * numberOfSeparators + i + numberOfSeparators);
			d->push_back(e * numberOfSeparators + (i + 1) % numberOfSeparators + numberOfSeparators);
			ret.geom->addPrimitiveSet(d);

			osg::ref_ptr<osg::DrawElementsUInt> d2 = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
			d2->push_back(e * numberOfSeparators + (i + 1) % numberOfSeparators + numberOfSeparators);
			d2->push_back(e * numberOfSeparators + (i + 1) % numberOfSeparators);
			d2->push_back(e * numberOfSeparators + i);
			ret.geom->addPrimitiveSet(d2);
		}
	}

	for (unsigned int i = 0; i < numberOfSeparators; ++i) {
		osg::ref_ptr<osg::DrawElementsUInt> d = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
		d->push_back(s * numberOfSeparators + i);
		d->push_back(s * numberOfSeparators + (i + 1) % numberOfSeparators);
		d->push_back(numberOfSeparators * (s + 1));
		ret.geom->addPrimitiveSet(d);
	}

	for (unsigned int i = 0; i < numberOfSeparators; ++i) {
		osg::ref_ptr<osg::DrawElementsUInt> d = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
		d->push_back(i);
		d->push_back((i + 1) % numberOfSeparators);
		d->push_back(numberOfSeparators * (s + 1) + 1);
		ret.geom->addPrimitiveSet(d);
	}

	ret.colors = new osg::Vec4Array;
	ret.colors->push_back(color);
	ret.geom->setColorArray(ret.colors);
	ret.geom->setColorBinding(osg::Geometry::BIND_OVERALL);

	return ret;
}

const GeometryBase CustomPrimitivesHelper::createSphereGeometryBase(const double radius,
	const unsigned int complex)
{
	GeometryBase geomBase;

	geomBase.verticies = new osg::Vec3Array();
	geomBase.normals = new osg::Vec3Array();

	const int n = complex;
	const double segmentRad = osg::PI / 2.0 / (n + 1);
	const unsigned int numberOfSeparators = 4 * n + 4;

	osg::Vec3 v;

	for (int e = -n; e <= n; ++e) {
		float r_e = radius * cosf(segmentRad * e);
		float y_e = radius * sinf(segmentRad * e);

		for (unsigned int s = 0; s <= (numberOfSeparators - 1); ++s) {
			float z_s = r_e * sinf(segmentRad * s) * (-1);
			float x_s = r_e * cosf(segmentRad * s);
			v.set(x_s, y_e, z_s);
			geomBase.verticies->push_back(v);
			v.normalize();
			geomBase.normals->push_back(v);
		}
	}

	v.set(0, radius, 0);
	geomBase.verticies->push_back(v);
	v.normalize();
	geomBase.normals->push_back(v);

	v.set(0, -1 * radius, 0);
	geomBase.verticies->push_back(v);
	v.normalize();
	geomBase.normals->push_back(v);

	return geomBase;
}

osg::ref_ptr<osg::Vec3Array> CustomPrimitivesHelper::recreateVerticesForRadius(const CustomSphereDescription & sphereDesription,
	const double radius)
{
	const double f = radius / sphereDesription.radius;

	osg::ref_ptr<osg::Vec3Array> ret(dynamic_cast<osg::Vec3Array*>(sphereDesription.geomBase.verticies->clone(osg::CopyOp::DEEP_COPY_ALL)));

	for(auto it = ret->begin(); it != ret->end(); ++it){
		(*it) *= f;
	}

	return ret;
}

const GeometryBase CustomPrimitivesHelper::createCylinderGeometryBase(const double height,
	const double radius, const unsigned int complex)
{
	GeometryBase ret;

	ret.verticies = new osg::Vec3Array();
	ret.normals = new osg::Vec3Array();
	
	const double segmentRad = 2 * osg::PI / complex;
	osg::Vec3 v;

	const double halfHeight = height / 2.0;

	for (unsigned int i = 0; i < complex; ++i) {
		float rad = static_cast<float>(i) * segmentRad;
		float x = radius * cosf(rad);
		float z = radius * sinf(rad);

		v.set(x, z, halfHeight);
		ret.verticies->push_back(v);
		v.set(x, z, -halfHeight);
		ret.verticies->push_back(v);

		v.set(x, z, 0.0f);
		v.normalize();
		ret.normals->push_back(v);
		ret.normals->push_back(v);
	}

	v.set(0, 0, halfHeight);
	ret.verticies->push_back(v);
	ret.normals->push_back(v);
	v.set(0, 0, -halfHeight);
	ret.verticies->push_back(v);   
	ret.normals->push_back(v);

	return ret;
}

void CustomPrimitivesHelper::scaleSphereRadius(osg::Vec3Array * vertices,
	const double radiusScaleFactor)
{
	for(auto it = vertices->begin(); it != vertices->end(); ++it){
		(*it) *= radiusScaleFactor;
	}
}

void CustomPrimitivesHelper::scaleCylinderRadius(osg::Vec3Array * vertices,
	const double radiusScaleFactor)
{
	const auto s = vertices->size() - 2;
	for(int i = 0; i < s; ++i){
		(*vertices)[i].x() *= radiusScaleFactor;
		(*vertices)[i].y() *= radiusScaleFactor;
	}
}

void CustomPrimitivesHelper::scaleCylinderHeight(osg::Vec3Array * vertices,
	const double heightScaleFactor)
{
	for(int i = 0; i < vertices->size(); ++i){
		(*vertices)[i].z() *= heightScaleFactor;
	}
}

void CustomPrimitivesHelper::scaleCylinderHeightRadius(osg::Vec3Array * vertices,
	const double radiusScaleFactor,
	const double heightScaleFactor)
{
	const auto s = vertices->size() - 2;
	for(int i = 0; i < s; ++i){
		(*vertices)[i].x() *= radiusScaleFactor;
		(*vertices)[i].y() *= radiusScaleFactor;
		(*vertices)[i].z() *= heightScaleFactor;
	}

	for(int i = s; i < vertices->size(); ++i){		
		(*vertices)[i].z() *= heightScaleFactor;
	}
}

osg::ref_ptr<osg::Vec3Array> CustomPrimitivesHelper::recreateVerticesForRadius(const CustomCylinderDescription & cylinderDesription,
	const double radius)
{	
	osg::ref_ptr<osg::Vec3Array> ret(dynamic_cast<osg::Vec3Array*>(cylinderDesription.geomBase.verticies->clone(osg::CopyOp::DEEP_COPY_ALL)));

	scaleCylinderRadius(ret, radius / cylinderDesription.radius);

	return ret;
}

osg::ref_ptr<osg::Vec3Array> CustomPrimitivesHelper::recreateVerticesForHeight(const CustomCylinderDescription & cylinderDesription,
	const double height)
{
	osg::ref_ptr<osg::Vec3Array> ret(dynamic_cast<osg::Vec3Array*>(cylinderDesription.geomBase.verticies->clone(osg::CopyOp::DEEP_COPY_ALL)));

	scaleCylinderHeight(ret, height / cylinderDesription.height);

	return ret;
}

osg::ref_ptr<osg::Vec3Array> CustomPrimitivesHelper::recreateVerticesForHeightRadius(const CustomCylinderDescription & cylinderDesription,
	const double height, const double radius)
{
	osg::ref_ptr<osg::Vec3Array> ret(dynamic_cast<osg::Vec3Array*>(cylinderDesription.geomBase.verticies->clone(osg::CopyOp::DEEP_COPY_ALL)));

	scaleCylinderHeightRadius(ret, height / cylinderDesription.height, radius / cylinderDesription.radius);

	return ret;
}

void CustomPrimitivesHelper::setColor(CustonGeometryDescriptionBase & primitiveDescription, osg::Vec4Array * color)
{
	primitiveDescription.geom->setColorArray(color, osg::Array::BIND_OVERALL);
    primitiveDescription.geom->dirtyDisplayList();
}

void CustomPrimitivesHelper::updatePrimitiveShape(CustonGeometryDescriptionBase & primitiveDescription,
	osg::Vec3Array * vertices)
{
	const auto count = primitiveDescription.geom->referenceCount();
	if (count == 1 || (count > 1 && primitiveDescription.geom->getVertexArray() != vertices)){
		primitiveDescription.geom->setVertexArray(vertices);
		primitiveDescription.geom->dirtyDisplayList();
	}
}

const CustomSphereDescription CustomPrimitivesHelper::cloneForRadiusChange(const CustomSphereDescription & sphereDesription,
	const double radius)
{
	CustomSphereDescription ret = sphereDesription;
	ret.radius = radius;	

	ret.geom = dynamic_cast<osg::Geometry*>(ret.geom->clone(osg::CopyOp::SHALLOW_COPY));
	ret.geomBase.verticies = recreateVerticesForRadius(sphereDesription, radius);

	updatePrimitiveShape(ret, ret.geomBase.verticies);

	return ret;
}

const CustomCylinderDescription CustomPrimitivesHelper::cloneForRadiusChange(const CustomCylinderDescription & cylinderDesription,
	const double radius)
{
	CustomCylinderDescription ret = cylinderDesription;
	ret.radius = radius;

	ret.geom = dynamic_cast<osg::Geometry*>(ret.geom->clone(osg::CopyOp::SHALLOW_COPY));
	ret.geomBase.verticies = recreateVerticesForRadius(cylinderDesription, radius);

	updatePrimitiveShape(ret, ret.geomBase.verticies);

	return ret;
}

const CustomCylinderDescription CustomPrimitivesHelper::cloneForHeightChange(const CustomCylinderDescription & cylinderDesription,
	const double height)
{
	CustomCylinderDescription ret = cylinderDesription;
	ret.height = height;	

	ret.geom = dynamic_cast<osg::Geometry*>(ret.geom->clone(osg::CopyOp::SHALLOW_COPY));
	ret.geomBase.verticies = recreateVerticesForHeight(cylinderDesription, height);

	updatePrimitiveShape(ret, ret.geomBase.verticies);

	return ret;
}