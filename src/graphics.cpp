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

//source: https://gist.github.com/postspectacular/2a4a8db092011c6743a7

float fract(float x) { return x - int(x); }

float mix(float a, float b, float t) { return a + (b - a) * t; }

float step(float e, float x) { return x < e ? 0.0 : 1.0; }

void Color::from5BitHSV(short hsv)
{
    float h, s, v;
    h = (hsv & 0b11111)/(float)0b11111;
    s = ((hsv >> 5) & 0b11111)/(float)0b11111;
    v = ((hsv >> 10) & 0b11111)/(float)0b11111;

    h = std::min(h, 1.0f);
    s = std::min(s, 1.0f);
    v = std::min(v, 1.0f);

    a = 1.0f;
    
    r = v * mix(1.0, std::min(std::max(abs(fract(h + 1.0000000) * 6.0 - 3.0) - 1.0, 0.0), 1.0), s);
    g = v * mix(1.0, std::min(std::max(abs(fract(h + 0.6666666) * 6.0 - 3.0) - 1.0, 0.0), 1.0), s);
    b = v * mix(1.0, std::min(std::max(abs(fract(h + 0.3333333) * 6.0 - 3.0) - 1.0, 0.0), 1.0), s);
}

short Color::to5BitHSV()
{
    float hue, sat, val;
    
    float s = step(b, g);
    float px = mix(b, g, s);
    float py = mix(g, b, s);
    float pz = mix(-1.0, 0.0, s);
    float pw = mix(0.6666666, -0.3333333, s);
    s = step(px, r);
    float qx = mix(px, r, s);
    float qz = mix(pw, pz, s);
    float qw = mix(r, px, s);
    float d = qx - std::min(qw, py);
    hue = abs(qz + (qw - py) / (6.0 * d + 1e-10));
    sat = d / (qx + 1e-10);
    val = qx;

    short result = 0;
    result = ((short)(val*31.0f));
    result = result << 5;
    result += ((short)(sat*31.0f));
    result = result << 5;
    result += ((short)(hue*31.0f));
    return result;
}

std::vector<int> Font::getAscii()
{
    std::vector<int> chars;
    chars.reserve(128);
    for (int i=33; i<127; i++)
    {
        chars.push_back(i);
    }
    return chars;
}