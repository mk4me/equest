/********************************************************************
    created:  2011/03/25
    created:  25:3:2011   16:01
    filename: VideoVisualizer.h
    author:	  Piotr Gwiazdowski
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_VIDEO__VIDEOVISUALIZER_H__
#define HEADER_GUARD_VIDEO__VIDEOVISUALIZER_H__

#include <string>
#include <osgui/QOsgWidgets.h>

namespace osgui {

	class AspectRatioKeeper;
}

namespace osgWidget {

	class Box;
	class Widget;

}

namespace vidlib {

	class VideoImage;

}

#include <corelib/IVisualizer.h>
#include <plugins/video/Wrappers.h>

//! Wizualizator wykresów.
class VideoVisualizer : public plugin::IVisualizer
{
    UNIQUE_ID("{82978BE1-6A5F-4CE8-9CCE-302F82256A20}");
	CLASS_DESCRIPTION("Video Visualizer", "Video Visualizer");

private:

	struct Refresher;
	struct WidgetUpdater;

	class VideoSerie : public plugin::IVisualizer::ISerie, public plugin::IVisualizer::ITimeAvareSerieFeatures
	{
	public:
		VideoSerie(VideoVisualizer * visualizer);

		virtual ~VideoSerie();

		virtual void setName(const std::string & name);

		virtual const std::string getName() const;

		virtual void setData(const utils::TypeInfo & requestedType, const core::VariantConstPtr & data);

		virtual const core::VariantConstPtr & getData() const;

		virtual const utils::TypeInfo & getRequestedDataType() const;

		virtual void update();

		//! \return długość kanału w sekundach
		virtual double getLength() const;

		virtual double getBegin() const;
		virtual double getEnd() const;

		//! Czas zawiera się między 0 a getLength()
		//! \param time Aktualny, lokalny czas kanału w sekundach
		virtual void setTime(double time);

        double getOffset() const;
        void setOffset(double val);

	private:

		void safeSetTime(const double time);

	private:
		VideoVisualizer * visualizer;
		std::string name;
		core::VariantConstPtr data;
		utils::TypeInfo requestedType;
        double offset;
        
    };

	friend class VideoSerie;

private:
    //! Viewer osg.
    osg::ref_ptr<osgui::QOsgDefaultWidget> viewer;
	//osgui::QOsgDefaultWidget * viewer;
    
    //! Czy używać textureRect?
    bool useTextureRect;
    //!
    double prevStreamTime;

	double currentStreamTime;
    //!
    int prevStreamWidth;

    //! Obiekt pilnujący współczynników proporcji widgetów.
    osg::ref_ptr<osgui::AspectRatioKeeper> ratioKeeper;
	//osgui::AspectRatioKeeper * ratioKeeper;
    //! Obszar roboczy.
    osg::ref_ptr<osgWidget::Box> workspace;
	//osgWidget::Box * workspace;
    //! Widget w którym rysujemy.
    //osg::ref_ptr<osgWidget::Widget> widget;
	osgWidget::Widget * widget;
    //! Bieżący obrazek.
    osg::ref_ptr<vidlib::VideoImage> streamImage;
	//!
	VideoStreamPtr stream;

	ISerie * currentSerie_;

private:

	void refreshImage();

	bool getImage();

public:
    //!
    VideoVisualizer();
    //!
    virtual ~VideoVisualizer();

public:
    //! 
    //! \param width
    //! \param height
    void refresh(float width, float height);
    //!
    void updateWidget();

public:
    //! \see IVisualizer::create
    virtual plugin::IVisualizer* create() const;
    //! \see IVisualizer::getSlotInfo
    virtual void getSupportedTypes(utils::TypeInfoList & supportedTypes) const;
    //! Nic nie robi.
    //! \see IVisualizer::update
    virtual void update(double deltaTime);
    //! \see IVisualizer::createWidget
    virtual QWidget* createWidget();
    //! \see IVisualizer::createIcon
    virtual QIcon* createIcon();
    virtual QPixmap takeScreenshot() const;

    virtual int getMaxDataSeries() const;

    //! \return Seria danych która można ustawiac - nazwa i dane, nie zarządza ta seria danych - czasem jej zycia, my zwalniamy jej zasoby!!
    virtual plugin::IVisualizer::ISerie* createSerie(const utils::TypeInfo & requestedType, const core::VariantConstPtr & data);

    virtual plugin::IVisualizer::ISerie* createSerie(const plugin::IVisualizer::ISerie * serie);

	virtual plugin::IVisualizer::ISerie* createSerie(const plugin::IVisualizer::ISerie * serie, const utils::TypeInfo & requestedType, const core::VariantConstPtr & data);

    //! \param serie Seria danych do usunięcia, nie powinien usuwać tej serii! Zarządzamy nią my!!
    virtual void removeSerie(plugin::IVisualizer::ISerie* serie);

	virtual void setActiveSerie(plugin::IVisualizer::ISerie * serie);

	virtual const plugin::IVisualizer::ISerie * getActiveSerie() const;

	virtual plugin::IVisualizer::ISerie * getActiveSerie();

private:

	void clear();
};


#endif  // HEADER_GUARD_VIDEO__VIDEOVISUALIZER_H__
