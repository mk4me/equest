/********************************************************************
    created:  2013/12/13
    created:  13:12:2013   7:33
    filename: CustomPrimitivesFactory.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_OSG_UTILS__CUSTOMPRIMITIVESFACTORY_H__
#define HEADER_GUARD_OSG_UTILS__CUSTOMPRIMITIVESFACTORY_H__

#include <utils/Debug.h>
#include <type_traits>
#include <osg/StateSet>
#include <osg/Geometry>

namespace osgutils
{

	struct GeometryBase
	{
		osg::ref_ptr<osg::Vec3Array> verticies;
		osg::ref_ptr<osg::Vec3Array> normals;
	};

	struct CustonGeometryDescriptionBase
	{		
		osg::ref_ptr<osg::Geometry> geom;
		osg::ref_ptr<osg::Vec4Array> colors;
		GeometryBase geomBase;
		unsigned int complex;
	};

	struct CustomCylinderDescription : public CustonGeometryDescriptionBase
	{
		double height;
		double radius;
	};

	struct CustomSphereDescription : public CustonGeometryDescriptionBase
	{
		double radius;
	};

	class CustomPrimitivesFactory
	{
	public:
		//! Metoda tworzy wêze³ z cylindrem
		//! \param complex z³o¿onoœæ cylindra (l. wierzcho³ków podstawy)
		//! \param height d³ugoœæ / wysokoœæ cylindra
		//! \param radius promieñ cylindra
		//! \param color kolor cylindra
		//! \return wêze³ z nowo utworzonym cylindrem
		static const CustomCylinderDescription createCylinder( const unsigned int complex,
			const double height,
			const double radius,
			const osg::Vec4 & color );

		//! Tworzy geometriê ze sfera
		//! \param complex z³o¿onoœæ sfery
		//! \param color kolor sfery
		//! \return wskaŸnik do stworzonej geometrii
		static const CustomSphereDescription createSphere( const unsigned int complex,
			const double radius,
			const osg::Vec4 & color );

	};


	//! Klasa z metodami pomocniczymy przy tworzeniu modyfikacji prymitywów
	//! Staraj¹ siê maxymalizowaæ wykorzystanie dostêpnych zasobów przez wspó³dzielenie ich
	//! Klonowania odbywaj¹ siê tylko tam gdzie jest to niezbêdne
	class CustomPrimitivesHelper
	{
	public:

		static void scaleSphereRadius(osg::Vec3Array * vertices,
			const double radiusScaleFactor);

		static void scaleCylinderRadius(osg::Vec3Array * vertices,
			const double radiusScaleFactor);

		static void scaleCylinderHeight(osg::Vec3Array * vertices,
			const double radiusScaleFactor);

		static void scaleCylinderHeightRadius(osg::Vec3Array * vertices,
			const double radiusScaleFactor,
			const double heightScaleFactor);

		static const GeometryBase createSphereGeometryBase(const double radius,
			const unsigned int complex);

		static osg::ref_ptr<osg::Vec3Array> recreateVerticesForRadius(const CustomSphereDescription & sphereDesription,
			const double radius);

		static const GeometryBase createCylinderGeometryBase(const double height,
			const double radius, const unsigned int complex);

		static osg::ref_ptr<osg::Vec3Array> recreateVerticesForRadius(const CustomCylinderDescription & cylinderDesription,
			const double radius);

		static osg::ref_ptr<osg::Vec3Array> recreateVerticesForHeight(const CustomCylinderDescription & cylinderDesription,
			const double height);

		static osg::ref_ptr<osg::Vec3Array> recreateVerticesForHeightRadius(const CustomCylinderDescription & cylinderDesription,
			const double height, const double radius);	

		static void setColor(CustonGeometryDescriptionBase & primitiveDescription, osg::Vec4Array * color);

		static void updatePrimitiveShape(CustonGeometryDescriptionBase & primitiveDescription, osg::Vec3Array * vertices);

		//! \tparam T Obiekt deskryptora pochodz¹cego od CustonGeometryDescriptionBase
		//! \param primitiveDesription Deskryptor klonowany na potrzeby zmiany koloru
		//! \param color Nowy kolor prymitywu
		//! \return Deskryptor prymitywu ze zmienionym kolorem
		template<typename T>
		static const T cloneForColorChange(const T & primitiveDesription, const osg::Vec4 & color)
		{
			static_assert((std::is_base_of<CustonGeometryDescriptionBase,T>::value), "Klonowany obiekt musi byæ pochodn¹ CustonGeometryDescriptionBase");

			T ret = primitiveDesription;
			ret.geom = dynamic_cast<osg::Geometry*>(ret.geom->clone(osg::CopyOp::SHALLOW_COPY));
			ret.colors = new osg::Vec3Array;
			ret.colors->push_back(color);
			ret.geom->setColorArray(ret.colors);

			return ret;
		}

		//! \param sphereDesription Deskryptor sfery klonowany na potrzeby zmiany promienia
		//! \param radius Nowy promieñ
		//! \return Deskryptor prymitywu ze zmienionym promieniem
		static const CustomSphereDescription cloneForRadiusChange(const CustomSphereDescription & sphereDesription,
			const double radius);

		//! \param cylinderDesription Deskryptor cylindra klonowany na potrzeby zmiany promienia
		//! \param radius Nowy promieñ
		//! \return Deskryptor prymitywu ze zmienionym promieniem
		static const CustomCylinderDescription cloneForRadiusChange(const CustomCylinderDescription & cylinderDesription,
			const double radius);

		//! \param cylinderDesription Deskryptor cylindra klonowany na potrzeby zmiany wysokoœci
		//! \param height Nowa wysokoœæ
		//! \return Deskryptor prymitywu ze zmienion¹ wysokoœci¹
		static const CustomCylinderDescription cloneForHeightChange(const CustomCylinderDescription & cylinderDesription,
			const double height);
	};
}

#endif	//	HEADER_GUARD_OSG_UTILS__CUSTOMPRIMITIVESFACTORY_H__
