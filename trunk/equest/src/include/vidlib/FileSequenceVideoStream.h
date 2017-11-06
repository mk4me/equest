/********************************************************************
    created:  2011/04/08
    created:  8:4:2011   13:39
    filename: FileSequenceVideoStream.h
    author:	  Piotr Gwiazdowski
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_VIDLIB__FILESEQUENCEVIDEOSTREAM_H__
#define HEADER_GUARD_VIDLIB__FILESEQUENCEVIDEOSTREAM_H__

#include <string>
#include <vector>
#include <utils/Utils.h>
#include <vidlib/Config.h>
#include <vidlib/VideoStream.h>

namespace vidlib 
{
    UTILS_PUSH_WARNINGS
    UTILS_DISABLE_DLL_INTERFACE_WARNING

    //! Strumień video oparty o fizyczny pliki. Do wczytywania plików używa obiektu ILoader
    class VIDLIB_EXPORT FileSequenceVideoStream : public VideoStream
    {
    public:
        //!
        class ILoader 
        {
        public:
            //!
            virtual ~ILoader() {}
            //! \param path Ścieżka z której należy odczytać plik.
            //! \param picture Zdjęcie wynikowe. Powinno mu się przypisywać pola, ale nie alokować!
            //! \return true gdy się udało, false w przeciwnym razie
            virtual bool readImage(const std::string& path, Picture& picture) = 0;
            //! Klonuje bieżącego loadera. To, czy ma to być płytka czy głęboka kopia, zależy
            //! od logiki loadera; gdy jest to możliwe płytka kopia jest preferowana.
            virtual ILoader* clone() const = 0;
        };

    private:
        //! Lista plików.
        std::vector<std::string> files;
        //! Bieżąca ramka.
        Picture frame;
        //! Wybrany czas.
        double wantedTime;
        //! Numer bieżącej ramki.
        int currentFrame;
        //! Interfejs do ładowania obrazków.
        ILoader* loader;

    public:
        //! \param name
        //! \param framerate
        //! \param files
        //! \param loader Obiekt przejmowany na własność.
        FileSequenceVideoStream(const std::string& name, double framerate, const std::vector<std::string>& files, ILoader* loader);
        //!
        virtual ~FileSequenceVideoStream();

    // VideoStream
    public:
        //!
        virtual VideoStream* clone() const;
        //!
        virtual bool readNext();
        //! \return Pozycja w źródle.
        virtual double getTime() const;
        //! \param time Pozycja w źródle
        virtual bool setTime(double time);
        //! \return Prawdziwy timestamp ramki.
        virtual double getFrameTimestamp() const;
        //! \return Prawdziwy timestamp kolejnej ramki.
        virtual double getNextFrameTimestamp() const;

    protected:
        //! \param dst
        virtual bool getData(Picture & /*dst*/);

    private:
        //! Inicjalizacja.
        //! \param name
        //! \param framerate
        //! \param files
        //! \param loader
        bool init(const std::string& name, double framerate, const std::vector<std::string>& files, ILoader* loader);
    };

    UTILS_POP_WARNINGS
} // namespace vidlib


#endif  // HEADER_GUARD_VIDLIB__FILESEQUENCEVIDEOSTREAM_H__
