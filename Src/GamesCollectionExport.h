
#ifdef _WIN32
    #ifdef LETOGAMES_EXPORT_LIBRARY
        #define GAMES_COLLECTION_EXPORT __declspec(dllexport)
    #else
        #define GAMES_COLLECTION_EXPORT __declspec(dllimport)
    #endif
#else
    #define GAMES_COLLECTION_EXPORT //__attribute__((visibility("default")))
#endif
