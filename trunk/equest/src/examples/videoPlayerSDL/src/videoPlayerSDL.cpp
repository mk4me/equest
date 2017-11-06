#include <SDL.h>
#include <SDL_thread.h>
#include <Windows.h>
#include <iostream>
#include <time.h>
#include <vidlib/FFmpegVideoStream.h>
#include <utils/LeakDetection.h>
#include <vidlib/BufferedVideoStream.h>
#include <vidlib/FileSequenceVideoStream.h>
#include <vidlib/Config.h>
#include <boost/filesystem.hpp>
#include <vidlib/osg/OsgAdapter.h>


#ifdef VIDLIB_ENABLE_OSG
UTILS_PUSH_WARNINGS
#include <osgDB/ReadFile>
UTILS_POP_WARNINGS

class OsgLoader : public vidlib::FileSequenceVideoStream::ILoader
{
    osg::ref_ptr<osg::Image> img;
    osg::ref_ptr<osgDB::Options> options;

public:
    OsgLoader(osgDB::Options* options) 
    : options(options)
    {
    }

    bool readImage(const std::string& path, vidlib::Picture& picture)
    {
        img = osgDB::readImageFile(path, options.get());
        if ( img ) {
            if (img->getOrigin() == osg::Image::BOTTOM_LEFT) {
                img->flipVertical();
                img->setOrigin(osg::Image::TOP_LEFT);
            }
            if ( img->getPixelFormat() == GL_RGB ) {
                picture.format = vidlib::PixelFormatRGB24;
                picture.width = img->s();
                picture.height = img->t();
                picture.dataHeight = picture.height;
                picture.dataWidth = 3 * picture.width;
                picture.data = (unsigned char*)(img->getDataPointer());
                return true;
            } else {
                return false;
            }
        } else {
            return false;
        }
        
    }


    ILoader* clone() const
    {
        return new OsgLoader(options);
    }
};

#endif // VIDLIB_ENABLE_OSG

#ifdef WIN32
#undef main /* We don't want SDL to override our main() */
#endif


int main(int argc, char **argv)
{
    using namespace vidlib;
    using namespace std;
    namespace fs = boost::filesystem;

    // czy mamy parametr?
    if ( argc == 1 ) {
        cerr << "Usage: " << argv[0] << " <filename>" << endl;
        return 1;
    }

    // otwieramy źródło
    VideoStream * input = NULL;
    SDL_Surface * screen = NULL;
    SDL_Surface * bmp = NULL;

    try {

        fs::path path = argv[1];
        if (fs::is_directory(path)) {
            // wczytanie listy plików
            std::vector<std::string> files;
            for ( fs::directory_iterator it(path), last; it != last; ++it ) {
                files.push_back(it->path().string());
            }

#ifdef VIDLIB_ENABLE_OSG
            //osgDB::Options* options = new osgDB::Options();
            //options->setObjectCacheHint( osgDB::Options::CACHE_IMAGES );
            //osgDB::Registry::instance()->setOptions(options);
            osgDB::Options* options = new osgDB::Options();
            options->setObjectCacheHint( osgDB::Options::CACHE_NONE );

            input = new FileSequenceVideoStream(path.string(), 25, files, new OsgLoader(new osgDB::Options()));
#else
            throw runtime_error("To enable reading files sequence vidlib must have OSG enabled.");
#endif

        } else {
            // strumień dla pliku
            input = new FFmpegVideoStream( path.string() );
        }

        // inicjalizacja SDL'a
        SDL_Init( SDL_INIT_VIDEO );
        SDL_EnableKeyRepeat(100, 10);
        SDL_WM_SetCaption("SDL/ffmpeg player", "SDL/ffmpeg player");
        SDL_Rect rect;
        rect.x = 0;
        rect.y = 0;
        rect.w = input->getWidth();
        rect.h = input->getHeight();
        screen = SDL_SetVideoMode( rect.w, rect.h, 24, SDL_HWSURFACE | SDL_ASYNCBLIT | SDL_HWACCEL ); 
        bmp = SDL_CreateRGBSurface(SDL_SWSURFACE, rect.w, rect.h, 24, 0x000000FF, 0x0000ff00, 0x00ff0000, 0 );

        // czas
        double time = 0;
        double timeScale = 1;

        // event
        SDL_Event event;
        bool quit = false;

        // czas rozpoczęcia ramki
        clock_t begin = clock();
        double bufferedTimeScale = 0.0f;

        // pętla
        while (!quit && !input->isEndOfStream()) {

            // próba ustawienia czasu
            time = min(time, input->getDuration());
            input->setTime(time);

            // kopiujemy i wyświetlamy ramke
            SDL_LockSurface(bmp);
            Picture picture;
            picture.format = PixelFormatRGB24;
            picture.data = reinterpret_cast<unsigned char*>(bmp->pixels);
            picture.dataWidth = bmp->pitch;
            picture.width = bmp->w;
            picture.height = picture.dataHeight = bmp->h;
            input->getFrame(picture);
            SDL_UnlockSurface(bmp);
            SDL_BlitSurface( bmp, NULL, screen, NULL );
            SDL_Flip( screen );

            // zajmujemy się eventami
            while (SDL_PollEvent(&event)) {
                switch ( event.type ) {
          case SDL_QUIT:
              quit = true;
              break; 
          case SDL_KEYDOWN:
              switch ( event.key.keysym.sym ) {
          case SDLK_ESCAPE:
              quit = true;
              break;
          case SDLK_PLUS:
          case SDLK_EQUALS:
              timeScale *= 2;
              break;
          case SDLK_MINUS:
              timeScale /= 2;
              break;
          case SDLK_LEFT:
              if ( event.key.keysym.mod & KMOD_LSHIFT ) {
                  time = max(time - input->getFrameDuration(), 0);
              } else {
                  time = max(time - 1, 0);
              }
              break;
          case SDLK_RIGHT:
              if ( event.key.keysym.mod & KMOD_LSHIFT ) {
                  time = min(time + input->getFrameDuration(), input->getDuration());
              } else {
                  time = min(time + 1, input->getDuration());
              }
              break;
          case SDLK_SPACE:
              if ( timeScale == 0.0 ) {
                  timeScale = bufferedTimeScale;
              } else {
                  bufferedTimeScale = timeScale;
                  timeScale = 0.0;
              }
              break;
          default:
              break;
              }
              break;
          default:
              break;
                }
            }

            // drukujemy spacje aby zamazać rzeczy z poprzedniego odpisania
            cout << "time: " << time << "\t timestamp: " << input->getFrameTimestamp() << "\t timescale: " << timeScale << "              \r";
            cout.flush();

            // ile czasu minęło?
            clock_t delta = clock() - begin;
            time += delta / double(CLOCKS_PER_SEC) * timeScale;
            begin = clock();
        }
    } catch ( VideoError & error ) {
        cerr << "Video ERROR: " << error.what() << endl;
    } catch ( exception& error ) {
        cerr << "ERROR: " << error.what() << endl;
    }

    if ( input ) {
        delete input;
    }
    if ( bmp ) {
        SDL_FreeSurface(bmp);
    }
    if ( screen ) { 
        SDL_FreeSurface(screen);
    }
    SDL_Quit();
    return 0; 
}
