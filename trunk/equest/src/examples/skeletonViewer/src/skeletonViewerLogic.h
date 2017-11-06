#ifndef HEADER_GUARD_SKELETONVIEWERLOGIC_H
#define HEADER_GUARD_SKELETONVIEWERLOGIC_H

#include <QtGUi/QTreeWidgetItem>
#include <kinematiclib/SkeletalParsers.h>

typedef osg::ref_ptr<osg::Geode> GeodePtr;
class MainWindow;

// Kontroler aplikacji
class SkeletonViewerLogic
{
public:
    SkeletonViewerLogic();

public:
    // akcesory
    SkeletalVisualizationSchemePtr getVisualization() const { return visualization; }
    void setVisualization(SkeletalVisualizationSchemePtr val) { visualization = val; }
    MainWindow* getMainWindow() const { return mainWindow; }
    void setMainWindow(MainWindow* val);
    double getTime() const { return time; }
    void setTime(double time);
public:
    //podstawowe funkcje
    void pause();
    void start();
    void stop();
    void update(bool updateSlider = true);

public:
    void createTree(kinematic::hAnimJointPtr parentJoint, QTreeWidgetItem* parentItem);
    void createMarkersCrowd(kinematic::JointPtr joint, const osg::Vec3& position = osg::Vec3());
    void loadBVH(const std::string& bvhFileName);
    void loadAsfAmc(const std::string& asfFile, const std::string& amcFile);

private:
    GeodePtr createMarker(const osg::Vec4& color, float scale = 1.0f) const;
    void addPoint(const osg::Vec3& point);
    void addLine(const osg::Vec3& from, const osg::Vec3& to);
    void setModel(kinematic::SkeletalModelPtr skeletalModel);

private:
    SkeletalVisualizationSchemePtr visualization;
    MainWindow* mainWindow;
    double time;
    bool loaded;
};

typedef boost::shared_ptr<SkeletonViewerLogic> SkeletonViewerLogicPtr;

#endif
