/********************************************************************
	created:  2010/08/06
	created:  6:8:2010   18:30
	filename: VideoManager.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__VIDEOMANAGER_H__
#define __HEADER_GUARD__VIDEOMANAGER_H__

// tymczasowe
#ifndef ENGINE_API
#define ENGINE_API
#endif

////////////////////////////////////////////////////////////////////////////////
namespace vm {
////////////////////////////////////////////////////////////////////////////////

class ENGINE_API VideoManager
{
private:
  //! Instancja.
  static VideoManager* instance;
  //! Preferowany format piksela.
  PixelFormat prefferedFormat;
  //! Czy buforowanie ma być aktywne?
  bool enableBuffering;

private:
  //! Prywatny konstruktor - tylko dla fabryk.
  VideoManager();
  //! Prytwatny destruktor - tylko dla fabryk.
  ~VideoManager();
  
public:
  //! \return Instancja.
  static VideoManager* getInstance();
  //! Zwalnia wszystkie zasoby.
  static void destroyInstance();

  //! \return
  inline bool getEnableBuffering() const
  { 
    return enableBuffering;
  }
  //! \param enableBuffering
  inline void setEnableBuffering(bool enableBuffering) 
  { 
    this->enableBuffering = enableBuffering; 
  }
  //! \return
  inline PixelFormat getPrefferedFormat() const
  { 
    return prefferedFormat;
  }
  //! \param prefferedFormat
  inline void setPrefferedFormat(PixelFormat prefferedFormat) 
  { 
    this->prefferedFormat = prefferedFormat; 
  }

  //! 
  //! \param path
  VideoStream* createStream(const std::string& path);

private:
  //!
  //bool init();
};


////////////////////////////////////////////////////////////////////////////////
} // namespace vm
////////////////////////////////////////////////////////////////////////////////



#endif  // __HEADER_GUARD__VIDEOMANAGER_H__
