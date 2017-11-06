/********************************************************************
	created:	2011/09/06
	created:	6:9:2011   13:34
	filename: 	TrajectoriesDrawer.h
	author:		Wojciech Kniec

	purpose:
*********************************************************************/

#ifndef HEADER_GUARD_TRAJECTORIES_DRAWER__TRAJECTORIESDRAWER_H__
#define HEADER_GUARD_TRAJECTORIES_DRAWER__TRAJECTORIESDRAWER_H__

#include <osgutils/OsgSchemeDrawer.h>

class TrajectoryDrawerManager : public osgutils::IBaseDrawerScheme, public osgutils::IOsgSchemeDrawer, public osgutils::IRangeDrawer
{
private:

	typedef std::vector<utils::shared_ptr<osgutils::TrajectorySchemeDrawer>> FeaturesDescriptors;

public:
	TrajectoryDrawerManager();
	virtual ~TrajectoryDrawerManager();

	virtual void setRange(const Range & range);


    void setRange(const unsigned int idx, const Range & range);
    Range range(const unsigned int idx) const;
    int maxRange(const unsigned int idx) const;
	void initialize(const std::vector<std::vector<osg::Vec3>> & pointsPositions);

	virtual osg::ref_ptr<osg::Node> getNode();

	const unsigned int count() const;

	//! \param color Kolor
	virtual void setColor(const osg::Vec4 & color);
	//! \param idx Indeks obiektu
	//! \param color Kolor
	virtual void setColor(const unsigned int idx, const osg::Vec4 & color);
	//! \param visible Czy węzeł jest widoczny
	virtual void setVisible(const bool visible);
	//! \param idx Indeks obiektu
	//! \param visible Czy węzeł jest widoczny
	virtual void setVisible(const unsigned int idx, const bool visible);
	//! \param size Rozmiar
	virtual void setSize(const float size);
	//! \param idx Indeks obiektu
	//! \param size Rozmiar
	virtual void setSize(const unsigned int idx, const float size);

	virtual const osg::Vec4 & color(const unsigned int idx) const;
	virtual const bool visible(const unsigned int idx) const;
	virtual const float size(const unsigned int idx) const;

private:
	osg::ref_ptr<osg::Switch> node;
	FeaturesDescriptors featuresDescriptors;
};
DEFINE_SMART_POINTERS(TrajectoryDrawerManager);

#endif
