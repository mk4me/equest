#include "PCH.h"
#include "VidLibPrivate.h"
#include <vidlib/FileSequenceVideoStream.h>

namespace vidlib 
{
    FileSequenceVideoStream::FileSequenceVideoStream( const std::string& name, double framerate, const std::vector<std::string>& files, ILoader* loader ) :
    VideoStream(), wantedTime(INVALID_TIMESTAMP), currentFrame(-1), loader(loader)
    {
        utils::zero(frame);
        init(name, framerate, files, loader);
    }

    FileSequenceVideoStream::~FileSequenceVideoStream()
    {
        if ( loader ) {
            delete loader;
        }
        // loader powinien sam zwolnić pamięć
        // frame.free();
    }

    bool FileSequenceVideoStream::init( const std::string& name, double framerate, const std::vector<std::string>& files, ILoader* loader )
    {
        if ( !(this->loader = loader) ) {
            VIDLIB_ERROR(VideoError("Image loader is null"));
        }
        if ( files.empty() ) {
            VIDLIB_ERROR(VideoError("files.empty()"));
        }
        // kopiujemy nazwy plików
        this->files.assign(files.begin(), files.end());

        // wczytanie pierwszej ramki i określenie własności strumienia
        currentFrame = 0;
        wantedTime = 0;
        if (!loader->readImage(*files.begin(), frame)) {
            VIDLIB_ERROR(VideoError(std::string("Error loadig file ") + *files.begin()));
        }

        onAfterInit(name, framerate, (files.size()-1) * 1/framerate, frame.format, frame.width, frame.height, 1.0);
        return true;
    }


    VideoStream* FileSequenceVideoStream::clone() const
    {
        std::unique_ptr<ILoader> loaderCloned(loader->clone());
		std::unique_ptr<FileSequenceVideoStream> cloned(new FileSequenceVideoStream(getSource(), getFramerate(), files, loaderCloned.get()));
		loaderCloned.release();
        cloned->setTime(wantedTime);
        return cloned.release();
    }

    double FileSequenceVideoStream::getNextFrameTimestamp() const
    {
        // TODO: być może trzeba będzie poprawić
        UTILS_ASSERT(!files.empty());
        return (currentFrame < 0 || currentFrame >= int(files.size()-1)) ? INVALID_TIMESTAMP : (currentFrame+1) * getFrameDuration();
    }

    double FileSequenceVideoStream::getFrameTimestamp() const
    {
        UTILS_ASSERT(!files.empty());
        return currentFrame < 0 ? INVALID_TIMESTAMP : currentFrame * getFrameDuration();
    }

    bool FileSequenceVideoStream::setTime( double time )
    {
        if (time < 0 || time > getDuration()) {
            VIDLIB_ERROR("time < 0 && time > getDuration()");
        }

        int requiredFrame = static_cast<int>(time * getFramerate());
        if ( requiredFrame == currentFrame ) {
            // nic nie trzeba robić
            return true;
        } else {
            UTILS_ASSERT(requiredFrame <= int(files.size()));
            // wczytanie nowej ramki
            const std::string& path = files[requiredFrame];
            Picture newFrame;
            utils::zero(newFrame);
            if (!loader->readImage(path, newFrame)) {
                VIDLIB_ERROR(VideoError(std::string("Error loadig file ") + path));
            } else {
                // czy nowa ramka jest w formacie poprzedniej?
                if (newFrame.format != frame.format || newFrame.width != frame.width || newFrame.height != frame.height ||
                    newFrame.dataWidth != frame.dataWidth || newFrame.dataHeight != frame.dataHeight) {
                        VIDLIB_ERROR(VideoError(std::string("Picture loaded from ") + path + " differs from stream format."));
                }
                // wszystko się udało
                wantedTime = time;
                currentFrame = requiredFrame;
                frame = newFrame;
                return true;
            }
        }
    }

    double FileSequenceVideoStream::getTime() const
    {
        return wantedTime;
    }

    bool FileSequenceVideoStream::getData( Picture & dst )
    {
        dst = frame;
        return true;
    }


    bool FileSequenceVideoStream::readNext()
    {
        return false;
    }
} // namespace vidlib
