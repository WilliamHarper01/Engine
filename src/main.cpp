#define SNES

#ifdef SNES
    #include "snesGraphicsEditor.cpp" 
#endif
#ifdef SPECTRUM
    #include "spectrum.cpp"
#endif

int main()
{
    try
    {
        #ifdef SNES
            snesGraphicsEditor();
        #endif 
        #ifdef SPECTRUM
            spectrum();
        #endif
    }
    catch (const std::exception& e) 
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}