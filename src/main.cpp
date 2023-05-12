#ifdef SNES
    #include "../demos/snesGraphicsEditor.cpp" 
#endif
#ifdef SPECTRUM
    #include "../demos/spectrum.cpp"
#endif
#ifdef CUBESPIN
    #include "../demos/cubespin.cpp"
#endif

int main(int argc, char** argv)
{
    try
    {
        #ifdef SNES
            snesGraphicsEditor(argc, argv);
        #endif 
        #ifdef SPECTRUM
            spectrum(argc, argv);
        #endif
        #ifdef CUBESPIN
            cubespin(argc, argv);
        #endif
    }
    catch (const std::exception& e) 
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}