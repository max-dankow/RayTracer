#ifndef RAYTRACER_PICTURE_H
#define RAYTRACER_PICTURE_H

#include <assert.h>
#include <cstdlib>
#include <vector>
#include <limits>

// todo: инкаплусировать всю работу с цветами в классе
// Представляет цвет в формате RGB каждая компонента - насыщенность от 0 до 1.
struct Color {
    Color(double r = 0, double g = 0, double b = 0) : r(r), g(g), b(b) {
        assert(r >= 0 && r <= 1 && g >= 0 && g <= 1 && b >= 0 && b <= 1);
    }

    Color operator*(double k) {
        return Color(std::min(r * k, 1.), std::min(g * k, 1.), std::min(b * k, 1.));
    }

    Color operator+(const Color &other) {
        return Color(std::min(this->r + other.r, 1.), std::min(this->g + other.g, 1.), std::min(this->b + other.b, 1.));
    }

    double r, g, b;
    double h, s, v;
};

class Picture {
public:
    explicit Picture(size_t width, size_t height) : width(width), height(height) {
        colorMap.assign(width * height, Color());
    }

    const Color &getAt(size_t col, size_t row) const {
        assert(col >= 0 && row >= 0 && col < width && row < height);
        return colorMap[width * row + col];
    }

    void setAt(size_t col, size_t row, const Color &pixel) {
        assert(col >= 0 && row >= 0 && col < width && row < height);
        colorMap[width * row + col] = pixel;
    }

    size_t getHeight() const {
        return height;
    }

    size_t getWidth() const {
        return width;
    }

private:
    size_t width, height;
    std::vector<Color> colorMap;
};

// todo: исправить на hsl
// todo: переписать нормально
static const Color CL_WHITE = Color(1, 1, 1);
static const Color CL_BLACK = Color(0, 0, 0);
/*
typedef struct {
    double r;       // percent
    double g;       // percent
    double b;       // percent
} rgb;

typedef struct {
    double h;       // angle in degrees
    double s;       // percent
    double v;       // percent
} hsv;*/

static Color   rgb2hsv(Color in);
static Color   hsv2rgb(Color in);

Color rgb2hsv(Color in)
{
    Color         out;
    double      min, max, delta;

    min = in.r < in.g ? in.r : in.g;
    min = min  < in.b ? min  : in.b;

    max = in.r > in.g ? in.r : in.g;
    max = max  > in.b ? max  : in.b;

    out.v = max;                                // v
    delta = max - min;
    if (delta < 0.00001)
    {
        out.s = 0;
        out.h = 0; // undefined, maybe nan?
        return out;
    }
    if( max > 0.0 ) { // NOTE: if Max is == 0, this divide would cause a crash
        out.s = (delta / max);                  // s
    } else {
        // if max is 0, then r = g = b = 0
        // s = 0, v is undefined
        out.s = 0.0;
        out.h = std::numeric_limits<double>::quiet_NaN();
        return out;
    }
    if( in.r >= max )                           // > is bogus, just keeps compilor happy
        out.h = ( in.g - in.b ) / delta;        // between yellow & magenta
    else
    if( in.g >= max )
        out.h = 2.0 + ( in.b - in.r ) / delta;  // between cyan & yellow
    else
        out.h = 4.0 + ( in.r - in.g ) / delta;  // between magenta & cyan

    out.h *= 60.0;                              // degrees

    if( out.h < 0.0 )
        out.h += 360.0;

    return out;
}


Color hsv2rgb(Color in)
{
    double      hh, p, q, t, ff;
    long        i;
    Color         out(in);

    if(in.s <= 0.0) {       // < is bogus, just shuts up warnings
        out.r = in.v;
        out.g = in.v;
        out.b = in.v;
        return out;
    }
    hh = in.h;
    if(hh >= 360.0) hh = 0.0;
    hh /= 60.0;
    i = (long)hh;
    ff = hh - i;
    p = in.v * (1.0 - in.s);
    q = in.v * (1.0 - (in.s * ff));
    t = in.v * (1.0 - (in.s * (1.0 - ff)));

    switch(i) {
        case 0:
            out.r = in.v;
            out.g = t;
            out.b = p;
            break;
        case 1:
            out.r = q;
            out.g = in.v;
            out.b = p;
            break;
        case 2:
            out.r = p;
            out.g = in.v;
            out.b = t;
            break;

        case 3:
            out.r = p;
            out.g = q;
            out.b = in.v;
            break;
        case 4:
            out.r = t;
            out.g = p;
            out.b = in.v;
            break;
        case 5:
        default:
            out.r = in.v;
            out.g = p;
            out.b = q;
            break;
    }
    return out;
}



class RGB
{
public:
    unsigned char R;
    unsigned char G;
    unsigned char B;

    RGB(unsigned char r, unsigned char g, unsigned char b)
    {
        R = r;
        G = g;
        B = b;
    }

    bool Equals(RGB rgb)
    {
        return (R == rgb.R) && (G == rgb.G) && (B == rgb.B);
    }
};

class HSL
{
public:
    int H;
    float S;
    float L;

    HSL(int h, float s, float l)
    {
        H = h;
        S = s;
        L = l;
    }

    bool Equals(HSL hsl)
    {
        return (H == hsl.H) && (S == hsl.S) && (L == hsl.L);
    }
};

static float HueToRGB(float v1, float v2, float vH) {
    if (vH < 0)
        vH += 1;

    if (vH > 1)
        vH -= 1;

    if ((6 * vH) < 1)
        return (v1 + (v2 - v1) * 6 * vH);

    if ((2 * vH) < 1)
        return v2;

    if ((3 * vH) < 2)
        return (v1 + (v2 - v1) * ((2.0f / 3) - vH) * 6);

    return v1;
}

static RGB HSLToRGB(HSL hsl) {
    unsigned char r = 0;
    unsigned char g = 0;
    unsigned char b = 0;

    if (hsl.S == 0)
    {
        r = g = b = (unsigned char)(hsl.L * 255);
    }
    else
    {
        float v1, v2;
        float hue = (float)hsl.H / 360;

        v2 = (hsl.L < 0.5) ? (hsl.L * (1 + hsl.S)) : ((hsl.L + hsl.S) - (hsl.L * hsl.S));
        v1 = 2 * hsl.L - v2;

        r = (unsigned char)(255 * HueToRGB(v1, v2, hue + (1.0f / 3)));
        g = (unsigned char)(255 * HueToRGB(v1, v2, hue));
        b = (unsigned char)(255 * HueToRGB(v1, v2, hue - (1.0f / 3)));
    }

    return RGB(r, g, b);
}


static float Min(float a, float b) {
    return a <= b ? a : b;
}

static float Max(float a, float b) {
    return a >= b ? a : b;
}

static HSL RGBToHSL(RGB rgb) {
    HSL hsl = HSL(0, 0, 0);

    float r = (rgb.R / 255.0f);
    float g = (rgb.G / 255.0f);
    float b = (rgb.B / 255.0f);

    float min = Min(Min(r, g), b);
    float max = Max(Max(r, g), b);
    float delta = max - min;

    hsl.L = (max + min) / 2;

    if (delta == 0)
    {
        hsl.H = 0;
        hsl.S = 0.0f;
    }
    else
    {
        hsl.S = (hsl.L <= 0.5) ? (delta / (max + min)) : (delta / (2 - max - min));

        float hue;

        if (r == max)
        {
            hue = ((g - b) / 6) / delta;
        }
        else if (g == max)
        {
            hue = (1.0f / 3) + ((b - r) / 6) / delta;
        }
        else
        {
            hue = (2.0f / 3) + ((r - g) / 6) / delta;
        }

        if (hue < 0)
            hue += 1;
        if (hue > 1)
            hue -= 1;

        hsl.H = (int)(hue * 360);
    }

    return hsl;
}

#endif //RAYTRACER_PICTURE_H
