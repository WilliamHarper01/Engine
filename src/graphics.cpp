#include "graphics.h"

void Color::from5Bit(short color)
{
    r = (color & 0b11111)/(float)0b11111;
    g = ((color >> 5) & 0b11111)/(float)0b11111;
    b = ((color >> 10) & 0b11111)/(float)0b11111;

    r = std::min(r, 1.0f);
    g = std::min(g, 1.0f);
    b = std::min(b, 1.0f);

    a = 1.0f;
}

short Color::to5Bit()
{
    short result = 0;
    result = ((short)(b*31.0f));
    result = result << 5;
    result += ((short)(g*31.0f));
    result = result << 5;
    result += ((short)(r*31.0f));
    return result;
}

void Color::from5BitHSV(short hsv)
{
    V3 in;
    in.x = (hsv & 0b11111)/(float)0b11111;
    in.y = ((hsv >> 5) & 0b11111)/(float)0b11111;
    in.z = ((hsv >> 10) & 0b11111)/(float)0b11111;

    in.x = std::min(in.x, 1.0f);
    in.y = std::min(in.x, 1.0f);
    in.z = std::min(in.x, 1.0f);

    a = 1.0f;
    
    double      hh, p, q, t, ff;
    long        i;


    if(in.y <= 0.0) {       // < is bogus, just shuts up warnings
        r = in.z;
        g = in.z;
        b = in.z;
        return;
    }
    hh = in.x;
    if(hh >= 360.0) hh = 0.0;
    hh /= 60.0;
    i = (long)hh;
    ff = hh - i;
    p = in.z * (1.0 - in.y);
    q = in.z * (1.0 - (in.y * ff));
    t = in.z * (1.0 - (in.y * (1.0 - ff)));

    switch(i) {
    case 0:
        r = in.z;
        g = t;
        b = p;
        break;
    case 1:
        r = q;
        g = in.z;
        b = p;
        break;
    case 2:
        r = p;
        g = in.z;
        b = t;
        break;

    case 3:
        r = p;
        g = q;
        b = in.z;
        break;
    case 4:
        r = t;
        g = p;
        b = in.z;
        break;
    case 5:
    default:
        r = in.z;
        g = p;
        b = q;
        break;
    }
    
      
}

short Color::to5BitHSV()
{
    V3 out;
    double      min, max, delta;

    min = r < g ? r : g;
    min = min  < b ? min  : b;

    max = r > g ? r : g;
    max = max  > b ? max  : b;

    out.z = max;                                // v
    delta = max - min;
    if (delta < 0.00001)
    {
        out.y = 0;
        out.x = 0; // undefined, maybe nan?
        goto bits;
    }
    if( max > 0.0 ) { // NOTE: if Max is == 0, this divide would cause a crash
        out.y = (delta / max);                  // s
    } else {
        // if max is 0, then r = g = b = 0              
        // s = 0, h is undefined
        out.y = 0.0;
        out.x = NAN;                            // its now undefined
        goto bits;
    }
    if( r >= max )                           // > is bogus, just keeps compilor happy
        out.x = ( g - b ) / delta;        // between yellow & magenta
    else
    if( g >= max )
        out.x = 2.0 + ( b - r ) / delta;  // between cyan & yellow
    else
        out.x = 4.0 + ( r - g ) / delta;  // between magenta & cyan

    out.x *= 60.0;                              // degrees

    if( out.x < 0.0 )
        out.x += 360.0;

bits:
    short result = 0;
    result = ((short)(out.z*31.0f));
    result = result << 5;
    result += ((short)(out.y*31.0f));
    result = result << 5;
    result += ((short)(out.x*31.0f));
    return result;
}