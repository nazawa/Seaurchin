#include "Easing.h"

double Easing::Linear(double time, double timeDuration, double startValue, double valueDuration)
{
    return valueDuration * (time / timeDuration) + startValue;
}

double Easing::InQuad(double time, double timeDuration, double startValue, double valueDuration)
{
    return valueDuration * (time /= timeDuration) * time + startValue;
}

double Easing::OutQuad(double time, double timeDuration, double startValue, double valueDuration)
{
    return -valueDuration * (time /= timeDuration) * (time - 2) + startValue;
}

double Easing::InOutQuad(double time, double timeDuration, double startValue, double valueDuration)
{
    if ((time /= timeDuration / 2) < 1) return valueDuration / 2 * time * time + startValue;
    return -valueDuration / 2 * ((--time) * (time - 2) - 1) + startValue;
}

double Easing::InCubic(double time, double timeDuration, double startValue, double valueDuration)
{
    return valueDuration * (time /= timeDuration) * time * time + startValue;
}

double Easing::OutCubic(double time, double timeDuration, double startValue, double valueDuration)
{
    return valueDuration * ((time = time / timeDuration - 1) * time * time + 1) + startValue;
}

double Easing::InOutCubic(double time, double timeDuration, double startValue, double valueDuration)
{
    if ((time /= timeDuration / 2) < 1) return valueDuration / 2 * time * time * time + startValue;
    return valueDuration / 2 * ((time -= 2) * time * time + 2) + startValue;
}

double Easing::InQuart(double time, double timeDuration, double startValue, double valueDuration)
{
    return valueDuration * (time /= timeDuration) * time * time * time + startValue;
}

double Easing::OutQuart(double time, double timeDuration, double startValue, double valueDuration)
{
    return -valueDuration * ((time = time / timeDuration - 1) * time * time * time - 1) + startValue;
}

double Easing::InOutQuart(double time, double timeDuration, double startValue, double valueDuration)
{
    if ((time /= timeDuration / 2) < 1) return valueDuration / 2 * time * time * time * time + startValue;
    return -valueDuration / 2 * ((time -= 2) * time * time * time - 2) + startValue;
}

double Easing::InQuint(double time, double timeDuration, double startValue, double valueDuration)
{
    return valueDuration * (time /= timeDuration) * time * time * time * time + startValue;
}

double Easing::OutQuint(double time, double timeDuration, double startValue, double valueDuration)
{
    return valueDuration * ((time = time / timeDuration - 1) * time * time * time * time + 1) + startValue;
}

double Easing::InOutQuint(double time, double timeDuration, double startValue, double valueDuration)
{
    if ((time /= timeDuration / 2) < 1) return valueDuration / 2 * time * time * time * time * time + startValue;
    return valueDuration / 2 * ((time -= 2) * time * time * time * time + 2) + startValue;
}

double Easing::InSine(double time, double timeDuration, double startValue, double valueDuration)
{
    return -valueDuration * cos(time / timeDuration * (M_PI / 2)) + valueDuration + startValue;
}

double Easing::OutSine(double time, double timeDuration, double startValue, double valueDuration)
{
    return valueDuration * sin(time / timeDuration * (M_PI / 2)) + startValue;
}

double Easing::InOutSine(double time, double timeDuration, double startValue, double valueDuration)
{
    return -valueDuration / 2 * (cos(M_PI * time / timeDuration) - 1) + startValue;
}

double Easing::InExpo(double time, double timeDuration, double startValue, double valueDuration)
{
    return (time == 0) ? startValue : valueDuration * pow(2, 10 * (time / timeDuration - 1)) + startValue;
}

double Easing::OutExpo(double time, double timeDuration, double startValue, double valueDuration)
{
    return (time == timeDuration) ? startValue + valueDuration : valueDuration * (-pow(2, -10 * time / timeDuration) + 1) + startValue;
}

double Easing::InOutExpo(double time, double timeDuration, double startValue, double valueDuration)
{
    if (time == 0) return startValue;
    if (time == timeDuration) return startValue + valueDuration;
    if ((time /= timeDuration / 2) < 1) return valueDuration / 2 * pow(2, 10 * (time - 1)) + startValue;
    return valueDuration / 2 * (-pow(2, -10 * --time) + 2) + startValue;
}

double Easing::InCircle(double time, double timeDuration, double startValue, double valueDuration)
{
    return -valueDuration * (sqrt(1 - (time /= timeDuration) * time) - 1) + startValue;
}

double Easing::OutCircle(double time, double timeDuration, double startValue, double valueDuration)
{
    return valueDuration * sqrt(1 - (time = time / timeDuration - 1) * time) + startValue;
}

double Easing::InOutCircle(double time, double timeDuration, double startValue, double valueDuration)
{
    if ((time /= timeDuration / 2) < 1) return -valueDuration / 2 * (sqrt(1 - time * time) - 1) + startValue;
    return valueDuration / 2 * (sqrt(1 - (time -= 2) * time) + 1) + startValue;
}

double Easing::InElastic(double time, double timeDuration, double startValue, double valueDuration)
{
    double s = 1.70158; double p = 0.0; double a = valueDuration;
    if (time == 0) return startValue; if ((time /= timeDuration) == 1) return startValue + valueDuration; if (p == 0) p = timeDuration * .3;
    if (a < abs(valueDuration))
    {
        a = valueDuration; s = p / 4;
    }
    else
    {
        s = p / (2 * M_PI) * asin(valueDuration / a);
    }
    return -(a * pow(2, 10 * (time -= 1)) * sin((time * timeDuration - s) * (2 * M_PI) / p)) + startValue;
}

double Easing::OutElastic(double time, double timeDuration, double startValue, double valueDuration)
{
    double s = 1.70158; double p = 0.0; double a = valueDuration;
    if (time == 0) return startValue; if ((time /= timeDuration) == 1) return startValue + valueDuration; if (p == 0) p = timeDuration * .3;
    if (a < abs(valueDuration))
    {
        a = valueDuration; s = p / 4;
    }
    else
    {
        s = p / (2 * M_PI) * asin(valueDuration / a);
    }
    return a * pow(2, -10 * time) * sin((time * timeDuration - s) * (2 * M_PI) / p) + valueDuration + startValue;
}

double Easing::InOutElastic(double time, double timeDuration, double startValue, double valueDuration)
{
    double s = 1.70158; double p = 0.0; double a = valueDuration;
    if (time == 0) return startValue; if ((time /= timeDuration / 2) == 2) return startValue + valueDuration; if (p == 0) p = timeDuration * (.3 * 1.5);
    if (a < abs(valueDuration))
    {
        a = valueDuration; s = p / 4;
    }
    else
    {
        s = p / (2 * M_PI) * asin(valueDuration / a);
    }
    if (time < 1) return -.5 * (a * pow(2, 10 * (time -= 1)) * sin((time * timeDuration - s) * (2 * M_PI) / p)) + startValue;
    return a * pow(2, -10 * (time -= 1)) * sin((time * timeDuration - s) * (2 * M_PI) / p) * .5 + valueDuration + startValue;
}

double Easing::InBack(double time, double timeDuration, double startValue, double valueDuration)
{
    return valueDuration * (time /= timeDuration) * time * ((2.70158) * time - 1.70158) + startValue;
}

double Easing::OutBack(double time, double timeDuration, double startValue, double valueDuration)
{
    return valueDuration * ((time = time / timeDuration - 1) * time * ((2.70158) * time + 1.70158) + 1) + startValue;
}

double Easing::InOutBack(double time, double timeDuration, double startValue, double valueDuration)
{
    double s = 1.70158;
    if ((time /= timeDuration / 2) < 1) return valueDuration / 2 * (time * time * (((s *= (1.525)) + 1) * time - s)) + startValue;
    return valueDuration / 2 * ((time -= 2) * time * (((s *= (1.525)) + 1) * time + s) + 2) + startValue;
}

double Easing::InBounce(double time, double timeDuration, double startValue, double valueDuration)
{
    return valueDuration - OutBounce(timeDuration - time, 0, valueDuration, timeDuration) + startValue;
}

double Easing::OutBounce(double time, double timeDuration, double startValue, double valueDuration)
{
    if ((time /= timeDuration) < (1 / 2.75))
    {
        return valueDuration * (7.5625 * time * time) + startValue;
    }
    else if (time < (2 / 2.75))
    {
        return valueDuration * (7.5625 * (time -= (1.5 / 2.75)) * time + .75) + startValue;
    }
    else if (time < (2.5 / 2.75))
    {
        return valueDuration * (7.5625 * (time -= (2.25 / 2.75)) * time + .9375) + startValue;
    }
    else
    {
        return valueDuration * (7.5625 * (time -= (2.625 / 2.75)) * time + .984375) + startValue;
    }
}

double Easing::InOutBounce(double time, double timeDuration, double startValue, double valueDuration)
{
    if (time < timeDuration / 2) return InBounce(time * 2, 0, valueDuration, timeDuration) * .5 + startValue;
    return OutBounce(time * 2 - timeDuration, 0, valueDuration, timeDuration) * .5 + valueDuration * .5 + startValue;
}